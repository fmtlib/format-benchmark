cmake_minimum_required(VERSION 3.1)

# Joins arguments and places the results in ${result_var}.
function(join result_var)
  set(result )
  foreach (arg ${ARGN})
    set(result "${result}${arg}")
  endforeach ()
  set(${result_var} "${result}" PARENT_SCOPE)
endfunction()

# Set the default CMAKE_BUILD_TYPE to Release.
# This should be done before the project command since the latter can set
# CMAKE_BUILD_TYPE itself (it does so for nmake).
if (MASTER_PROJECT AND NOT CMAKE_BUILD_TYPE)
  set_verbose(CMAKE_BUILD_TYPE Release CACHE STRING
              "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or "
              "CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif ()

project(FORMAT_BENCHMARKS)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 17)

include(CheckCXXCompilerFlag)

if (NOT DEFINED INTEL)
  set(_IS_INTEL_HOST OFF)
  if (APPLE)
    execute_process(COMMAND sysctl -n machdep.cpu.brand_string
                    OUTPUT_VARIABLE out)
    if (out MATCHES "Intel.*")
      set(_IS_INTEL_HOST ON)
    endif ()
  else ()
    file(READ /proc/cpuinfo out)
    if (out MATCHES "(.|\n)*GenuineIntel(.|\n)*")
      set(_IS_INTEL_HOST ON)
    endif ()
  endif ()
  option(INTEL "Enable Intel JCC bug mitigation." ${_IS_INTEL_HOST})
endif ()

function(check_flags_and_append_on_success test_flags_var_name append_to_var)
  check_cxx_compiler_flag("${${test_flags_var_name}_FLAGS}"
                          ${test_flags_var_name}_WORKS)
  if (${test_flags_var_name}_WORKS)
    list(APPEND ${append_to_var} ${${test_flags_var_name}_FLAGS})
    set(${append_to_var} ${${append_to_var}} PARENT_SCOPE)
  endif ()
endfunction()

# Workaround a JCC bug in Intel CPUs:
# https://www.intel.com/content/dam/support/us/en/documents/processors/
# mitigations-jump-conditional-code-erratum.pdf
# to get more reliable benchmark results.
#
# Ideally we should use -mbranches-within-32B-boundaries but it's not widely
# available so at least align loops/functions as fallback to prevent unrelated
# code changes from affecting benchmark results.
set(ALIGN_ALL_BLOCKS_FLAGS -mllvm -align-all-blocks=5)
set(ALIGN_32B_BOUNDARIES_FLAGS -Wa,-mbranches-within-32B-boundaries)
set(ALIGN_FUNCTION_FLAGS -falign-functions=32)
check_flags_and_append_on_success(ALIGN_32B_BOUNDARIES ALIGN_OPTIONS)
if (NOT ALIGN_32B_BOUNDARIES_WORKS)
  check_flags_and_append_on_success(ALIGN_ALL_BLOCKS ALIGN_OPTIONS)
  check_flags_and_append_on_success(ALIGN_FUNCTION ALIGN_OPTIONS)
endif ()

message(STATUS "Align options: ${ALIGN_OPTIONS}")
add_definitions(${ALIGN_OPTIONS})

# Use shared libraries to make comparison with IOStreams and printf
# fair as these use shared libraries too (libstdc++ and libc).
set(BUILD_SHARED_LIBS ON CACHE BOOL
    "Build shared library instead of static one")
set(FMT_TEST TRUE CACHE BOOL "Enable fmt tests")
add_subdirectory(fmt)

find_package(Boost)

find_path(FOLLY_INCLUDE_DIR folly/Format.h)
find_library(FOLLY_LIB folly)
if (FOLLY_INCLUDE_DIR AND FOLLY_LIB)
  set(EXTRA_LIBS ${FOLLY_LIB})
endif ()

find_library(PROFILER_LIB profiler)
find_path(PROFILER_INCLUDE_DIR gperftools/profiler.h)
if (PROFILER_LIB AND PROFILER_INCLUDE_DIR)
  include_directories(${PROFILER_INCLUDE_DIR})
  set(HAVE_PROFILER TRUE)
endif ()

add_executable(tinyformat_speed_test src/tinyformat-test.cc)
target_link_libraries(tinyformat_speed_test fmt ${EXTRA_LIBS})
if (HAVE_PROFILER)
  target_link_libraries(tinyformat_speed_test ${PROFILER_LIB})
  set(PROFILE_DEFS ";FMT_PROFILE")
