#!/bin/bash

if [ -n "$1" ]; then
    echo "IP is set"
else
    echo "IP is not set"
    exit 1
fi

IP=$1
DATA_SIZES=(64 128 256 512 1024)

echo "RESULTS $IP"

for size in "${DATA_SIZES[@]}"; do
    ./client $IP $size
done