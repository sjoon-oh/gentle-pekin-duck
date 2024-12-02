/*
 * dynamic-buffer.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 *  Test file for the LRU cache implementation.
 */

#include <iostream>

#include "utils/Logger.hh"
#include "utils/ArgParser.hh"

#include "memory/Buffer.hh"

int
main(int argc, char* argv[])
{
    pduck::utils::Logger::getInstance().getLogger()->info("Dynamic buffer test");
    pduck::utils::ArgumentParser argParser;

    size_t initialBufferSize = 20;
    std::string testString1 = "This is a test string 1";
    std::string fileNameString1 = "test-string-1.txt";

    pduck::memory::MmappedFixedBuffer 
        mmappedBuff(testString1.size(), fileNameString1.c_str());

    char* allocatedBuffer = (char*)mmappedBuff.getBlock();
    std::memcpy(allocatedBuffer, testString1.c_str(), testString1.size());

    pduck::utils::Logger::getInstance().getLogger()->info(
        "Test string 1: {}, size {}", (char*)(mmappedBuff.getBlock()), mmappedBuff.getSize());

    if (mmappedBuff.flushBlockAsync(0, mmappedBuff.getSize()) == false)
        pduck::utils::Logger::getInstance().getLogger()->error("Failed to flush block");

    mmappedBuff.flushBlockWait();

    pduck::utils::Logger::getInstance().getLogger()->info(
        "Buffer metadata size: {}", sizeof(mmappedBuff));

    return 0;
}