endif ()
if (TARGET Boost::boost)
  target_link_libraries(tinyformat_speed_test Boost::boost)
endif ()
set_target_properties(tinyformat_speed_test PROPERTIES COMPILE_DEFINITIONS
  "SPEED_TEST;HAVE_FORMAT;_SCL_SECURE_NO_WARNINGS;${PROFILE_DEFS}")
if (CPP11_FLAG)
  set_target_properties(tinyformat_speed_test
    PROPERTIES COMPILE_FLAGS ${CPP11_FLAG})
endif ()

if (WIN32)
  add_custom_target(speed-test
	COMMAND @echo running speed tests...
	COMMAND cd ${CMAKE_CFG_INTDIR}
	COMMAND @echo printf timings: start %time%
	COMMAND .\\tinyformat_speed_test.exe printf >NUL
	COMMAND @echo stop %time%
	COMMAND @echo iostreams timings: start %time%
	COMMAND .\\tinyformat_speed_test.exe iostreams >NUL
	COMMAND @echo stop %time%
	COMMAND @echo format timings: start %time%
	COMMAND .\\tinyformat_speed_test.exe format >NUL
	COMMAND @echo stop %time%
	COMMAND @echo tinyformat timings: start %time%
	COMMAND .\\tinyformat_speed_test.exe tinyformat >NUL
	COMMAND @echo stop %time%
	COMMAND @echo boost timings: start %time%
	COMMAND .\\tinyformat_speed_test.exe boost >NUL
	COMMAND @echo stop %time%
	COMMAND @echo stb_sprintf timings: start %time%
	COMMAND .\\tinyformat_speed_test.exe stb_sprintf >NUL
	COMMAND @echo stop %time%
	DEPENDS tinyformat_speed_test)
else()
  add_custom_target(speed-test
  COMMAND @echo running speed tests...
	COMMAND @echo printf timings:
	COMMAND @time -p ./tinyformat_speed_test printf > /dev/null
	COMMAND @echo iostreams timings:
	COMMAND @time -p ./tinyformat_speed_test iostreams > /dev/null
	COMMAND @echo format timings:
	COMMAND @time -p ./tinyformat_speed_test format > /dev/null
	COMMAND @echo fmt::compile timings:
	COMMAND @time -p ./tinyformat_speed_test fmt::compile > /dev/null
	COMMAND @echo tinyformat timings:
	COMMAND @time -p ./tinyformat_speed_test tinyformat > /dev/null
	COMMAND @echo boost timings:
	COMMAND @time -p ./tinyformat_speed_test boost > /dev/null
  COMMAND @echo folly timings:
  COMMAND @time -p ./tinyformat_speed_test folly > /dev/null
  COMMAND @echo stb_sprintf timings:
  COMMAND @time -p ./tinyformat_speed_test stb_sprintf > /dev/null
	DEPENDS tinyformat_speed_test)
endif ()

add_custom_target(bloat-test
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bloat-test.py -I${Boost_INCLUDE_DIRS}
                  DEPENDS fmt)

add_custom_target(variadic-test
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/variadic-test.py
                          \${ARGS} -I${Boost_INCLUDE_DIRS}
                  DEPENDS fmt)

enable_testing()
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests.")
add_subdirectory(benchmark)
add_subdirectory(digits10)
add_executable(vararg-benchmark src/vararg-benchmark.cc)
target_link_libraries(vararg-benchmark benchmark fmt)

add_executable(int-benchmark src/int-benchmark.cc)
target_link_libraries(int-benchmark benchmark fmt)
if (TARGET Boost::boost)
  target_link_libraries(int-benchmark Boost::boost)
endif ()

target_compile_features(int-benchmark PRIVATE cxx_relaxed_constexpr)

add_executable(locale-benchmark src/locale-benchmark.cc)
target_link_libraries(locale-benchmark benchmark fmt)

add_executable(parse-benchmark src/parse-benchmark.cc)
target_link_libraries(parse-benchmark benchmark fmt)

add_executable(concat-benchmark src/concat-benchmark.cc)
target_link_libraries(concat-benchmark benchmark fmt)

add_executable(file-benchmark src/file-benchmark.cc)
target_link_libraries(file-benchmark benchmark fmt)

add_executable(find-pow10-benchmark src/find-pow10-benchmark.cc)
target_link_libraries(find-pow10-benchmark benchmark)

add_subdirectory(src/itoa-benchmark)
