#!/bin/bash

#exercice 2

for file in *.Tab; do
  mv "$file" "$file.tex"
done
