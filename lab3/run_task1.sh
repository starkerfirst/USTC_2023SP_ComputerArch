#!/bin/bash
# WARNING: This script is just a test script, of course it is not the best script.

policies=("NMRU" "LIP" "LRU" "Random")
association=("1" "2" "4" "8" "16")


# task 1
# mkdir ../../lab3/task1
for policy in "${policies[@]}";
    do for asso in "${association[@]}";
            # run the configuration
        do echo "Running $policy $asso";
            build/X86/gem5.opt configs/ca/lab3.py --association $asso --policy $policy;
            cp m5out/stats.txt ../../lab3/task1/$policy-$asso.txt;
            cp m5out/config.ini ../../lab3/task1/$policy-$asso.ini;        
        done
    done

