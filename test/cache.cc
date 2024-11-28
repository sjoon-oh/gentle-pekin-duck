/*
 * lru.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 *  Test file for the LRU cache implementation.
 */

#include <algorithm>
#include <random>

#include <fstream>
#include <memory>

#include "utils/logger.hh"
#include "utils/argument.hh"

#include "cache/lru.hh"
#include "cache/fifo.hh"

#include "utils/timer.hh"



int
main(int argc, char* argv[])
{
    pduck::utils::Logger::getInstance().getLogger()->info("lru test");
    pduck::utils::ArgumentParser argParser;

    argParser.addIntOption("iteration,i", "Number of iterations");
    argParser.parseArgs(argc, argv);

    std::int32_t totalIteration = argParser.getIntArgument("iteration");

    std::vector<std::int32_t> requestVec;

    for (int i = 1; i <= totalIteration; i++)
    {
        for (int elem_cnt = 0; elem_cnt < totalIteration; elem_cnt++)
            requestVec.push_back(i);
    }

    std::random_device randDev;
    std::mt19937 generator(randDev());
 
    std::shuffle(requestVec.begin(), requestVec.end(), generator);

    pduck::cache::IDelayableCache* cacheInstance = new pduck::cache::weak::LruCacheFixedBuffer(4*6);      // 20B cache
    std::unique_ptr<pduck::cache::IDelayableCache> cacheInstancePtr(cacheInstance);


    pduck::utils::TimestampList tList;
    

    // pduck::cache::weak::LruCacheFixedBuffer cacheInstance(4*1024);  // 4KB cache

    for (int i = 0; i < requestVec.size(); i++)
    {
        pduck::cache::CacheObjInfo cacheObj;

        cacheObj.m_key = requestVec[i];                 // Just use the index of the element
        cacheObj.m_size = sizeof(std::int32_t);         // Size of the element
        cacheObj.m_buffer = (uint8_t*)&requestVec[i];   // Just use the address of the element
        
        tList.recordStart();

        pduck::cache::FixedBufferType* cachedData = cacheInstancePtr->getImmediate(cacheObj);

        tList.recordStop();

        if (cachedData != nullptr)
        {
            uint8_t* cacheAddr = cachedData->getAddr();
            uint8_t* rawAddr = (uint8_t*)requestVec.data() + i * sizeof(std::int32_t);

            if (std::memcmp(cacheAddr, rawAddr, sizeof(std::int32_t)) != 0)
            {
                pduck::utils::Logger::getInstance().getLogger()->error("Data mismatch.");
            }

            else
            {
                // pduck::utils::Logger::getInstance().getLogger()->info("Data match: (index) {}", i);
            }
        }

        cacheInstancePtr->dumpCacheStatus();
    }
    
    tList.dumpElapsedTimes("elapsed-time-imm.csv");
    tList.recordClear();

    size_t hitCounts = cacheInstancePtr->getCacheStatus().m_hitCounts;
    size_t missCounts = cacheInstancePtr->getCacheStatus().m_missCounts;

    pduck::utils::Logger::getInstance().getLogger()->info("Hit counts: {}, Miss counts {}", hitCounts, missCounts);
    pduck::utils::Logger::getInstance().getLogger()->info("Immediate cache update test done, moving to delayed cache update.");

    cacheInstancePtr->clearCache();
    pduck::utils::Logger::getInstance().getLogger()->info("Cache cleared.");

    pduck::utils::Logger::getInstance().getLogger()->info("Every update triggered at 5 epochs");
    for (int i = 0; i < requestVec.size(); i++)
    {
        pduck::cache::CacheObjInfo cacheObj;

        cacheObj.m_key = requestVec[i];                 // Just use the index of the element
        cacheObj.m_size = sizeof(std::int32_t);         // Size of the element
        cacheObj.m_buffer = (uint8_t*)&requestVec[i];   // Just use the address of the element

        tList.recordStart();
        pduck::cache::FixedBufferType* cachedData = cacheInstancePtr->getDelayed(cacheObj);
        tList.recordStop();

        if (cachedData != nullptr)
        {
            uint8_t* cacheAddr = cachedData->getAddr();
            uint8_t* rawAddr = (uint8_t*)requestVec.data() + i * sizeof(std::int32_t);

            if (std::memcmp(cacheAddr, rawAddr, sizeof(std::int32_t)) != 0)
            {
                pduck::utils::Logger::getInstance().getLogger()->error("Data mismatch.");
            }

            else
            {
                // pduck::utils::Logger::getInstance().getLogger()->info("Data match: (index) {}", i);
            }
        }

        if (i % 5 == 0)
        {
            cacheInstancePtr->processDelayed();
            cacheInstancePtr->dumpCacheStatus("cache-dump-delayed.csv");
        }
    }

    tList.dumpElapsedTimes("elapsed-time-delayed.csv");

    hitCounts = cacheInstancePtr->getCacheStatus().m_hitCounts;
    missCounts = cacheInstancePtr->getCacheStatus().m_missCounts;

    pduck::utils::Logger::getInstance().getLogger()->info("Hit counts: {}, Miss counts {}", hitCounts, missCounts);

    std::fstream requestDump("request-dump.csv", std::ios::out);
    if (!requestDump.is_open())
    {
        pduck::utils::Logger::getInstance().getLogger()->error("Failed to open the request dump file.");
    }
    else
    {
        for (auto& elem: requestVec)
        {
            requestDump << elem << ", ";
        }
        requestDump.close();
    }

    return 0;
}