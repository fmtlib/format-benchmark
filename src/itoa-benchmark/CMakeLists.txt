
add_executable(
    itoa-benchmark
    amartin.cpp
    branchlut.cpp
    branchlut2.cpp
    count.cpp
    countdecimaldigit.h
    countlut.cpp
    digitslut.h
    folly.cpp
    itoa_jeaiii.cpp
    itoa_jeaiii_bind.cpp
    itoa_ljust.h
    itoa_ljust_impl.h
    lut.cpp
    main.cpp
    msinttypes
    mwilson.cpp
    naive.cpp
    null.cpp
    ostringstream.cpp
    ostrstream.cpp
    resultfilename.h
    sprintf.cpp
    sse2.cpp
    test.h
    timer.h
    tmueller.cpp
    to_string.cpp
    unnamed.cpp
    unrolledlut.cpp
    vc.cpp)

if (APPLE)
  execute_process(COMMAND sysctl -n machdep.cpu.brand_string
                  OUTPUT_VARIABLE out)
elseif(UNIX)
  file(READ /proc/cpuinfo out)
  string(REGEX MATCH "(model name[^\n]*)" out "${out}")
endif ()

string(REGEX REPLACE
        "(model name.*: )|[ |\n]+|(Intel\\(R\\))|\\(TM\\)|\\(R\\)|CPU|@" ""
        out "${out}")
if(out)
  target_compile_definitions(itoa-benchmark PRIVATE MACHINE="${out}")
endif()