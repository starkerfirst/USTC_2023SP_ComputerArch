#!/bin/bash
# we transverse the following configurations of SIZE and BLOCK_SIZE
for size in $(seq 6 2 10)
do
    for block_size in $(seq 3 1 5)
    do
        echo "SIZE: $size, BLOCK_SIZE: $block_size"
        gcc -O3 -mavx -DSIZE=$size -DBLOCK_SIZE=$block_size cpu.c -o cpu 
        ./cpu >> cpu.txt
    done
done
