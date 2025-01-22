#!/bin/bash

# find . -name Makefile -execdir make -f Makefile.local $1\; 

folders=$(find . -type d)

echo $folders | while read folder; do
    cd $folder
    if [ -f "Makefile.local" ]; then
        echo "----------------"
        echo "Makefile trouvé dans $folder"
        make -f Makefile.local $1
    fi
    cd -
done