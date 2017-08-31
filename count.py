#python count.py --src=/home/fjywade/traces/msr-cowcache/mds_0.csv --iosize=3000000
import os
import sys
import argparse

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='manual to this script')
  parser.add_argument('--src', type=str, default='/Users/wanglonghua/Desktop/ddd/new/test.txt')
  parser.add_argument('--iosize', type=int, default=1000000)
  args = parser.parse_args()
  src = open(args.src, 'r')
  line = src.readline()
  unique_block = 0
  dict = {-1: -1};
  lines = 0
  while line and lines < args.iosize:
    n1, n2, n3, w_r, offset, iosize, n7 = line.split(',')
    if w_r == "read" or w_r == "Read" or w_r == "READ":
      w_r = "R"
    elif w_r == "write" or w_r == "Write" or w_r == "WRITE":
      w_r = "W"
    else:
      print w_r
      exit(0)
    start = int(offset)
    end = start+int(iosize) #[start, end)
    if start % 4096 != 0:
      start = start - start % 4096
    if end % 4096 != 0:
      end = end - end % 4096 + 4096
    while start < end:
      if w_r == "W" and (not dict.has_key(start/65536)):
        unique_block += 1
        print start
        dict[start/65536] = -1
      start += 4096
    line = src.readline()
    lines += 1
  print "unique_block = ", unique_block
  print "lines = ", lines
  src.close()
