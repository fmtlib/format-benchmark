cmake_minimum_required(VERSION 2.6)

# Set the default CMAKE_BUILD_TYPE to Release.
# This should be done before the project command since the latter can set
# CMAKE_BUILD_TYPE itself (it does so for nmake).
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif ()

project(FORMAT_BENCHMARKS)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 HAVE_STD_CPP11_FLAG)
if (HAVE_STD_CPP11_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

# Use shared libraries to make comparison with IOStreams and printf
# fair as these use shared libraries too (libstdc++ and libc).
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared library instead of static one")
add_subdirectory(cppformat)

find_package(Boost)
if (Boost_FOUND)
  add_definitions(-DHAVE_BOOST)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(int-generator int_generator.cpp ltoa.c)
  target_link_libraries(int-generator cppformat)
  find_library(HAVE_RT rt)
  if (HAVE_RT)
    target_link_libraries(int-generator rt)
  endif ()
endif ()

find_library(PROFILER_LIB profiler)
find_path(PROFILER_INCLUDE_DIR gperftools/profiler.h)
if (PROFILER_LIB AND PROFILER_INCLUDE_DIR)
  include_directories(${PROFILER_INCLUDE_DIR})
  set(HAVE_PROFILER TRUE)
endif ()

add_executable(tinyformat_speed_test tinyformat_test.cpp)
target_link_libraries(tinyformat_speed_test cppformat)
if (HAVE_PROFILER)
  target_link_libraries(tinyformat_speed_test ${PROFILER_LIB})
  set(PROFILE_DEFS ";FMT_PROFILE")
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
	COMMAND @echo tinyformat timings:
	COMMAND @time -p ./tinyformat_speed_test tinyformat > /dev/null
	COMMAND @echo boost timings:
	COMMAND @time -p ./tinyformat_speed_test boost > /dev/null
	DEPENDS tinyformat_speed_test)
endif()

add_custom_target(bloat-test COMMAND ./bloat-test.py DEPENDS cppformat)
