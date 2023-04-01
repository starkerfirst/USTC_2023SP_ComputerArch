#!/bin/python

import random
import argparse
import sys
import numpy as np

parser = argparse.ArgumentParser(description='Generate random arrays for spmv')
parser.add_argument('-r','--rows', default=8192, type=int, help='Number of Rows')
parser.add_argument('-c','--cols', default=65536, type=int, help='Number of Cols')
parser.add_argument('-e','--avg_elem_per_row', default=64, type=int, help='Elements in each row')
parser.add_argument('-s','--dev_elem_per_row', default=32, type=int, help='Elements in each row')

parser.add_argument('-v','--range', default=96, type=int, help='Upper Bound on Random Value -- 2 for Binary')
parser.add_argument('-d', '--data_type', default='float')
#parser.add_argument('--non_random',action='store_true')
parser.add_argument('-o','--output', default='spmvArr.h')

args = parser.parse_args()

raw_elem_per_row = np.random.normal(args.avg_elem_per_row,args.dev_elem_per_row,args.rows)
elem_per_row = [max(0,min(int(x),args.cols-1)) for x in raw_elem_per_row]
elem_per_row = [0] + elem_per_row
nnz = sum(elem_per_row)

row_delim = np.cumsum(elem_per_row)


out_file = open(args.output,'w')

out_file.write("#define N_COLS " + str(args.cols)+"\n")
out_file.write("#define N_ROWS " + str(args.rows)+"\n")
out_file.write("#define NNZ " + str(nnz)+"\n")


# VAL ARRAY (nnz)
out_file.write(args.data_type + " val[] = {\n")
for i in range(0,nnz):
  out_file.write(str(random.randint(0,args.range-1)))
  if i!=nnz-1:
    out_file.write(",")

  if i % 20 == 19:
    out_file.write("\n")

out_file.write("};\n\n")

# COLS ARRAY (nnz)
out_file.write("int cols[] = {\n")
for i in range(1,len(elem_per_row)):
  num_elem_in_row=elem_per_row[i]
  samples = random.sample(range(0,args.cols),num_elem_in_row)
  samples.sort()
  
  for j,val in enumerate(samples):
    out_file.write(str(val))
    if not (j==len(samples)-1 and i ==len(elem_per_row)-1):
     out_file.write(",")

  out_file.write("\n")
out_file.write("};\n\n")

# ROW DELIM ARRAY
out_file.write("int row_delim[] = {\n")
for i,val in enumerate(row_delim): 
  out_file.write(str(val))
  if i!=len(row_delim)-1:
    out_file.write(",")

  if i % 20 == 19:
    out_file.write("\n")

out_file.write("};\n\n")

# Input Vec Array
out_file.write(args.data_type + " vec[] = {\n")
for i in range(0,args.cols):
  out_file.write(str(random.randint(0,args.range-1)))
  if i!=args.cols-1:
    out_file.write(",")

  if i % 20 == 19:
    out_file.write("\n")

out_file.write("};\n\n")

