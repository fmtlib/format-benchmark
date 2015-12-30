#!/usr/bin/env python

# Script to test how much bloating a large project will suffer when using
# different formatting methods.
# Based on bloat_test.sh from https://github.com/c42f/tinyformat.

from __future__ import print_function, division

import argparse
import os
import pickle
import re
import sys
from glob import glob
from subprocess import check_call, Popen, PIPE
from timeit import timeit


parser = argparse.ArgumentParser()
subparsers = parser.add_subparsers(help='possible commands', dest='command')

parser_bench = subparsers.add_parser('bench', help='run the benchmark')
parser_bench.add_argument('min', type=int, help='minimum number of arguments')
parser_bench.add_argument('max', type=int, help='maximum number of arguments')
parser_bench.add_argument('num_translation_units', metavar='N', type=int,
                          help='number of translation units')

parser_plot = subparsers.add_parser('plot', help='plot the results')
parser_plot.add_argument('--filename', type=str, default='variadic-test.pkl',
                         help='bench result file path')

parser_plotdiff = subparsers.add_parser(
  'plotdiff', help='plot the difference between result files'
)
parser_plotdiff.add_argument('files', type=str, nargs='*',
                             help='result files to be compared')
parser_plotdiff.add_argument('--method', type=str, default='C++ Format',
                             help='formatting library')
parser_plotdiff.add_argument('--config', type=str, default='optimized',
                             help='optimized or debug')

options, more_compiler_flags = parser.parse_known_args(sys.argv[1:])

if 'plot' in options.command:
  import numpy as np
  import matplotlib.pyplot as plt
  import seaborn as sns


prefix = '_variadic_test_tmp_'
NUM_RUNS = 3
use_clobber = False

configs = [
  ('optimized', ['-O3', '-DNDEBUG']),
  ('debug',     [])
]

methods = [
  ('printf'      , []),
  ('IOStreams'   , ['-DUSE_IOSTREAMS']),
  ('C++ Format'  , ['-DUSE_CPPFORMAT', '-Lcppformat', '-lcppformat']),
  ('tinyformat'  , ['-DUSE_TINYFORMAT']),
  ('Boost Format', ['-DUSE_BOOST'])
]

method_templates = {
  'boost': {
    'statement': r'std::cout << boost::format("{fmt_str}\n") % {args};',
    'sep': ' % ',
    'specifier': '%{type}',
  },
  'cppformat': {
    'statement': r'fmt::print("{fmt_str}\n", {args});',
    'sep': ', ',
    'specifier': '{{:{type}}}',
  },
  'iostream': {
    'statement': r'std::cout << {args} << "\n";',
    'sep': ' << ":" << ',
    'specifier': '',
  },
  'tinyformat': {
    'statement': r'tfm::printf("{fmt_str}\n", {args});',
    'sep': ', ',
    'specifier': '%{type}',
  },
  'printf': {
    'statement': r'::printf("{fmt_str}\n", {args});',
    'sep': ', ',
    'specifier': '%{type}',
  }
}

main_template = r'''
#ifdef USE_BOOST

#include <boost/format.hpp>
#include <iostream>

{boost}

#elif defined(USE_CPPFORMAT)

#include "cppformat/format.h"

{cppformat}

#elif defined(USE_IOSTREAMS)

#include <iostream>

{iostream}

#elif defined(USE_TINYFORMAT)

#include "tinyformat.h"

{tinyformat}

#else

#include <stdio.h>

{printf}

#endif
'''


def make_format_string(method, args):
  specifiers = [method['specifier'].format(type=a[1]) for a in args]
  return ':'.join(specifiers)


def make_statement(method, args):
  d = {'args': method['sep'].join(a[0] for a in args)}
  if method['specifier']:
    d['fmt_str'] = make_format_string(method, args)

  return method['statement'].format(**d)


def generate_args(start_n):
  n = start_n
  while True:
    args = [(str(n), 'd'),
            (str(float(n)), '.1f'),
            ('"String{}"'.format(n), 's'),
            ('i', 'd'),
            ('f', '.1f'),
            ('s', 's')]
    for a in args:
      yield a
    n += 1


def make_function(func_def, method, n, num_args):
  from itertools import islice
  mul = 5
  args = list(islice(generate_args(n), 2 * mul * num_args))

  statements = [make_statement(method, args[shift:shift + num_args])
                for shift in range(mul * num_args)]

  if use_clobber:
    sep = ' asm volatile("" : : : "memory");\n  '
  else:
    sep = '\n  '

  function = (func_def + ' {{\n'
              '  {}\n'
              '}}').format(sep.join(statements))
  return function


