#!/usr/bin/env python

# Script to test how much bloating a large project will suffer when using
# different formatting methods.  Call as
#
# C99 printf            :  bloat-test.py [-O3]
# C++ Format            :  bloat-test.py [-O3] -DUSE_CPPFORMAT
# tinyformat            :  bloat-test.py [-O3] -DUSE_TINYFORMAT
# boost::format         :  bloat-test.py [-O3] -DUSE_BOOST
# std::iostream         :  bloat-test.py [-O3] -DUSE_IOSTREAMS
#
# Based on bloat_test.sh from https://github.com/c42f/tinyformat.

from __future__ import print_function
import os, re, sys
from contextlib import nested
from glob import glob
from subprocess import check_call
from timeit import timeit

template = '''
#ifdef USE_BOOST

#include <boost/format.hpp>
#include <iostream>

void doFormat_a() {
  std::cout << boost::format("%s\\n") % "somefile.cpp";
  std::cout << boost::format("%s:%d\\n") % "somefile.cpp" % 42;
  std::cout << boost::format("%s:%d:%s\\n") % "somefile.cpp" % 42 % "asdf";
  std::cout <<
    boost::format("%s:%d:%d:%s\\n") % "somefile.cpp" % 42 % 1 % "asdf";
  std::cout <<
    boost::format("%s:%d:%d:%d:%s\\n") % "somefile.cpp" % 42 % 1 % 2 % "asdf";
}

#elif defined(USE_CPPFORMAT)

#include "../format.h"

void doFormat_a() {
  fmt::Print("{}\\n", "somefile.cpp");
  fmt::Print("{}:{}\\n", "somefile.cpp", 42);
  fmt::Print("{}:{}:{}\\n", "somefile.cpp", 42, "asdf");
  fmt::Print("{}:{}:{}:{}\\n", "somefile.cpp", 42, 1, "asdf");
  fmt::Print("{}:{}:{}:{}:{}\\n", "somefile.cpp", 42, 1, 2, "asdf");
}

#elif defined(USE_IOSTREAMS)

#include <iostream>

void doFormat_a() {
  std::cout << "somefile.cpp" << "\\n";
  std::cout << "somefile.cpp" << 42 << "\\n";
  std::cout << "somefile.cpp" << 42 << "asdf" << "\\n";
  std::cout << "somefile.cpp" << 42 << 1 << "asdf" << "\\n";
  std::cout << "somefile.cpp" << 42 << 1 << 2 << "asdf" << "\\n";
}

#else
# ifdef USE_TINYFORMAT
#   include "tinyformat.h"
#   define PRINTF tfm::printf
# else
#   include <stdio.h>
#   define PRINTF ::printf
# endif

void doFormat_a() {
  PRINTF("%s\\n", "somefile.cpp");
  PRINTF("%s:%d\\n", "somefile.cpp", 42);
  PRINTF("%s:%d:%s\\n", "somefile.cpp", 42, "asdf");
  PRINTF("%s:%d:%d:%s\\n", "somefile.cpp", 42, 1, "asdf");
  PRINTF("%s:%d:%d:%d:%s\\n", "somefile.cpp", 42, 1, 2, "asdf");
}
#endif
'''

prefix = '_bloat_test_tmp_'
num_translation_units = 100

# Remove old files.
filenames = glob(prefix + '??.cc')
for f in [prefix + 'main.cc', prefix + 'all.h']:
  if os.path.exists(f):
    filenames.append(f)
for f in filenames:
  os.remove(f)

# Generate all the files.
main_source = prefix + 'main.cc'
main_header = prefix + 'all.h'
sources = [main_source, main_header]
with nested(open(main_source, 'w'), open(main_header, 'w')) as \
     (main_file, header_file):
  main_file.write(re.sub('^ +', '', '''
    #include "{}all.h"

    int main() {{
    '''.format(prefix), 0, re.MULTILINE))
  for i in range(num_translation_units):
    n = '{:03}'.format(i)
    func_name = 'doFormat_a' + n
    source = prefix + n + '.cc'
    sources.append(source)
    with open(source, 'w') as f:
      f.write(template.replace('doFormat_a', func_name).replace('42', str(i)))
    main_file.write(func_name + '();\n')
    header_file.write('void ' + func_name + '();\n')
  main_file.write('}')

def get_size_kb(filename):
  return int(round(os.stat(output_filename).st_size / 1024.0))

# Compile.
# TODO: check that g++ is not a cache
output_filename = prefix + '.out'
if os.path.exists(output_filename):
  os.remove(output_filename)
command = 'check_call({})'.format(
  ['g++', '-std=c++11', '-o', output_filename] + sys.argv[1:] + sources)
print('Compile time:',
  timeit(command, setup = 'from subprocess import check_call', number = 1))
print('Size:', get_size_kb(output_filename), 'KiB')
check_call(['strip', output_filename])
print('Stripped size:', get_size_kb(output_filename), 'KiB')
