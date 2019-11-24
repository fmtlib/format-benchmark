#!/usr/bin/env python

from __future__ import print_function
import re, subprocess, sys

class Stat:
  def __init__(self):
    self.num_runs = 0
    self.time = float('inf')

  def __repr__(self):
    return str.format('[{} {}]', self.num_runs, self.time)

stats = {}

# Do 3 runs and select the best time out of 3 for each method.
num_runs = 3
for i in range(num_runs):
  proc = subprocess.Popen(['./int-generator'], stdout=subprocess.PIPE)
  while True:
    line = proc.stdout.readline()
    if line == '':
      break
    m = re.match(r'(.+):\t+([0-9.]+).*\n', line)
    if m:
      name, time = m.group(1), float(m.group(2))
      print(name, time)
      stat = stats.setdefault(name, Stat())
      stat.num_runs += 1
      stat.time = min(time, stat.time)
    else:
      print(line, end='')

print('\nFinal results:')
sorted_stats = sorted(stats.iteritems(), key=lambda i: i[1].time);
best_time = sorted_stats[0][1].time
print("[{:30}, {:10}, {:13}],".format("'Method'", "'Time, s'", "'Time ratio'"))
for name, stat in sorted_stats:
  if stat.num_runs != num_runs:
    print("Invalid number of runs for", name)
    sys.exit(1)
  print("[{:30}, {:10}, {:13}],".format("'" + name + "'", stat.time, stat.time / best_time))

