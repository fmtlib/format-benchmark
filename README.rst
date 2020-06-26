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

Sample results on macOS with clang and libc++:

.. code::

    2020-06-25 18:35:12
    Running ./int-benchmark
    Run on (8 X 2800 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x4)
      L1 Instruction 32K (x4)
      L2 Unified 262K (x4)
      L3 Unified 8388K (x1)
    Load Average: 3.05, 2.27, 2.05
    --------------------------------------------------------------------------------
    Benchmark                      Time             CPU   Iterations UserCounters...
    --------------------------------------------------------------------------------
    sprintf                 62015560 ns     61939300 ns           10 items_per_second=16.1448M/s
    std_ostringstream      153347073 ns    153167000 ns            4 items_per_second=6.52882M/s
    std_to_string           15058924 ns     15052045 ns           44 items_per_second=66.4362M/s
    std_to_chars            10995660 ns     10991238 ns           63 items_per_second=90.9816M/s
    fmt_to_string           11909200 ns     11905632 ns           57 items_per_second=83.9939M/s
    fmt_format_runtime      17071781 ns     17062878 ns           41 items_per_second=58.6068M/s
    fmt_format_compile      11897635 ns     11893517 ns           58 items_per_second=84.0794M/s
    fmt_format_to_runtime   13539784 ns     13534137 ns           51 items_per_second=73.8872M/s
    fmt_format_to_compile    8941199 ns      8937675 ns           77 items_per_second=111.886M/s
    fmt_format_int           8721323 ns      8718253 ns           79 items_per_second=114.702M/s
    boost_lexical_cast      29685237 ns     29668455 ns           22 items_per_second=33.7058M/s
    boost_format           315875676 ns    315739000 ns            2 items_per_second=3.16717M/s
    boost_karma_generate    12138668 ns     12134518 ns           56 items_per_second=82.4095M/s
    voigt_itostr            20177849 ns     19994243 ns           37 items_per_second=50.0144M/s
    u2985907                 9445288 ns      9427174 ns           69 items_per_second=106.076M/s
    decimal_from            13009338 ns     12952232 ns           56 items_per_second=77.2068M/s
    stout_ltoa              39336210 ns     39325000 ns           18 items_per_second=25.4291M/s


