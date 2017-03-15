#!/bin/bash

#Check for arguments if not null
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    echo "./test.sh <levels>"
    echo "Max level allowed is 5"
    exit -1
fi

# Change range for argument
if [ "$1" -lt "1" ]
  then
    echo "Minimum 1 level required"
    echo "./test.sh <levels>"
    echo "Max level allowed is 5"
    exit -1
fi

# Change range for argument
if [ "$1" -gt "5" ]
  then
    echo "Maximum 5 levels allowed"
    echo "./test.sh <levels>"
    exit -1
fi

#run test with the supplied level
./test $1 &

#extract pid
pid=$!

#sleep for 1second to populate process hierarchy
sleep 1

echo "Format: <process>(pid) {<thread>}(pid)"
#print the structure of the process tree
pstree -p $pid
