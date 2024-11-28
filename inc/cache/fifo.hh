/*
 * fifo.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _CACHE_FIFO_H
#define _CACHE_FIFO_H

#include <list>
#include <unordered_map>

#include "memory/buffer.hh"
#include "cache/cache.hh"

namespace pduck
{
    namespace cache
    {
        namespace weak
        {
            /**
             * @class FifoCacheFixedBuffer
             * @brief A First-In-First-Out (FIFO) cache implementation for fixed-size buffers.
             *
             * The FifoCacheFixedBuffer class provides a FIFO caching mechanism for fixed-size buffers.
             * It inherits from CacheFixedBuffer and overrides necessary methods to implement the FIFO policy.
             */
            class FifoCacheFixedBuffer : public CacheFixedBuffer
            {
            protected:
                std::list<uint64_t> m_recencyList; ///< List to maintain the order of cache elements.

                /**
                 * @brief Evicts elements from the cache if it overflows.
                 * @param p_newSize The size of the new element to be inserted.
                 * @return The number of evicted elements.
                 */
                virtual size_t evictOverflows(size_t p_newSize = 0) noexcept;

            public:
                /**
                 * @brief Constructor for FifoCacheFixedBuffer.
                 * @param p_capacity The capacity of the cache.
                 */
                FifoCacheFixedBuffer(size_t p_capacity = 0) noexcept
                    : CacheFixedBuffer(p_capacity)
                {

                };

                virtual ~FifoCacheFixedBuffer() = default;

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
            };
        }
    }
}

#endif // _CACHE_FIFO_H