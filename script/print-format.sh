#!/bin/bash
# Author: Sukjoon Oh, sjoon@kaist.ac.kr
#

print_info() {
    sign='\033[0;32m[BASH:INFO]\033[0m'
    printf "${sign} $1 \n"
}

print_warn() {
    sign='\033[0;31m[BASH:WARN]\033[0m'
    printf "${sign} $1 \n"
}