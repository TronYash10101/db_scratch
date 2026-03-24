#!/usr/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <target_name> [mode]"
    exit 1
fi

BIN=$1
MODE=$2

if [[ "$BIN" == "schm_test" && -z "$MODE" ]]; then
    echo -e "Give mode for this test:\n0 = don't wipe schema\n1 = wipe whole schema"
    exit 1
fi

./test_bins/"${BIN}" "${MODE}"
