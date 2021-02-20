#!/bin/bash
cd ../Unsolved/
g++ generatecases.cpp -o generatecases
./generatecases
cd ../Solver/
g++ realizabilityChecker.cpp -Ofast -o realizabilityChecker
for i in {1..23}
do
   ./realizabilityChecker $i &
done
