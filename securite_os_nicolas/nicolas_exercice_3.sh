#!/bin/bash

#exercice 3 

list_files=$(grep -rl 'toto' .| grep -v 'nicolas_exercice_3.sh')
echo "$list_files"
