#!/bin/bash

#exercice 5

echo -n "   " 
for i in {1..9}
do
  printf "%-3d" $i
done
echo 

for i in {1..9}
do
  
  printf "%-3d" $i
  for j in {1..9}
  do
    printf "%-3d" $((i * j))
  done
  echo
done
