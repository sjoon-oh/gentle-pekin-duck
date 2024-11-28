#!/bin/bash
# Author: Sukjoon Oh, sjoon@kaist.ac.kr
#

# export PATH=/opt/rh/devtoolset-9/root/usr/bin:$PATH
# export LD_LIBRARY_PATH=/opt/rh/devtoolset-9/root/usr/lib64:/opt/rh/devtoolset-9/root/usr/lib:/opt/rh/devtoolset-9/root/usr/lib64/dyninst:/opt/rh/devtoolset-9/root/usr/lib/dyninst:/opt/rh/devtoolset-9/root/usr/lib64:/opt/rh/devtoolset-9/root/usr/lib

# Directory must match.
export WORKING_PROJ=gentle-pekin-duck

echo -e "Current working path: `pwd`"
if [[ "$(basename $PWD)" == "$WORKING_PROJ" ]]; then
    export WORKING_PATH=`pwd`
    echo -e "WORKING_PATH: $WORKING_PATH \n"
else
    echo -e "Current working path not set\n"
fi

git submodule update --init
