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

# Find compiler.
compiler_path = None
for path in os.getenv('PATH').split(os.pathsep):
  filename = os.path.join(path, 'g++')
  if os.path.exists(filename):
    if os.path.islink(filename) and \
       os.path.basename(os.path.realpath(filename)) == 'ccache':
      # Don't use ccache.
      print('Ignoring ccache link at', filename)
      continue
    compiler_path = filename
    break
print('Using compiler', filename)

class Result:
  pass

# Compile.
def benchmark(flags):
  output_filename = prefix + '.out'
  if os.path.exists(output_filename):
    os.remove(output_filename)
  command = 'check_call({})'.format(
    [compiler_path, '-std=c++11', '-o', output_filename] + sources + flags)
  result = Result()
  result.time = timeit(
    command, setup = 'from subprocess import check_call', number = 1)
  print('Compile time: {:.2f}s'.format(result.time))
  result.size = os.stat(output_filename).st_size
  print('Size: {}B'.format(result.size))
  check_call(['strip', output_filename])
  result.stripped_size = os.stat(output_filename).st_size
  print('Stripped size: {}B'.format(result.stripped_size))
  sys.stdout.flush()
  return result

configs = [
  ('Debug',     []),
  ('Optimized', ['-O3'])
]

methods = [
  ('printf'      , []),
  ('IOStreams'   , ['-DUSE_IOSTREAMS']),
  ('C++ Format'  , ['-DUSE_CPPFORMAT', '-L..', '-lformat']),
  ('tinyformat'  , ['-DUSE_TINYFORMAT']),
  ('Boost Format', ['-DUSE_BOOST'])
]

NUM_RUNS = 3
for config, flags in configs:
  results = {}
  for i in range(NUM_RUNS):
    for method, method_flags in methods:
      print('Benchmarking', config, method)
      sys.stdout.flush()
      new_result = benchmark(flags + method_flags)
      if method not in results:
        results[method] = new_result
        continue
      old_result = results[method]
      old_result.time = min(old_result.time, new_result.time)
      if new_result.size != old_result.size or \
         new_result.stripped_size != old_result.stripped_size:
        raise Exception('size mismatch')
  print(config, 'Results:')
  for method, method_flags in methods:
    result = results[method]
    print(method, result.time, result.size, result.stripped_size)
