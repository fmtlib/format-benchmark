#!/usr/bin/env python

# Script to test how much bloating a large project will suffer when using
# different formatting methods.
# Based on bloat_test.sh from https://github.com/c42f/tinyformat.

from __future__ import print_function
import os, re, sys
from contextlib import nested
from glob import glob
from subprocess import check_call, Popen, PIPE
from timeit import timeit

template = r'''
#ifdef USE_BOOST

#include <boost/format.hpp>
#include <iostream>

void doFormat_a() {
  std::cout << boost::format("%s\n") % "somefile.cpp";
  std::cout << boost::format("%s:%d\n") % "somefile.cpp" % 42;
  std::cout << boost::format("%s:%d:%s\n") % "somefile.cpp" % 42 % "asdf";
  std::cout <<
    boost::format("%s:%d:%d:%s\n") % "somefile.cpp" % 42 % 1 % "asdf";
  std::cout <<
    boost::format("%s:%d:%d:%d:%s\n") % "somefile.cpp" % 42 % 1 % 2 % "asdf";
}

#elif USE_FOLLY

#include <folly/Format.h>
#include <iostream>

void doFormat_a() {
  std::cout << folly::format("{}\n", "somefile.cpp");
  std::cout << folly::format("{}:{}\n", "somefile.cpp", 42);
  std::cout << folly::format("{}:{}:{}\n", "somefile.cpp", 42, "asdf");
  std::cout <<
    folly::format("{}:{}:{}:{}\n", "somefile.cpp", 42, 1, "asdf");
  std::cout <<
    folly::format("{}:{}:{}:{}:{}\n", "somefile.cpp", 42, 1, 2, "asdf");
}

#elif defined(USE_FMT)

#include "fmt/core.h"

void doFormat_a() {
  fmt::print("{}\n", "somefile.cpp");
  fmt::print("{}:{}\n", "somefile.cpp", 42);
  fmt::print("{}:{}:{}\n", "somefile.cpp", 42, "asdf");
  fmt::print("{}:{}:{}:{}\n", "somefile.cpp", 42, 1, "asdf");
  fmt::print("{}:{}:{}:{}:{}\n", "somefile.cpp", 42, 1, 2, "asdf");
}

#elif defined(USE_IOSTREAMS)

#include <iostream>

void doFormat_a() {
  std::cout << "somefile.cpp" << "\n";
  std::cout << "somefile.cpp:" << 42 << "\n";
  std::cout << "somefile.cpp:" << 42 << ":asdf" << "\n";
  std::cout << "somefile.cpp:" << 42 << ':' << 1 << ":asdf" << "\n";
  std::cout << "somefile.cpp:" << 42 << ':' << 1 << ':' << 2 << ":asdf" << "\n";
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
  PRINTF("%s\n", "somefile.cpp");
  PRINTF("%s:%d\n", "somefile.cpp", 42);
  PRINTF("%s:%d:%s\n", "somefile.cpp", 42, "asdf");
  PRINTF("%s:%d:%d:%s\n", "somefile.cpp", 42, 1, "asdf");
  PRINTF("%s:%d:%d:%d:%s\n", "somefile.cpp", 42, 1, 2, "asdf");
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
sources = [main_source]
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

# Measure compile time and executable size.
expected_output = None
def benchmark(flags):
  output_filename = prefix + '.out'
  if os.path.exists(output_filename):
    os.remove(output_filename)
  include_dir = '-I' + os.path.dirname(os.path.realpath(__file__))
  command = 'check_call({})'.format(
    [compiler_path, '-std=c++14', '-o', output_filename, include_dir] + sources + flags)
  result = Result()
  result.time = timeit(
    command, setup = 'from subprocess import check_call', number = 1)
  print('Compile time: {:.2f}s'.format(result.time))
  result.size = os.stat(output_filename).st_size
  print('Size: {}'.format(result.size))
  check_call(['strip', output_filename])
  result.stripped_size = os.stat(output_filename).st_size
  print('Stripped size: {}'.format(result.stripped_size))
  p = Popen(['./' + output_filename], stdout=PIPE,
            env={'LD_LIBRARY_PATH': 'fmt'})
  output = p.communicate()[0]
  global expected_output
  if not expected_output:
    expected_output = output
  elif output != expected_output:
    raise Exception("output doesn't match")
  sys.stdout.flush()
  return result

configs = [
  ('optimized', ['-O3', '-DNDEBUG']),
  ('debug',     [])
]

fmt_library = 'fmt/libfmt.so'
if not os.path.exists(fmt_library):
  fmt_library = fmt_library.replace('.so', '.dylib')

methods = [
  ('printf'      , []),
  ('IOStreams'   , ['-DUSE_IOSTREAMS']),
  ('fmt'         , ['-DUSE_FMT', '-Ifmt/include', fmt_library]),
  ('tinyformat'  , ['-DUSE_TINYFORMAT']),
  ('Boost Format', ['-DUSE_BOOST']),
  ('Folly Format', ['-DUSE_FOLLY', '-lfolly'])
]

def format_field(field, format = '', width = ''):
  return '{:{}{}}'.format(field, width, format)

def print_rulers(widths):
  for w in widths:
    print('=' * w, end = ' ')
  print()

# Prints a reStructuredText table.
def print_table(table, *formats):
  widths = [len(i) for i in table[0]]
  for row in table[1:]:
    for i in range(len(row)):
      widths[i] = max(widths[i], len(format_field(row[i], formats[i])))
  print_rulers(widths)
  row = table[0]
  for i in range(len(row)):
    print(format_field(row[i], '', widths[i]), end = ' ')
  print()
  print_rulers(widths)
  for row in table[1:]:
    for i in range(len(row)):
      print(format_field(row[i], formats[i], widths[i]), end = ' ')
    print()
  print_rulers(widths)

# Converts n to kibibytes.
def to_kib(n):
  return int(round(n / 1024.0))

NUM_RUNS = 3
for config, flags in configs:
  results = {}
  for i in range(NUM_RUNS):
    for method, method_flags in methods:
      print('Benchmarking', config, method)
      sys.stdout.flush()
      new_result = benchmark(flags + method_flags + sys.argv[1:])
      if method not in results:
        results[method] = new_result
        continue
      old_result = results[method]
      old_result.time = min(old_result.time, new_result.time)
      if new_result.size != old_result.size or \
         new_result.stripped_size != old_result.stripped_size:
        raise Exception('size mismatch')
  print(config, 'Results:')
  table = [
    ('Method', 'Compile Time, s', 'Executable size, KiB', 'Stripped size, KiB')
  ]
  for method, method_flags in methods:
    result = results[method]
    table.append(
      (method, result.time, to_kib(result.size), to_kib(result.stripped_size)))
  print_table(table, '', '.1f', '', '')
