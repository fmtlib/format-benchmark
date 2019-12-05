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

   2019-12-05 15:48:29
   Running ./int-benchmark
   Run on (4 X 3500 MHz CPU s)
   CPU Caches:
   L1 Data 32K (x2)
   L1 Instruction 32K (x2)
   L2 Unified 262K (x2)
   L3 Unified 4194K (x1)
   Load Average: 2.07, 2.84, 3.22
   -------------------------------------------------------------------------------
   Benchmark                     Time             CPU   Iterations UserCounters...
   -------------------------------------------------------------------------------
   sprintf               672127867 ns    668650000 ns            1 items_per_second=14.9555M/s
   ostringstream        1666452973 ns   1659734000 ns            1 items_per_second=6.02506M/s
   to_string             893241039 ns    888744000 ns            1 items_per_second=11.2518M/s
   format                357718258 ns    352311000 ns            2 items_per_second=28.384M/s
   format_to             246231548 ns    243193333 ns            3 items_per_second=41.1195M/s
   format_to_compile      81436084 ns     81192125 ns            8 items_per_second=123.165M/s
   format_int             81342646 ns     81180625 ns            8 items_per_second=123.182M/s
   lexical_cast          295065135 ns    293862500 ns            2 items_per_second=34.0295M/s
   boost_format         3412564591 ns   3382437000 ns            1 items_per_second=2.95645M/s
   boost_karma_generate  126400731 ns    126235000 ns            5 items_per_second=79.2173M/s
   voigt_itostr          170224535 ns    169919000 ns            4 items_per_second=58.8516M/s
   decimal_from           97890019 ns     97524000 ns            7 items_per_second=102.539M/s
   stout_ltoa            376954591 ns    375572500 ns            2 items_per_second=26.626M/s
   
