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
    std::unique_ptr<pduck::extender::YcsbSeqGenerator> seqGenerator 
        = std::make_unique<pduck::extender::YcsbSeqGenerator>();

    std::uint64_t recordCount = 10000000;
    std::string distribution = "zipfian";

    // We start by the number of recordCount, but we extend it to the number by 1.
    std::uint64_t extendedRecordCount = recordCount;
    std::vector<std::pair<std::uint64_t, size_t>> sequenceIdsByFreq;

    seqGenerator->resetGenerator();
    seqGenerator->setGenerator(extendedRecordCount, distribution);

    pduck::utils::Logger::getInstance().getLogger()->info("Starting to generate a sequence of size {}", recordCount);

    std::vector<std::uint64_t>& sequence = seqGenerator->generateSequence(1000000);
    size_t uniqueKeys = seqGenerator->checkUniqueIds(sequenceIdsByFreq);

    pduck::utils::Logger::getInstance().getLogger()->info("New record count ({}), unique count ({})", extendedRecordCount, uniqueKeys);
}