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

    2020-06-13 07:53:52
    Running ./int-benchmark
    Run on (8 X 2800 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x4)
      L1 Instruction 32K (x4)
      L2 Unified 262K (x4)
      L3 Unified 8388K (x1)
    Load Average: 2.25, 2.19, 2.56
    -------------------------------------------------------------------------------
    Benchmark                     Time             CPU   Iterations UserCounters...
    -------------------------------------------------------------------------------
    sprintf                63208047 ns     63063727 ns           11 items_per_second=15.857M/s
    ostringstream         152176979 ns    151975600 ns            5 items_per_second=6.58M/s
    to_string              14826185 ns     14798865 ns           52 items_per_second=67.5727M/s
    format                 18501672 ns     18484917 ns           36 items_per_second=54.0982M/s
    format_compile         13791866 ns     13770143 ns           49 items_per_second=72.6209M/s
    format_to              11845735 ns     11830586 ns           58 items_per_second=84.5267M/s
    format_to_compile       8059886 ns      8051549 ns           82 items_per_second=124.2M/s
    format_int              5530719 ns      5524033 ns          123 items_per_second=181.027M/s
    lexical_cast           28058332 ns     28017920 ns           25 items_per_second=35.6914M/s
    boost_format          323923950 ns    323532000 ns            2 items_per_second=3.09088M/s
    boost_karma_generate   11482504 ns     11472267 ns           60 items_per_second=87.1667M/s
    voigt_itostr           16637843 ns     16606341 ns           41 items_per_second=60.218M/s
    decimal_from            9433401 ns      9418366 ns           71 items_per_second=106.176M/s
    stout_ltoa             36457529 ns     36436263 ns           19 items_per_second=27.4452M/s

   
