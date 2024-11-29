/*
 * lru.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <fstream>
#include <memory>

#include <cstdio>

#include "memory/Buffer.hh"
#include "cache/CacheLru.hh"

/**
 * Evicts elements from the cache until there is enough space for a new element.
 * 
 * @param p_newSize The size of the new element to be inserted.
 * @return The total size of the evicted elements.
 */
size_t pduck::cache::weak::LruCacheFixedBuffer::evictOverflows(size_t p_newSize) noexcept
{
    size_t evictSize = 0;
    while ((m_currSize + p_newSize) > m_capacity)
    {
        uint64_t leastRUsedKey = m_recencyList.back();          // 1. Get the least recently used key
        evictSize = m_dataContainer[leastRUsedKey]->getSize();  // 2. Get the size of the least recently used key
        
        m_dataContainer.erase(leastRUsedKey);                   // 3. Erase the least recently used key from the data container
        m_recencyListPos.erase(leastRUsedKey);                  // 4. Erase the position of the least recently used key in the recency list
        
        m_recencyList.pop_back();                               // 5. Update the recency list
                                                                //  Oldest element is at the back of the list

        m_currSize -= evictSize;                                // 6. Update the current size        
    }

    return evictSize;
}

/**
 * Inserts an element into the cache immediately, evicting elements if necessary.
 * @param p_element The element to be inserted.
 */
void pduck::cache::weak::LruCacheFixedBuffer::insertImmediate(struct CacheObjInfo p_element) noexcept
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

    m_recencyList.push_front(p_element.m_key);                  // 3. Update the recency list
    m_recencyListPos[p_element.m_key] = m_recencyList.begin();  // 4. Update the position of the element in the recency list

}

/**
 * Inserts an element into the delayed container.
 * @param p_element The element to be inserted.
 */
void pduck::cache::weak::LruCacheFixedBuffer::insertDelayed(struct CacheObjInfo p_element) noexcept
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
void pduck::cache::weak::LruCacheFixedBuffer::processDelayed() noexcept
{
    for (auto& elem: m_delayedContainer)
    {   
        if (elem.m_status == CacheStatusType::CACHE_HIT)
            m_cacheStatus.m_hitCounts++;

        else
            m_cacheStatus.m_missCounts++;

        // 
        // Why only call insertImmediate?
        // If the element is in the delayed container, it is hard to track the recency order.
        // For example, if the element is in the cache, it should be moved to the front of the recency list.
        // Previously processed element affects the recency order of the cache.
        // So, preparing in advance whether to reorder in the recency list is not a good idea.
        // 
        // For simplicity, just call insertImmediate.
        // Although this does not optimize the cache performance, it is a simple and effective way to keep
        // the recency order of the cache consistent.
        // Here, only track the number of hits and misses.
        // 

        insertImmediate(elem);
    }

    m_delayedContainer.clear();                                 
}

/**
 * Clears the cache.
 */
void pduck::cache::weak::LruCacheFixedBuffer::clearCache() noexcept
{
    CacheFixedBuffer::clearCache();

    m_recencyList.clear();
    m_recencyListPos.clear();
}

/**
 * Gets an element from the cache and updates its status to immediate.
 * @param p_element The element to be retrieved.
 * @return A pointer to the retrieved element's buffer, or nullptr if not found.
 */
pduck::memory::FixedBuffer* 
pduck::cache::weak::LruCacheFixedBuffer::getImmediate(struct CacheObjInfo p_element) noexcept
{
    if (isCached(p_element.m_key))
    {
        m_cacheStatus.m_hitCounts++;

        auto recencyListPos = m_recencyListPos[p_element.m_key];    // 1. Get the position of the element in the recency list
        m_recencyList.erase(recencyListPos);                        // 2. Erase the element from the recency list

        m_recencyList.push_front(p_element.m_key);                  // 3. Update the recency list, front is the most recently used
        m_recencyListPos[p_element.m_key] = m_recencyList.begin();  // 4. Update the position of the element in the recency list

        return m_dataContainer[p_element.m_key].get();
    }
    else
    {
        m_cacheStatus.m_missCounts++;
        p_element.m_status = CacheStatusType::CACHE_MISS;

        insertImmediate(p_element);

        return nullptr;
    }
}

/**
 * Erases an element from the cache immediately.
 * @param p_element The element to be erased.
 */
size_t pduck::cache::weak::LruCacheFixedBuffer::eraseImmediate(struct CacheObjInfo p_element) noexcept
{
    size_t eraseSize = 0;
    if (isCached(p_element.m_key))
    {
        eraseSize = m_dataContainer[p_element.m_key]->getSize(); 

        auto recencyListPos = m_recencyListPos[p_element.m_key];    // 1. Get the position of the element in the recency list
        m_recencyList.erase(recencyListPos);                        // 2. Erase the element from the recency list

        m_dataContainer.erase(p_element.m_key);                     // 3. Erase the element from the data container
        m_recencyListPos.erase(p_element.m_key);                    // 4. Erase the position of the element in the recency list

        m_currSize -= eraseSize;                                    // 5. Update the current size
    }

    return eraseSize;
}

/**
 * Gets an element from the cache and updates its status to delayed.
 * @param p_element The element to be retrieved.
 * @return A pointer to the retrieved element's buffer, or nullptr if not found.
 */
pduck::memory::FixedBuffer* 
pduck::cache::weak::LruCacheFixedBuffer::getDelayed(struct CacheObjInfo p_element) noexcept
{
    insertDelayed(p_element);
    if (isCached(p_element.m_key))
        return m_dataContainer[p_element.m_key].get();

    return nullptr;
}

/**
 * Dumps the cache status to a CSV file.
 * @param p_path The path to the CSV file.
 */
void pduck::cache::weak::LruCacheFixedBuffer::dumpCacheStatus(const char* p_path) noexcept
{
    std::fstream dumpFile(p_path, std::ios::out | std::ios::app);
    if (!dumpFile.is_open())
        return;

    for (auto& elem: m_recencyList)
        dumpFile << elem << ",";
    
    dumpFile << std::endl;
    dumpFile.close();
}

/**
 * Forces the cache to evict elements until the cache size is less than the given size.
 * @param p_size The size to be reduced.
 */
void pduck::cache::weak::LruCacheFixedBuffer::forceEvict(uint64_t p_size) noexcept
{
    size_t evictedSize = 0;

    if (m_currSize < p_size)
        return;

    while (evictedSize < p_size)
    {
        // Check what is left
        size_t left = p_size - evictedSize;
        if (m_currSize < left)
            return;

        uint64_t leastRUsedKey = m_recencyList.back();          // 1. Get the least recently used key
        size_t evictSize = m_dataContainer[leastRUsedKey]->getSize();  // 2. Get the size of the least recently used key
        
        m_dataContainer.erase(leastRUsedKey);                   // 3. Erase the least recently used key from the data container
        m_recencyListPos.erase(leastRUsedKey);                  // 4. Erase the position of the least recently used key in the recency list
        
        m_recencyList.pop_back();                               // 5. Update the recency list
                                                                //  Oldest element is at the back of the list

        m_currSize -= evictSize;                                // 6. Update the current size
        evictedSize += evictSize;                               // 7. Update the evicted size
    }
}

/**
 * Forces the cache to clear the delayed container.
 */
void pduck::cache::weak::LruCacheFixedBuffer::forceClearDelayed() noexcept
{
    m_delayedContainer.clear();
}