def make_template(func_def, n, num_args):
  functions = {k: make_function(func_def, v, n, num_args)
               for k, v in method_templates.items()}
  return main_template.format(**functions)


class Table:
  """Prints a reStructuredText table"""

  def __init__(self, header, formats):
    self.widths = [len(i) for i in header]
    self.formats = formats

    self.print_rulers()
    for field, width in zip(header, self.widths):
      print(self.format_field(field, '', width), end=' ')
    print()
    self.print_rulers()

  @staticmethod
  def format_field(field, fmt='', width=''):
    return '{:{}{}}'.format(field, width, fmt)

  def print_rulers(self):
    for w in self.widths:
      print('=' * w, end=' ')
    print()

  def print_row(self, *row):
    for field, fmt, width in zip(row, self.formats, self.widths):
      print(self.format_field(field, fmt, width), end=' ')
    print()


def to_kib(n):
  """Converts n to kibibytes"""
  return int(round(n / 1024.0))


def remove_old_files():
  filenames = glob(prefix + '??.cc')
  for f in [prefix + 'main.cc', prefix + 'all.h']:
    if os.path.exists(f):
      filenames.append(f)
  for f in filenames:
    os.remove(f)


def generate_files(num_args):
  main_source = prefix + 'main.cc'
  main_header = prefix + 'all.h'
  sources = [main_source]
  with open(main_source, 'w') as cppfile, open(main_header, 'w') as hppfile:
    cppfile.write(re.sub('^ +', '', '''\
      #include "{}all.h"
      #ifdef USE_IOSTREAMS
      # include <iostream>
      #endif

      int main() {{
      #ifdef USE_IOSTREAMS
        std::cout.setf(std::ios::fixed);
        std::cout.precision(1);
      #endif
      '''.format(prefix), 0, re.MULTILINE))
    for i in range(options.num_translation_units):
      n = '{:03}'.format(i)
      func_name = 'doFormat_a' + n
      func_params = '(int i, float f, const char* s)'
      func_def = 'void ' + func_name + func_params
      source = prefix + n + '.cc'
      sources.append(source)

      with open(source, 'w') as f:
        f.write(make_template(func_def, i, num_args))

      cppfile.write(func_name + '(1, 1.0f, "String");\n')
      hppfile.write(func_def + ';\n')
    cppfile.write('}')

  return sources


def find_compiler():
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

  return compiler_path


def measure_compile(compiler_path, sources, flags):
  """Measure compile time and executable size"""
  output_filename = prefix + '.out'
  if os.path.exists(output_filename):
    os.remove(output_filename)

  include_dir = '-I' + os.path.dirname(os.path.realpath(__file__))
  command = 'check_call({})'.format(
    [compiler_path, '-std=c++11', '-o', output_filename,
     include_dir] + sources + flags + more_compiler_flags
  )

  result = {
    'time': timeit(command, number=1,
                   setup='from subprocess import check_call'),
    'size': os.stat(output_filename).st_size
  }

  check_call(['strip', output_filename])
  result['stripped_size'] = os.stat(output_filename).st_size

  p = Popen(['./' + output_filename], stdout=PIPE,
            env={'LD_LIBRARY_PATH': 'cppformat'})
  result['output'] = p.communicate()[0]
  sys.stdout.flush()

  return result


def bench_single(num_args, flags):
  remove_old_files()
  sources = generate_files(num_args)
  compiler_path = find_compiler()

  result = {}
  for i in range(NUM_RUNS):
    sys.stdout.flush()

    old_result = result
    result = measure_compile(compiler_path, sources, flags)

    if 'time' not in old_result:
      continue

    result['time'] = min(old_result['time'], result['time'])
    if any(result[k] != old_result[k] for k in ('size', 'stripped_size')):
      raise Exception('size mismatch')

  return result


def bench(method, config, flags):
  print('Benchmarking', config, method)
  table = Table(
    ['Args', 'Compile time, s', 'Executable size, KiB', 'Stripped size, KiB'],
    ['', '.1f', '', '']
  )
  results = []
  for num_args in range(options.min, options.max):
    result = bench_single(num_args, flags)
    table.print_row(num_args, result['time'], to_kib(result['size']),
                    to_kib(result['stripped_size']))
    results.append(result)
  table.print_rulers()
  print()
  return results


def check_output(expected_list, actual_list):
  for expected, actual in zip(expected_list, actual_list):
    if expected['output'] != actual['output']:
      print(expected['output'])
      print(actual['output'])
      raise Exception("output doesn't match")


