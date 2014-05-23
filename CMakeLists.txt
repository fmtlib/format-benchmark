cmake_minimum_required(VERSION 2.6)

project(FORMAT_BENCHMARKS)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 HAVE_STD_CPP11_FLAG)

# Use shared cppformat library to make comparison with IOStreams and printf
# fair as these use shared libraries too (libstdc++ and libc).
set(FMT_SHARED ON CACHE BOOL "Build shared library instead of static one")
add_subdirectory(cppformat)

find_package(Boost)
if (Boost_FOUND)
  if (HAVE_STD_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif ()
  add_executable(int-generator int_generator.cpp ltoa.c)
  target_link_libraries(int-generator format)
  find_library(HAVE_RT rt)
  if (HAVE_RT)
    target_link_libraries(int-generator rt)
  endif ()
  add_definitions(-DHAVE_BOOST)
endif ()

add_custom_target(bloat-test COMMAND ./bloat-test.py DEPENDS format)
