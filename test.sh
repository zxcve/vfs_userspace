#!/bin/bash
./test $1 &
pid=$!
sleep 1
pstree -pg $pid
