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

    2020-06-13 17:32:29
    Running ./int-benchmark
    Run on (8 X 2800 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x4)
      L1 Instruction 32K (x4)
      L2 Unified 262K (x4)
      L3 Unified 8388K (x1)
    Load Average: 3.39, 3.87, 3.17
    -------------------------------------------------------------------------------
    Benchmark                     Time             CPU   Iterations UserCounters...
    -------------------------------------------------------------------------------
    sprintf                61441509 ns     61327000 ns           11 items_per_second=16.306M/s
    ostringstream         153424504 ns    153049250 ns            4 items_per_second=6.53384M/s
    to_string              15079086 ns     15068333 ns           45 items_per_second=66.3643M/s
    to_chars               11545883 ns     11533672 ns           61 items_per_second=86.7027M/s
    format                 19669498 ns     19658235 ns           34 items_per_second=50.8693M/s
    format_compile         14194653 ns     14186939 ns           49 items_per_second=70.4874M/s
    format_to              12766521 ns     12759778 ns           54 items_per_second=78.3713M/s
    format_to_compile       9351341 ns      9344743 ns           74 items_per_second=107.012M/s
    format_int              9083547 ns      9078132 ns           76 items_per_second=110.155M/s
    lexical_cast           28611769 ns     28595520 ns           25 items_per_second=34.9705M/s
    boost_format          314370003 ns    313796500 ns            2 items_per_second=3.18678M/s
    boost_karma_generate   12417178 ns     12409218 ns           55 items_per_second=80.5853M/s
    voigt_itostr           18646865 ns     18639459 ns           37 items_per_second=53.6496M/s
    decimal_from           10705138 ns     10696781 ns           64 items_per_second=93.4861M/s
    stout_ltoa             38064232 ns     38032833 ns           18 items_per_second=26.2931M/s

   
