#!/bin/bash

if [[ -v WORKING_PATH ]]; then
    source $WORKING_PATH/script/print-format.sh
    print_info "Correct working path set" 
else
    printf "Incorrect working path set"
    exit
fi

rm sequence-freqs.csv

# gdb -ex run --args ./build/bin/pduck-cache-lfu -i 10

# ./build/bin/pduck-query-extender \
#     --number 1000000 \
#     --dimension 100 \
#     --type INT8 \
#     --input-query $WORKING_PATH/dataset/spacev1b/query_log.bin \
#     --output-query $WORKING_PATH/dataset/spacev1b/query_log_extended.bin \

./build/bin/pduck-query-extender \
    --number 1000000 \
    --dimension 100 \
    --type INT8 \
    --input-query $WORKING_PATH/dataset/spacev1b/query_log.bin \
    --output-query $WORKING_PATH/temp/query_extended.bin \
    --input-gt $WORKING_PATH/dataset/spacev1b/query_log_gtgen.bin \
    --output-gt $WORKING_PATH/temp/truth_extended.bin \




