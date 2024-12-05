/*
 * lru.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 *  Test file for the LRU cache implementation.
 */

#include <algorithm>
#include <random>

#include <fstream>
#include <memory>
#include <string>

#include "utils/Logger.hh"
#include "utils/ArgParser.hh"

#include "extender/VectorReader.hh"
#include "extender/YcsbSeqGenerator.hh"

// 
// Command:
// ./build/bin/pduck-query-extender -n 1000000 -d 100 -p "./dataset/spacev1b/query.bin" -t INT8
int main(int argc, char* argv[])
{

    // Generate Sequence
    // Load the vectors
    std::unique_ptr<pduck::extender::VectorReader> reader = std::make_unique<pduck::extender::VectorReader>();

    // Load the vectors
    reader->loadVectors("");

}