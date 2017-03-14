#!/bin/bash
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    echo "./test.sh <levels>"
    echo "Max level allowed is 5"
    exit -1
fi

if [ "$1" -lt "1" ]
  then
    echo "Minimum 1 level required"
    echo "./test.sh <levels>"
    echo "Max level allowed is 5"
    exit -1
fi

if [ "$1" -gt "5" ]
  then
    echo "Maximum 5 levels allowed"
    echo "./test.sh <levels>"
    exit -1
fi

./test $1 &
pid=$!
sleep 1
echo "Format: <process>(pid) {<thread>}(pid)"
pstree -p $pid
