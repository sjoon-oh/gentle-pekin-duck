/*
 * fifo.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <fstream>
#include <memory>

#include <cstdio>

#include "memory/buffer.hh"
#include "cache/fifo.hh"

/**
 * Evicts elements from the cache until there is enough space for a new element.
 * 
 * @param p_newSize The size of the new element to be inserted.
 * @return The total size of the evicted elements.
 */
 size_t pduck::cache::weak::FifoCacheFixedBuffer::evictOverflows(size_t p_newSize) noexcept
 {
    // Unlike LRU, FIFO just pops out the oldest element
    // until there is enough space for the new element
    // Since the m_recencyList is same as the one in the LRU,
    // this function is identical to the one in the LRU.

    size_t evictSize = 0;
    while ((m_currSize + p_newSize) > m_capacity)
    {
        uint64_t oldestKey = m_recencyList.back();              // 1. Get the oldest key
        evictSize = m_dataContainer[oldestKey]->getSize();      // 2. Get the size of the oldest key
        
        m_dataContainer.erase(oldestKey);                       // 3. Erase the oldest key from the data container
        m_recencyList.pop_back();                               // 4. Update the recency list
                                                                //  Oldest element is at the back of the list

        m_currSize -= evictSize;                                // 5. Update the current size

    }

     return evictSize;
 }


void pduck::cache::weak::FifoCacheFixedBuffer::insertImmediate(struct CacheObjInfo p_element) noexcept
{
    if (isCached(p_element.m_key))
    {
        return;
    }

    // Evict overflows if necessary
    evictOverflows(p_element.m_size);

    m_dataContainer[p_element.m_key] = std::make_unique<pduck::memory::FixedBuffer>(p_element.m_buffer, p_element.m_size);
                                                                // 1. Insert the element into the data container
    m_currSize += p_element.m_size;                             // 2. Update the current size

    m_recencyList.push_front(p_element.m_key);                  // 3. Update the recency list, most recent is at the front
}


void pduck::cache::weak::FifoCacheFixedBuffer::insertDelayed(struct CacheObjInfo p_element) noexcept
{
    if (isCached(p_element.m_key))
        p_element.m_status = CacheStatusType::CACHE_HIT;        // Update the status of the element
    
    else
        p_element.m_status = CacheStatusType::CACHE_MISS;       // Update the status of the element (Miss case)

    m_delayedContainer.emplace_back(p_element);                 // Insert the element into the delayed container
                                                                // Later, elements in the delayed container will be updated
}

/**
 * Updates the cache with elements from the delayed container.
 */
void pduck::cache::weak::FifoCacheFixedBuffer::processDelayed() noexcept
{
    for (auto& elem: m_delayedContainer)  
    {
        if (elem.m_status == CacheStatusType::CACHE_HIT)
        {
            m_cacheStatus.m_hitCounts++;
                // Unlike LRU, FIFO does not need to update the recency list
                // When hit, just increase the hit count
        }

        else
        {
            m_cacheStatus.m_missCounts++;
            insertImmediate(elem);
        }
    }

    m_delayedContainer.clear();                                 // Clear the delayed container
}

/**
 * Clears the cache.
 */
void pduck::cache::weak::FifoCacheFixedBuffer::clearCache() noexcept
{
    CacheFixedBuffer::clearCache();

    m_recencyList.clear();
}

/**
 * Gets an element from the cache and updates its status to immediate.
 * 
 * @param p_element The element to be retrieved.
 * @return A pointer to the retrieved element's buffer, or nullptr if not found.
 */
pduck::memory::FixedBuffer* pduck::cache::weak::FifoCacheFixedBuffer::getImmediate(struct CacheObjInfo p_element) noexcept
{
    // In FIFO, there is no need to update the recency list
    // Just check if the element is in the cache
    if (!isCached(p_element.m_key))
    {
        m_cacheStatus.m_missCounts++;
        insertImmediate(p_element);

        return nullptr;
    }

    m_cacheStatus.m_hitCounts++;
    return m_dataContainer[p_element.m_key].get();
}

size_t pduck::cache::weak::FifoCacheFixedBuffer::eraseImmediate(struct CacheObjInfo p_element) noexcept
{   
    // Return immediately if the element is not in the cache
    if (!isCached(p_element.m_key))
        return 0;
    
    size_t evictSize = m_dataContainer[p_element.m_key]->getSize();
    m_currSize -= evictSize;

    m_dataContainer.erase(p_element.m_key);
    m_recencyList.remove(p_element.m_key);

    return evictSize;
}

/**
 * Gets an element from the cache and updates its status to delayed.
 * 
 * @param p_element The element to be retrieved.
 * @return A pointer to the retrieved element's buffer, or nullptr if not found.
 */
pduck::memory::FixedBuffer* pduck::cache::weak::FifoCacheFixedBuffer::getDelayed(struct CacheObjInfo p_element) noexcept
{
    insertDelayed(p_element);
    if (isCached(p_element.m_key))
        return m_dataContainer[p_element.m_key].get();

    return nullptr;
}


/**
 * Dump the cache status to a CSV file.
 * @param p_path The path to the CSV file.
 */
 void pduck::cache::weak::FifoCacheFixedBuffer::dumpCacheStatus(const char* p_path) noexcept
 {
    std::fstream dumpFile(p_path, std::ios::out | std::ios::app);
    if (!dumpFile.is_open())
        return;

    for (auto& elem: m_recencyList)
        dumpFile << elem << ", ";

    dumpFile << std::endl;
    dumpFile.close();
 }

