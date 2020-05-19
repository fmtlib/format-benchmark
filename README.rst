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

  2019-12-05 17:42:51
  Running ./int-benchmark
  Run on (4 X 3500 MHz CPU s)
  CPU Caches:
    L1 Data 32K (x2)
    L1 Instruction 32K (x2)
    L2 Unified 262K (x2)
    L3 Unified 4194K (x1)
  Load Average: 2.54, 4.70, 5.89
  -------------------------------------------------------------------------------
  Benchmark                     Time             CPU   Iterations UserCounters...
  -------------------------------------------------------------------------------
  sprintf                65568024 ns     65138818 ns           11 items_per_second=15.3518M/s
  ostringstream         167387783 ns    166251750 ns            4 items_per_second=6.01497M/s
  to_string              89624630 ns     89081500 ns            8 items_per_second=11.2257M/s
  format                 35467244 ns     35385700 ns           20 items_per_second=28.26M/s
  format_to              23738393 ns     23682133 ns           30 items_per_second=42.2259M/s
  format_to_compile       8250425 ns      8235280 ns           82 items_per_second=121.429M/s
  format_int              8115486 ns      8092583 ns           84 items_per_second=123.57M/s
  lexical_cast           31042594 ns     30936609 ns           23 items_per_second=32.3242M/s
  boost_format          338535980 ns    337967000 ns            2 items_per_second=2.95887M/s
  boost_karma_generate   12525130 ns     12458764 ns           55 items_per_second=80.2648M/s
  voigt_itostr           17066540 ns     17032610 ns           41 items_per_second=58.7109M/s
  decimal_from           10044434 ns     10027072 ns           69 items_per_second=99.73M/s
  stout_ltoa             40864801 ns     40756647 ns           17 items_per_second=24.5359M/s 
   
