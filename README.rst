A collection of formatting benchmarks
=====================================

* Speed, compile time and code bloat tests from
  `tinyformat <https://github.com/c42f/tinyformat>`__.
* ``int-benchmark``: decimal integer to string conversion benchmark

Building and running ``int-benchmark``:

.. code::

   cmake .
   make
   ./int-benchmarks

.. code::

    2020-06-14 10:11:37
    Running ./int-benchmark
    Run on (8 X 2800 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x4)
      L1 Instruction 32K (x4)
      L2 Unified 262K (x4)
      L3 Unified 8388K (x1)
    Load Average: 2.39, 2.42, 2.95
    -------------------------------------------------------------------------------
    Benchmark                     Time             CPU   Iterations UserCounters...
    -------------------------------------------------------------------------------
    sprintf                68372567 ns     68256111 ns            9 items_per_second=14.6507M/s
    ostringstream         153144783 ns    152921250 ns            4 items_per_second=6.53931M/s
    std_to_string          15296922 ns     15285522 ns           46 items_per_second=65.4214M/s
    std_to_chars           11491346 ns     11484224 ns           58 items_per_second=87.076M/s
    fmt_to_string          14156613 ns     14147469 ns           49 items_per_second=70.684M/s
    format_runtime         19865740 ns     19854886 ns           35 items_per_second=50.3654M/s
    format_compile         13912813 ns     13903729 ns           48 items_per_second=71.9232M/s
    format_to_runtime      12231571 ns     12226714 ns           56 items_per_second=81.7881M/s
    format_to_compile       8894683 ns      8890792 ns           77 items_per_second=112.476M/s
    format_int              8081654 ns      8074035 ns           85 items_per_second=123.854M/s
    lexical_cast           28552634 ns     28542458 ns           24 items_per_second=35.0355M/s
    boost_format          297975272 ns    297871500 ns            2 items_per_second=3.35715M/s
    boost_karma_generate   11993005 ns     11986000 ns           59 items_per_second=83.4307M/s
    voigt_itostr           19096746 ns     19087568 ns           37 items_per_second=52.3901M/s
    decimal_from           10543687 ns     10538833 ns           60 items_per_second=94.8872M/s
    stout_ltoa             37787807 ns     37767789 ns           19 items_per_second=26.4776M/s

   
