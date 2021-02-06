#!/bin/bash
g++ determineGaps.cpp -Ofast -o determineGaps
for i in {1..23}
do
   ./determineGaps $i &
done
