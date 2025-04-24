#!/bin/bash

read -p "Entrez le nombre d'étages pour la pyramide : " n

for (( i=1; i<=n; i++ ))
do
    for (( j=i; j<n; j++ ))
    do
        echo -n " " 
    done

    for (( k=1; k<=(2*i-1); k++ ))
    # for (( k=0; k<2*i; k++ )) est peut être plus clair
    do
        echo -n "*"
    done

    echo ""
done


