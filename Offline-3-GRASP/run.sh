#!/bin/bash

# compile
g++ -O3 -w 1905066.cpp -o 1905066.out

echo "Problem,,,,,Constructive algorithm,,,Local search,,,GRASP,,Known Best Solution
Name, |V| or n, |E| or m, min edge, max edge, Simple Randomized, Simple Greedy, Semi Greedy, Avg iterations, Avg improvements, Avg value, Iterations, Best Value, Best Solution" > Report.csv


for i in {1..54}
do
    ./1905066.out "Graphs/g$i".rud >> Report.csv
    echo "Graph $i done"
done

