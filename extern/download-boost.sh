#!/bin/bash
# Author: Sukjoon Oh, sjoon@kaist.ac.kr

if [[ -v WORKING_PATH ]]; then
    source $WORKING_PATH/script/print-format.sh
    print_info "Correct working path set" 
else
    printf "Incorrect working path set"
    exit
fi


download_url="https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz"
file_name="boost_1_86_0.tar.gz"

rm temp/${file_name}

print_info "Downloading boost: ${download_url}"

wget ${download_url} -P temp
tar -zxvf temp/${file_name} -C extern/

print_info "Download boost complete"