def bench_command():
  data = {'options': options}
  for method, method_flags in methods:
    data[method] = {config: bench(method, config, method_flags + config_flags)
                    for config, config_flags in configs}

    if 'printf' in data:
      for config, _ in configs:
        check_output(data['printf'][config], data[method][config])

  for method, _ in methods:
    for config, _ in configs:
      for result in data[method][config]:
        del result['output']

  with open('variadic-test.pkl', 'wb') as file:
    pickle.dump(data, file)


def load_data(filename):
  if not filename.endswith('.pkl'):
    filename += '.pkl'

  with open(filename, 'rb') as file:
    return pickle.load(file)


def set_plot_style():
  sns.set_style("ticks", rc={'xtick.direction': 'in', 'ytick.direction': 'in'})
  sns.set_palette('Set1')


def plot_title(s):
  plt.annotate(s, (0.05, 0.9), xycoords='axes fraction',
               fontsize=14, horizontalalignment='left',
               bbox=dict(boxstyle="round,pad=0.2", fc='white'))


def plot_subfigure(x, y, prop, **kwargs):
  zorder = 3 if kwargs.get('label') == 'C++ Format' else 2
  plt.plot(x, y, marker='o', markersize=6, zorder=zorder, **kwargs)
  plt.axvline(16, ls='--', color='grey')

  xmax = max(x) + 1
  plt.xlim(0, xmax)
  plt.xticks(np.arange(0, xmax, 4))
  plt.grid(color='k', alpha=0.4, ls=':')
  sns.despine()

  plt.xlabel('number of arguments')
  if prop == 'time':
    plt.ylabel('compile time (s)')
  else:
    plt.ylabel('binary size (MiB)')


def plot_all(filename, prop):
  data = load_data(filename)
  x = np.arange(data['options'].min, data['options'].max)

  def make_y(results):
    y = [result[prop] for result in results]
    if prop == 'size':
      y = np.array(y) / 1024 / 1024
    return y

  set_plot_style()
  plt.figure(figsize=(8, 3.5))

  plt.subplot('121')
  plot_title('release')
  for method, _ in methods:
    plot_subfigure(x, make_y(data[method]['optimized']), prop, label=method)

  plt.legend(loc='upper center', bbox_to_anchor=(1.05, 1.17),
             ncol=5, fontsize=11)

  plt.subplot('122')
  plot_title('debug')
  for method, _ in methods:
    plot_subfigure(x, make_y(data[method]['debug']), prop, label=method)

  plt.suptitle('variadic-test', fontsize=16, y=1.1)
  plt.savefig('variadic-test_{}.png'.format(prop), bbox_inches='tight')


def plot_command():
  for prop in 'time', 'size':
    plot_all(options.filename, prop)


def plot_diff(filenames, method, config, prop):
  dataset = [load_data(f) for f in filenames]
  baseline = dataset[0][method][config]
  x = np.arange(dataset[0]['options'].min, dataset[0]['options'].max)

  set_plot_style()
  plt.figure(figsize=(8, 3.5))

  plt.subplot('121')
  plot_title('absolute')
  for name, data in zip(filenames, dataset):
    y = [result[prop] for result in data[method][config]]
    if prop == 'size':
      y = np.array(y) / 1024 / 1024
    plot_subfigure(x, y, prop, label=name)

  plt.legend(loc='upper center', bbox_to_anchor=(1.05, 1.17),
             ncol=5, fontsize=11)

  plt.subplot('122')
  plot_title('relative')
  for name, data in zip(filenames, dataset):
    results = data[method][config]
    y = [result[prop] / base[prop] for result, base in zip(results, baseline)]
    plot_subfigure(x, y, prop, label=name)

  ylim = plt.ylim()
  plt.ylim(ylim[0] * 0.8, ylim[1] * 1.2)
  ax = plt.gca()
  vals = ax.get_yticks()
  ax.set_yticklabels(['{:3.0%}'.format(x) for x in vals])
  plt.ylabel('')

  plt.suptitle('variadic-diff', fontsize=16, y=1.1)
  plt.savefig('variadic-diff_{}.png'.format(prop), bbox_inches='tight')


def plotdiff_command():
  for prop in 'time', 'size':
    plot_diff(options.files, options.method, options.config, prop)


commands = {
  'bench': bench_command,
  'plot': plot_command,
  'plotdiff': plotdiff_command,
}

if __name__ == '__main__':
  commands[options.command]()
