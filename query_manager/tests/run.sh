#!/bin/bash

BIN_DIR=test_bins

if [ $# -eq 0 ]; then
    echo "Usage: ./run.sh <binary>"
    exit 1
fi

./$BIN_DIR/$1
