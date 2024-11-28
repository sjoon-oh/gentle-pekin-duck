/*
 * lfu.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <fstream>
#include <memory>

#include <limits>

#include <cstdio>

#include "memory/buffer.hh"
#include "cache/cache.hh"
#include "cache/lfu.hh"


size_t pduck::cache::weak::LfuCacheFixedBuffer::evictOverflows(size_t p_newSize) noexcept
{
    // Minimum frequency is the first element in the lfu mapper
    size_t evictedSize = 0;
    size_t minFreq = 0;

    return 0; // Not implemented, for now.

    // Currently, the evictOverflows aggressively deletes elements in LRU units.
    // This is because it adds complexity to trace what candidates in a LRU, 
    // while some of them are not deleted.

    // Without adding another reverse mapper, that points all the elements in the specific frequency LRU cache,
    // it scans linearly to find the mapping information of deleted LRUs.

    // Scan the LRU caches from the minimum frequency,
    // and evict elements until there is enough space for the new element.
    // The LRU cache with the minimum frequency is the first element (ascending order) in the lfu mapper.
    for (auto& lruCache: m_lruCaches)
    {
        size_t lruSize = lruCache.second->getCurrSize();

        if (evictedSize < p_newSize)
        {
            minFreq = lruCache.first;

            m_lruCaches.erase(minFreq);
            evictedSize += lruSize;
        }

        else
            break;
    }

    // Erase the frequency mapping of the evicted elements

    return 0;
}

/**
 * Moves the element to the upper frequency LRU cache.
 * @param p_element The element to be moved.
 *  [WARNING] This function is called when the element is found in the cache.
 *  Any action is undefined when the element is not in the cache.
 */
void pduck::cache::weak::LfuCacheFixedBuffer::moveUpperFreqLru(struct CacheObjInfo p_element) noexcept
{
    uint64_t currentFreq = m_frequencyMapper[p_element.m_key];   // 1. Get the current frequency of the element
    uint64_t nextFreq = currentFreq + 1;                         // 2. Get the next frequency of the element

    m_frequencyMapper[p_element.m_key] = nextFreq;               // 3. Increase the frequency of the element
    m_lruCaches[currentFreq]->eraseImmediate(p_element);         // 4. Erase the element from the LRU cache with the current frequency

    if (m_lruCaches.find(nextFreq) == m_lruCaches.end())         // 5. Make a new LRU cache for the next frequency
    {
        m_lruCaches[nextFreq] = std::make_unique<LruCacheFixedBuffer>(
            std::numeric_limits<size_t>::max()
        );
    }

    // Case when existing LRU cache is empty, eliminate it.
    if (m_lruCaches[currentFreq]->getCurrSize() == 0)
        m_lruCaches.erase(currentFreq);

    m_lruCaches[nextFreq]->insertImmediate(p_element);           // Insert the element into the LRU cache with the new frequency
}


/**
 * Evicts elements from the cache until there is enough space for a new element.
 * @param p_newSize The size of the new element to be inserted.
 * @return The total size of the evicted elements.
 */
void pduck::cache::weak::LfuCacheFixedBuffer::insertImmediate(struct CacheObjInfo p_element) noexcept
{
    if (isCached(p_element.m_key))
    {
        return;
    }

    evictOverflows(p_element.m_size);
    // uint64_t minFreq = m_lruCaches.begin()->first;      // No need to find further. 
    //                                                     // The minimum frequency is the first element in the lfu mapper.

    uint64_t minFreq = 1;                               // The minimum frequency is 1 for the new element 

    m_frequencyMapper[p_element.m_key] = minFreq;       // 1. Map the frequency to the key.

    if (m_lruCaches.find(minFreq) == m_lruCaches.end()) // Case when there is no LRU cache for frequency 1
    {
        m_lruCaches[minFreq] = std::make_unique<LruCacheFixedBuffer>(
            std::numeric_limits<size_t>::max()
        );
                                                        // 2. Create a new LRU cache for frequency 1
                                                        // When setting the capacity, make it max value so that
                                                        // the cache does not evict elements.
                                                        // Eviction is managed by the LFU cache.
                                                        // We set the maximum capacity to the std::numeric_limits<size_t>::max().
    }

    m_currSize += p_element.m_size;                     // Increase the current size of the cache

    m_lruCaches[minFreq]->insertImmediate(p_element);   // 2. Insert the element into the LRU cache with frequency 1

}


void pduck::cache::weak::LfuCacheFixedBuffer::insertDelayed(struct CacheObjInfo p_element) noexcept
{
    if (isCached(p_element.m_key))
        p_element.m_status = CacheStatusType::CACHE_HIT;

    else
        p_element.m_status = CacheStatusType::CACHE_MISS;

    m_delayedContainer.emplace_back(p_element);         // Insert the element into the delayed container
                                                        // Later, elements in the delayed container will be updated
}


