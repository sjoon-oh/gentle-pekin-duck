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
    std::string testString2 = "This is a test string 2, which is longer than the first one";

    pduck::memory::DynamicAlignedBuffer dynBuff((uint8_t*)testString1.c_str(), testString1.size());

    pduck::utils::Logger::getInstance().getLogger()->info(
        "Test string 1: {}, size {}", (char*)(dynBuff.getBlock()), dynBuff.getSize());

    dynBuff.resizeAlloc(testString2.size());

    pduck::utils::Logger::getInstance().getLogger()->info(
        "After realloc: {}, size {}", (char*)(dynBuff.getBlock()), dynBuff.getSize());

    std::memcpy(dynBuff.getBlock(), testString2.c_str(), testString2.size());

    pduck::utils::Logger::getInstance().getLogger()->info(
        "Test string 2: {}, size {}", (char*)(dynBuff.getBlock()), dynBuff.getSize());

    pduck::utils::Logger::getInstance().getLogger()->info(
        "Buffer metadata size: {}", sizeof(dynBuff));

    return 0;
}