#!/bin/bash


if [ -z "$1" ]; then
  echo "Usage: $0 doit contenir un ficher en argument"
  exit 1
fi

grep -v '\.' "$1" > temp_file && mv temp_file "$1"
echo " les points ont été retirés"