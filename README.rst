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



Other benchmarks running on provided by fast_io author. On Linux with libstdc++, GCC-11 C++20

cqwrteur@DESKTOP-C4VAUFM:~/format-benchmark/build$ ./int-benchmark
The number of values by digit count:
 1  27518
 2 246950
 3 450053
 4 247697
 5  25016
 6   2491
 7    249
 8     25
 9      0
10      1
2020-06-21T08:21:27-04:00
Running ./int-benchmark
Run on (12 X 3593.26 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 512 KiB (x6)
  L3 Unified 16384 KiB (x1)
Load Average: 0.31, 0.09, 0.17
--------------------------------------------------------------------------------
Benchmark                      Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------
sprintf                 66180655 ns     66180815 ns           11 items_per_second=15.1101M/s
std_ostringstream       59197433 ns     59196831 ns           12 items_per_second=16.8928M/s
std_to_string           14227102 ns     14226740 ns           49 items_per_second=70.2902M/s
std_to_chars            10778731 ns     10778754 ns           64 items_per_second=92.7751M/s
fmt_to_string           15449511 ns     15449548 ns           46 items_per_second=64.7268M/s
fmt_format_runtime      25347564 ns     25347609 ns           28 items_per_second=39.4515M/s
fmt_format_compile      15521973 ns     15522015 ns           45 items_per_second=64.4246M/s
fmt_format_to_runtime   19493339 ns     19493384 ns           36 items_per_second=51.2995M/s
fmt_format_to_compile   11964826 ns     11964854 ns           58 items_per_second=83.5781M/s
fmt_format_int           9702564 ns      9702586 ns           72 items_per_second=103.065M/s
boost_lexical_cast      29437537 ns     29437599 ns           24 items_per_second=33.9702M/s
boost_format           259534333 ns    259534899 ns            3 items_per_second=3.85305M/s
boost_karma_generate    10422500 ns     10422529 ns           67 items_per_second=95.946M/s
voigt_itostr            16333445 ns     16333491 ns           42 items_per_second=61.2239M/s
u2985907                10527758 ns     10527785 ns           66 items_per_second=94.9867M/s
decimal_from            11788081 ns     11788109 ns           59 items_per_second=84.8312M/s
stout_ltoa              21071739 ns     21071791 ns           33 items_per_second=47.4568M/s
fast_io_concat          11085544 ns     11085565 ns           63 items_per_second=90.2074M/s
fast_io_print_reserve    9506848 ns      9506866 ns           73 items_per_second=105.187M/s


concat benchmark

cqwrteur@DESKTOP-C4VAUFM:~/format-benchmark/build$ ./concat-benchmark
2020-06-21T08:49:23-04:00
Running ./concat-benchmark
Run on (12 X 3593.26 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 512 KiB (x6)
  L3 Unified 16384 KiB (x1)
Load Average: 1.65, 1.57, 0.68
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
naive                   87.3 ns         87.3 ns      7980351
append                  61.1 ns         61.1 ns     11319732
appendWithReserve       41.8 ns         41.8 ns     16762382
format_compile          74.8 ns         74.8 ns      9358126
format_runtime           112 ns          112 ns      6235314
format_to               84.3 ns         84.3 ns      8146667
fast_io_print           23.3 ns         23.3 ns     30091614
fast_io_concat          66.4 ns         66.4 ns     10546989
nullop                 0.252 ns        0.252 ns   1000000000


It looks fmt's benchmark deliberately ruins cache locality of jiaendu algorithm


cqwrteur@DESKTOP-C4VAUFM:~/format-benchmark/build$ ./int-benchmark-in-order
The number of values by digit count:
 1     10
 2     90
 3    900
 4   9000
 5  90000
 6 900000
 7      0
 8      0
 9      0
10      0
2020-06-21T10:08:22-04:00
Running ./int-benchmark-in-order
Run on (12 X 3593.26 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 512 KiB (x6)
  L3 Unified 16384 KiB (x1)
Load Average: 0.42, 0.28, 0.55
--------------------------------------------------------------------------------
Benchmark                      Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------
sprintf                 62320291 ns     62320479 ns           11 items_per_second=16.0461M/s
std_ostringstream       54647454 ns     54646801 ns           13 items_per_second=18.2993M/s
std_to_string            9350224 ns      9350126 ns           75 items_per_second=106.95M/s
std_to_chars             7519653 ns      7519667 ns           93 items_per_second=132.985M/s
fmt_to_string           11011169 ns     11011201 ns           62 items_per_second=90.8166M/s
fmt_format_runtime      22369400 ns     22369443 ns           31 items_per_second=44.7038M/s
fmt_format_compile      11017577 ns     11017598 ns           64 items_per_second=90.7639M/s
fmt_format_to_runtime   17233278 ns     17233323 ns           40 items_per_second=58.0271M/s
fmt_format_to_compile    7646626 ns      7646645 ns           92 items_per_second=130.776M/s
fmt_format_int           5806645 ns      5806657 ns          121 items_per_second=172.216M/s
boost_lexical_cast      24628418 ns     24628471 ns           28 items_per_second=40.6034M/s
boost_format           226440000 ns    226440589 ns            3 items_per_second=4.41617M/s
boost_karma_generate     9217050 ns      9217077 ns           76 items_per_second=108.494M/s
voigt_itostr            10767260 ns     10767286 ns           65 items_per_second=92.8739M/s
u2985907                 4781043 ns      4781053 ns          147 items_per_second=209.159M/s
decimal_from            10713470 ns     10713493 ns           66 items_per_second=93.3402M/s
stout_ltoa              26098837 ns     26098883 ns           27 items_per_second=38.3158M/s
fast_io_concat          10248196 ns     10248215 ns           68 items_per_second=97.578M/s
fast_io_print_reserve    5300714 ns      5300725 ns          133 items_per_second=188.653M/s


