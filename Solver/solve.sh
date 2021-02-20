#!/bin/bash
g++ realizabilityChecker.cpp -Ofast -o realizabilityChecker
for i in {1..23}
do
   ./realizabilityChecker $i &
done
