#!/bin/bash

if [[ -v WORKING_PATH ]]; then
    source $WORKING_PATH/script/print-format.sh
    print_info "Correct working path set" 
else
    printf "Incorrect working path set"
    exit
fi

rm cache*csv
rm request-dump.csv

gdb -ex run --args ./build/bin/pduck-cache-lfu -i 10