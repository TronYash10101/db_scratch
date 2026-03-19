#!/usr/bin/bash

if [ $# -eq 0 ]; then
	echo "Mention target name in front of this file"
	exit 1
fi

BIN=$1

./test_bins/"${BIN}"
