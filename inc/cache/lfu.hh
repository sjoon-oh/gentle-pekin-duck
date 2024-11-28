/*
 * lfu.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _CACHE_LFU_H
#define _CACHE_LFU_H

#include <list>
#include <map>

#include "memory/buffer.hh" 
#include "cache/cache.hh"
#include "cache/lru.hh"

namespace pduck
{
    namespace cache
    {
        namespace weak
        {   
            // Although this LFU cache shares the same structure as the LRU cache,
            // this class is independent (not inheriting from CacheFixedBuffer) because
            // the LFU must hold multiple LRU lists to track the different frequencies of the elements.
            // 
            // Thus, instead of inheriting from CacheFixedBuffer, this class is implemented as a standalone class.

            class LfuCacheFixedBuffer : public IDelayableCache
            {
            protected:

                size_t                                          m_capacity;             // Capacity of the cache
                size_t                                          m_currSize;             // Current size of the cache

                size_t                                          m_minFreq;              // Current minimum frequency
                std::unordered_map<uint64_t, uint64_t>          m_frequencyMapper;      // Mapper for frequencies and LRU caches
                    
                std::map<uint64_t, std::unique_ptr<LruCacheFixedBuffer>>
                                                                m_lruCaches;            // LRU caches for different frequencies

                // 
                // Captures delayed requests
                std::vector<struct CacheObjInfo>                m_delayedContainer;     // Delayed container for the cache

                // So, 
                // for a key with certain frequency f, 
                // the corresponding LRU cache is m_frequencyMapper[f].
                // The item is stored in the LRU cache m_lruCaches[m_frequencyMapper[f]].

                virtual bool isCached(uint64_t p_key) noexcept
                {
                    // To find out the existence of an element,
                    // we need to check the existence of the element in the LRU cache.
                    // If the element is in the LRU cache, it is currently mapped to a certain frequency.
                    return (m_frequencyMapper.find(p_key) != m_frequencyMapper.end());
                }

                virtual size_t evictOverflows(size_t p_newSize = 0) noexcept;

                virtual void moveUpperFreqLru(struct CacheObjInfo p_element) noexcept;
            
            public:
                /**
                 * Constructor for LfuCacheFixedBuffer.
                 * @param p_capacity The capacity of the cache.
                 */
                LfuCacheFixedBuffer(size_t p_capacity = 0) noexcept
                    : m_capacity(p_capacity), m_currSize(0), m_minFreq(1)
                {
                    m_frequencyMapper.clear();
                    m_lruCaches.clear();

                    m_delayedContainer.clear();
                };

                virtual ~LfuCacheFixedBuffer() = default;

                /*
                 * @brief Inserts an element into the cache immediately.
                 * @param p_element The element to be inserted.
                 */                
                virtual void insertImmediate(struct CacheObjInfo p_element) noexcept override;

                
                virtual void insertDelayed(struct CacheObjInfo p_element) noexcept override;

                virtual FixedBufferType* getImmediate(struct CacheObjInfo p_element) noexcept override;

                virtual FixedBufferType* getDelayed(struct CacheObjInfo p_element) noexcept override;

                virtual size_t eraseImmediate(struct CacheObjInfo p_element) noexcept override;

                virtual void processDelayed() noexcept override;

                virtual void clearCache() noexcept override;

                virtual void dumpCacheStatus(const char* p_path = "cache-dump.csv") noexcept override;

                virtual void incrCapacity(size_t p_incrCap) noexcept override
                {
                    m_capacity += p_incrCap;
                }

                virtual void decrCapacity(size_t p_decrCap) noexcept override
                {
                    m_capacity -= p_decrCap;
                }

                virtual size_t getCapacity() const noexcept override
                {
                    return m_capacity;
                }

                virtual size_t getCurrSize() const noexcept override
                {
                    return m_currSize;
                }

                virtual struct CacheServingStatus getCacheStatus() const noexcept override 
                {
                    return m_cacheStatus;
                }
            };
        }










    }
}

#endif