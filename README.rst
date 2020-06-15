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

	2020-06-15 11:12:14
	Running ./int-benchmark
	Run on (8 X 2800 MHz CPU s)
	CPU Caches:
	L1 Data 32K (x4)
	L1 Instruction 32K (x4)
	L2 Unified 262K (x4)
	L3 Unified 8388K (x1)
	Load Average: 2.79, 2.42, 2.09
	-------------------------------------------------------------------------------
	Benchmark                     Time             CPU   Iterations UserCounters...
	-------------------------------------------------------------------------------
	sprintf                61791924 ns     61719273 ns           11 items_per_second=16.2024M/s
	ostringstream         154815487 ns    154604000 ns            4 items_per_second=6.46814M/s
	std_to_string          15329374 ns     15314065 ns           46 items_per_second=65.2994M/s
	std_to_chars           11174077 ns     11166258 ns           62 items_per_second=89.5555M/s
	fmt_to_string          11847305 ns     11840276 ns           58 items_per_second=84.4575M/s
	format_runtime         19862675 ns     19855914 ns           35 items_per_second=50.3628M/s
	format_compile         11823433 ns     11816000 ns           58 items_per_second=84.631M/s
	format_to_runtime      13048756 ns     13043604 ns           53 items_per_second=76.6659M/s
	format_to_compile       9069190 ns      9063973 ns           75 items_per_second=110.327M/s
	format_int              8699864 ns      8694291 ns           79 items_per_second=115.018M/s
	lexical_cast           30010119 ns     29992750 ns           24 items_per_second=33.3414M/s
	boost_format          320914919 ns    320660500 ns            2 items_per_second=3.11856M/s
	boost_karma_generate   12260277 ns     12253893 ns           56 items_per_second=81.6067M/s
	voigt_itostr           18697344 ns     18688703 ns           37 items_per_second=53.5083M/s
	decimal_from           10555579 ns     10549531 ns           64 items_per_second=94.7909M/s
	stout_ltoa             40028853 ns     40010824 ns           17 items_per_second=24.9932M/s

Sample results on Linux with gcc and libstdc++:

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

   