pduck::cache::FixedBufferType* 
pduck::cache::weak::LfuCacheFixedBuffer::getImmediate(struct CacheObjInfo p_element) noexcept
{
    if (!isCached(p_element.m_key))                     // Case when the element is not in the cache
    {
        m_cacheStatus.m_missCounts++;
        insertImmediate(p_element);

        return nullptr;
    }

    m_cacheStatus.m_hitCounts++;
    
    // uint64_t freq = m_frequencyMapper[p_element.m_key];                             // Get the frequency of the element
    // pduck::cache::weak::LruCacheFixedBuffer* lruCache = m_lruCaches[freq].get();    // Get the LRU cache for the frequency

    moveUpperFreqLru(p_element);                                                    // Move the element to the upper frequency LRU cache        

    uint64_t freq = m_frequencyMapper[p_element.m_key];                             // Get the frequency of the element
    pduck::cache::weak::LruCacheFixedBuffer* lruCache = m_lruCaches[freq].get();    // Get the LRU cache for the frequency

    return lruCache->getImmediate(p_element);
}


pduck::cache::FixedBufferType* 
pduck::cache::weak::LfuCacheFixedBuffer::getDelayed(struct CacheObjInfo p_element) noexcept
{
    insertDelayed(p_element);
    if (isCached(p_element.m_key))
    {
        uint64_t freq = m_frequencyMapper[p_element.m_key];                             // Get the frequency of the element
        pduck::cache::weak::LruCacheFixedBuffer* lruCache = m_lruCaches[freq].get();    // Get the LRU cache for the frequency
    
        return lruCache->getDelayed(p_element);
    }

    // For getDelayed, call the getDelayed of the LRU cache with the frequency of the element.
    // At the processDelayed, the internal LRU processDelayed will be called for the hit cases.
    // For the miss cases, the element will be inserted into the LRU cache with frequency 1, without
    // calling the getDelayed of the LRU cache.

    return nullptr;
}


size_t pduck::cache::weak::LfuCacheFixedBuffer::eraseImmediate(struct CacheObjInfo p_element) noexcept
{
    if (!isCached(p_element.m_key))
        return 0;

    uint64_t frequency = m_frequencyMapper[p_element.m_key];            // Get the frequency of the element
    LruCacheFixedBuffer* lruCache = m_lruCaches[frequency].get();       // Get the LRU cache for the frequency

    size_t eraseSize = lruCache->eraseImmediate(p_element);             // 1. Erase the element from the LRU cache first.

    if (lruCache->getCurrSize() == 0)
        m_lruCaches.erase(frequency);                                   // Erase the LRU cache if it is empty
    
    m_frequencyMapper.erase(p_element.m_key);                           // 2. Erase the frequency mapping of the element
    m_currSize -= eraseSize;
    
    return eraseSize;
}


void pduck::cache::weak::LfuCacheFixedBuffer::processDelayed() noexcept
{

    std::vector<struct CacheObjInfo> hitDelayed;
    std::vector<struct CacheObjInfo> missDelayed;

    for (auto& elem: m_delayedContainer)
    {
        if (elem.m_status == CacheStatusType::CACHE_HIT)
        {
            m_cacheStatus.m_hitCounts++;
            hitDelayed.emplace_back(elem);
        }
        else
        {
            m_cacheStatus.m_missCounts++;
            missDelayed.emplace_back(elem); 
        }
    }

    // Function below substitutes the following code: moveUpperFreqLru

    // Need to move elements between LRU caches
    // for (auto& elem: hitDelayed)
    // {
    //     uint64_t currentFreq = m_frequencyMapper[elem.m_key];   // 1. Get the current frequency of the element    
    //     m_frequencyMapper[elem.m_key] = currentFreq + 1;        // 2. Increase the frequency of the element
        
    //     m_lruCaches[currentFreq]->eraseImmediate(elem);         // 3. Erase the element from the LRU cache with the current frequency
    //                                                             // Make one if there is no LRU cache for the next frequency
    //     if (m_lruCaches.find(currentFreq + 1) == m_lruCaches.end())
    //     {
    //         m_lruCaches[currentFreq + 1] = std::make_unique<LruCacheFixedBuffer>(
    //             std::numeric_limits<size_t>::max()
    //         );
    //     }

    //     m_lruCaches[currentFreq + 1]->insertImmediate(elem);    // 4. Insert the element into the LRU cache with the new frequency
    // }

    for (auto& elem: hitDelayed)
        moveUpperFreqLru(elem);      

    // Okay.
    for (auto& elem: missDelayed)
    {
        // m_frequencyMapper[elem.m_key] = 1;                      // Set the frequency of the new element to 1
        insertImmediate(elem);                                  // Insert the element into the LRU cache with frequency 1
                                                                // Ignore frequency mapping of an element, as it is done in the 
                                                                // insertImmediate.
    }

    m_delayedContainer.clear();
}


void pduck::cache::weak::LfuCacheFixedBuffer::clearCache() noexcept
{
    m_frequencyMapper.clear();
    m_lruCaches.clear();
    m_delayedContainer.clear();

    m_cacheStatus = {0, 0};
}

/**
 * Dumps the cache status to a file.
 * @param p_path The path to the file.
 */
void pduck::cache::weak::LfuCacheFixedBuffer::dumpCacheStatus(const char* p_path) noexcept
{

    for (auto& lru: m_lruCaches)
    {
        std::fstream dumpFile(p_path, std::ios::out | std::ios::app);
        if (!dumpFile.is_open())
            return;
        
        dumpFile << lru.first << " : ";
        dumpFile.close();

        lru.second->dumpCacheStatus(p_path);
    }

    // Add an '\n' at the end of the file
    std::fstream reopenDumpFile(p_path, std::ios::out | std::ios::app);
    if (!reopenDumpFile.is_open())
        return;
    
    reopenDumpFile << "\n";
    reopenDumpFile.close();
}
