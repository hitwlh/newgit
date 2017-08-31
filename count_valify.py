#python count_valify.py --src=/home/fjywade/traces/msr-cowcache/mds_0.csv --iosize=3000000
import os
import sys
import argparse


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='manual to this script')
  parser.add_argument('--src', type=str, default='/Users/wanglonghua/Desktop/ddd/new/test.txt')
  parser.add_argument('--dst', type=str, default='/Users/wanglonghua/Desktop/ddd/new/test_result.txt')
  parser.add_argument('--iosize', type=int, default=1000000)
  args = parser.parse_args()
  src = open(args.src, 'r')
  if args.dst != '/Users/wanglonghua/Desktop/ddd/new/test_result.txt':
    dst = open(args.dst, 'w')
  else:
    dst = open(args.src+'.data','w')
  line = src.readline()
  dict = {};
  lines = 0
  total_read_4k = 0
  total_write_4k = 0
  unique_write_4k = 0
  unique_write_64k = 0
  full_64k = 0
  #while line and lines < args.iosize:
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
      dst.write(w_r+' '+str(start)+' 4096'+'\n')
      #if w_r == "W" and (not dict.has_key(start/65536)):
      if w_r == "W":
        total_write_4k += 1
        if not dict.has_key(start/65536):
          dict[start/65536] = [0] * 16
          unique_write_64k += 1
        #print start
        if dict[start/65536][(start%65536)/4096] != 1:
          unique_write_4k += 1
        dict[start/65536][(start%65536)/4096] = 1
      else:
        total_read_4k += 1
      start += 4096
    line = src.readline()
    lines += 1
  for i in dict: 
    if sum(dict[i]) == 16:
      full_64k += 1
  print "lines = ", lines
  print "total read = ", total_read_4k, "(", round(float(total_read_4k)*4096/1000000, 2), "MB, ", round(float(total_read_4k)*4096/1000000000, 2), "GB), 4k based"
  print "total write = ", total_write_4k, " (", round(float(total_write_4k)*4096/1000000, 2), "MB, ", round(float(total_write_4k)*4096/1000000000, 2), "GB), 4k based"
  print "unique write = ", unique_write_4k, " (", round(float(unique_write_4k)*4096/1000000, 2), "MB, ", round(float(unique_write_4k)*4096/1000000000, 2), "GB), 4k based"
  print "unique write = ", unique_write_64k, " (", round(float(unique_write_64k)*65536/1000000, 2), "MB, ", round(float(unique_write_64k)*65536/1000000000, 2), "GB), 64k based"
  print "cow = ", round(float(unique_write_64k) / total_write_4k, 4)
  print "fully writed clusters = ", full_64k

  src.close()
