/*
 * lru.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _CACHE_LRU_H
#define _CACHE_LRU_H

#include <unordered_map>
#include <list>

#include "memory/buffer.hh"
#include "cache/cache.hh"

namespace pduck
{
    namespace cache
    {
        namespace weak
        {
            /**
             * @class LruCacheFixedBuffer
             * @brief A Least Recently Used (LRU) cache implementation for fixed-size buffers.
             *
             * The LruCacheFixedBuffer class provides an LRU caching mechanism for fixed-size buffers.
             * It inherits from CacheFixedBuffer and overrides necessary methods to implement the LRU policy.
             */
            class LruCacheFixedBuffer : public CacheFixedBuffer
            {
            protected:
                std::list<uint64_t>                                                     m_recencyList;      // List to maintain the recency order of cache elements.
                std::unordered_map<uint64_t, typename std::list<uint64_t>::iterator>    m_recencyListPos;   // Map to store positions of elements in the recency list.

                virtual size_t evictOverflows(size_t p_newSize = 0) noexcept;

            public:
                /**
                 * @brief Constructor for LruCacheFixedBuffer.
                 * @param p_capacity The capacity of the cache.
                 */
                LruCacheFixedBuffer(size_t p_capacity = 0) noexcept
                    : CacheFixedBuffer(p_capacity)
                {

                };

                virtual ~LruCacheFixedBuffer() = default;

                /**
                 * @brief Inserts an element into the cache immediately.
                 * @param p_element The element to be inserted.
                 */
                virtual void insertImmediate(struct CacheObjInfo p_element) noexcept override;

                /**
                 * @brief Inserts an element into the cache with a delay.
                 * @param p_element The element to be inserted.
                 */
                virtual void insertDelayed(struct CacheObjInfo p_element) noexcept override;

                /**
                 * @brief Gets and updates an element in the cache immediately.
                 * @param p_element The element to be updated.
                 * @return A pointer to the updated FixedBuffer.
                 */
                virtual FixedBufferType* getImmediate(struct CacheObjInfo p_element) noexcept override;

                /**
                 * @brief Gets and updates an element in the cache with a delay.
                 * @param p_element The element to be updated.
                 * @return A pointer to the updated FixedBuffer.
                 */
                virtual FixedBufferType* getDelayed(struct CacheObjInfo p_element) noexcept override;

                /**
                 * @brief Erases an element from the cache immediately.
                 * @param p_element The element to be erased.
                 */
                virtual size_t eraseImmediate(struct CacheObjInfo p_element) noexcept override;

                /**
                 * @brief Updates the cache with delayed elements.
                 */
                virtual void processDelayed() noexcept override;

                /**
                 * @brief Clears the cache.
                 */
                virtual void clearCache() noexcept override;
                
                /**
                 * @brief Dumps the cache status to a CSV file.
                 * @param p_path The path to the CSV file.
                 */
                virtual void dumpCacheStatus(const char* p_path = "cache-dump.csv") noexcept override;

                /**
                 * @brief Free elements in sizes (minimum)
                 * @param p_incrCap The amount to increase the capacity by.
                 *  This interface is just for the LFU control.
                 */
                virtual void forceEvict(uint64_t p_size) noexcept;

                /**
                 * @brief Forces the cache to clear the delayed container.
                 *  This interface is just for the LFU control.
                 */
                virtual void forceClearDelayed() noexcept;
            };
        }
    }

}

#endif // _CACHE_LRU_H