#!/bin/bash
# WARNING: This script is just a test script, of course it is not the best script.

policies=("NMRU" "LIP" "LRU" "Random")
association=("1" "2" "4" "8" "16")

# task 2
# mkdir ../../lab3/task2
for policy in "${policies[@]}";
    do for asso in "${association[@]}";
        # run the configuration
        do  if [ $policy == "NMRU" ] && [ $asso -le 8 ]; 
            then
                echo "Running $policy $asso";
                build/X86/gem5.opt configs/ca/lab3.py --association $asso --lookup_time 500 --policy $policy;
                cp m5out/stats.txt ../../lab3/task2/$policy-$asso.txt;
                cp m5out/config.ini ../../lab3/task2/$policy-$asso.ini;   
            fi 
            if [ $policy == "Random" ] && [ $asso -le 16 ]; 
            then
                echo "Running $policy $asso";
                build/X86/gem5.opt configs/ca/lab3.py --association $asso --lookup_time 100 --policy $policy;
                cp m5out/stats.txt ../../lab3/task2/$policy-$asso.txt;
                cp m5out/config.ini ../../lab3/task2/$policy-$asso.ini;   
            fi 
            if [ $policy == "LIP" ] && [ $asso -le 8 ]; 
            then
                echo "Running $policy $asso";
                build/X86/gem5.opt configs/ca/lab3.py --association $asso --lookup_time 555 --policy $policy;
                cp m5out/stats.txt ../../lab3/task2/$policy-$asso.txt;
                cp m5out/config.ini ../../lab3/task2/$policy-$asso.ini;   
            fi 
        done
    done