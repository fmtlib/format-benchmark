add_executable(digits10-benchmark digits10.cc digits10-benchmark.h digits10-benchmark.cc)
target_link_libraries(digits10-benchmark benchmark)

add_executable(digits10-test digits10.cc digits10-test.cc)
target_link_libraries(digits10-test gmock benchmark)
add_test(digits10-test digits10-test)
