#!/bin/bash
# WARNING: This script is just a test script, of course it is not the best script.

benchmarks=("mm" "spmv" "lfsr" "merge" "sieve")
CPU=("DerivO3CPU" "MinorCPU")
IssueWidth=(8 2)
Frequency=("1GHz" "4GHz")
L2Cache=("0kB" "256kB" "2MB" "16MB")

# big loop
for bench in "${benchmarks[@]}";
    #do mkdir ../../lab2/$bench;
    do for cpu in "${CPU[@]}";
        do for issue in "${IssueWidth[@]}";
            do for freq in "${Frequency[@]}";
                do for l2 in "${L2Cache[@]}";
                    # check if the configuration is valid
                    do counter=0;
                    if [ $cpu == "DerivO3CPU" ]; 
                    then counter=$((counter+1));
                    fi
                    if [ $l2 == "0kB" ]; 
                    then counter=$((counter+1));
                    fi
                    if [ $issue -eq 8 ]; 
                    then counter=$((counter+1));
                    fi
                    if [ $freq == "1GHz" ]; 
                    then counter=$((counter+1));
                    fi
                    if [ $counter -lt 3 ]; 
                    then continue;
                    #echo "Skipping $cpu $issue $freq $l2";
                    fi
                    # run the configuration
                    echo "Running $cpu $issue $freq $l2 $bench";
                    build/X86/gem5.opt configs/ca/lab2.py -c ../../lab2/cs251a-microbench-master/$bench --cpu_type=$cpu \
                    --issue_width=$issue --cpu_clock=$freq --l2_size=$l2;
                    cp m5out/stats.txt ../../lab2/$bench/$cpu-$issue-$freq-$l2.txt;
                    done
                done
            done
        done
    done
