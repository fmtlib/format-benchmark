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

