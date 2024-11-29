// Author: Sukjoon Oh (sjoon@kaist.ac.kr)
// 

#ifndef _CACHE_CORE_H
#define _CACHE_CORE_H

#include <vector>
#include <unordered_map>

#include "memory/Buffer.hh"
#include "utils/Timer.hh"

namespace pduck
{
    namespace cache
    {
        /**
         * The status of the cache.
         */
        struct CacheServingStatus
        {
            size_t              m_hitCounts;        // Number of cache hits
            size_t              m_missCounts;       // Number of cache misses
        };

        enum class CacheStatusType {
            CACHE_MISS          = 0,
            CACHE_HIT           = 1
        };

        /**
         * Parameter of a cache object.
         */
        struct CacheObjInfo
        {
            CacheStatusType     m_status;  // Status of the cache object
            uint64_t            m_key;     // Key of the cache object
            uint8_t*            m_buffer;  // Buffer of the cache object
            size_t              m_size;    // Size of the cache object
        };   

        using FixedBufferType = ::pduck::memory::FixedBuffer;
        
        /**
         * Interface for a cache.
         *  Delayable cache can delay the cache management until the processDelayed() is called.
         */
        class IDelayableCache
        {
        protected:
            struct CacheServingStatus       m_cacheStatus; // Status of the cache

        public:
            virtual void insertImmediate(struct CacheObjInfo p_element) noexcept = 0;

            virtual void insertDelayed(struct CacheObjInfo p_element) noexcept = 0;

            virtual FixedBufferType* getImmediate(struct CacheObjInfo p_element) noexcept = 0;

            virtual FixedBufferType* getDelayed(struct CacheObjInfo p_element) noexcept = 0;

            virtual size_t eraseImmediate(struct CacheObjInfo p_element) noexcept = 0;

            virtual void processDelayed() noexcept = 0;

            virtual void clearCache() noexcept = 0;

            virtual void dumpCacheStatus(const char* p_path = "cache-dump.csv") noexcept = 0;

            virtual void incrCapacity(size_t p_incrCap) noexcept = 0;

            virtual void decrCapacity(size_t p_decrCap) noexcept = 0;

            virtual size_t getCapacity() const noexcept = 0;

            virtual size_t getCurrSize() const noexcept = 0;

            virtual struct CacheServingStatus getCacheStatus() const noexcept = 0;
        };

        /**
         * Base class for a fixed buffer cache.
         */
        class CacheFixedBuffer : public IDelayableCache
        {
        protected:
            size_t              m_capacity;  // Capacity of the cache
            size_t              m_currSize;  // Current size of the cache

            std::unordered_map<uint64_t, std::unique_ptr<FixedBufferType>>  m_dataContainer;    // Data container for the cache
            std::vector<struct CacheObjInfo>                             m_delayedContainer; // Delayed container for the cache

            

            virtual bool isCached(uint64_t p_key) noexcept
            {
                return (m_dataContainer.find(p_key) != m_dataContainer.end());
            }

        public:
            /**
             * Constructor for CacheFixedBuffer.
             * @param p_capacity The capacity of the cache.
             */
            CacheFixedBuffer(size_t p_capacity = 0) noexcept
                : m_capacity(p_capacity), m_currSize(0)
            {
                m_cacheStatus = {0, 0};
            };

            virtual ~CacheFixedBuffer() = default;

            /**
             * Inserts an element into the cache immediately.
             * @param p_element The element to be inserted.
             */
            virtual void insertImmediate(struct CacheObjInfo p_element) noexcept  = 0;

            /**
             * Inserts an element into the delayed container.
             * @param p_element The element to be inserted.
             */
            virtual void insertDelayed(struct CacheObjInfo p_element) noexcept = 0;

            /**
             * Gets an element from the cache and updates its status to immediate.
             * @param p_element The element to be retrieved.
             * @return A pointer to the retrieved element's buffer, or nullptr if not found.
             */
            virtual FixedBufferType* getImmediate(struct CacheObjInfo p_element) noexcept = 0;

            /**
             * Gets an element from the cache and updates its status to delayed.
             * @param p_element The element to be retrieved.
             * @return A pointer to the retrieved element's buffer, or nullptr if not found.
             */
            virtual FixedBufferType* getDelayed(struct CacheObjInfo p_element) noexcept = 0;

            /**
             * Erases an element from the cache immediately.
             * @param p_element The element to be erased.
             * @return The size of the erased element.
             */
            virtual size_t eraseImmediate(struct CacheObjInfo p_element) noexcept = 0;

            /**
             * Updates the cache with elements from the delayed container.
             */
            virtual void processDelayed() noexcept = 0;

            virtual void clearCache() noexcept
            {
                m_dataContainer.clear();
                m_delayedContainer.clear();

                m_cacheStatus = {0, 0};
                m_currSize = 0;
            }

            virtual void dumpCacheStatus(const char* p_path = "cache-dump.csv") noexcept = 0;
            
            virtual void incrCapacity(size_t p_incrCap) noexcept 
            {
                m_capacity += p_incrCap;
            }

            virtual void decrCapacity(size_t p_decrCap) noexcept
            {
                m_capacity -= p_decrCap;
            }

            virtual size_t getCapacity() const noexcept
            {
                return m_capacity;
            }

            virtual size_t getCurrSize() const noexcept 
            {
                return m_currSize;
            }

            virtual struct CacheServingStatus getCacheStatus() const noexcept override
            {
                return m_cacheStatus;
            }

            virtual size_t countDelayed() noexcept 
            {
                return m_delayedContainer.size();
            }
        };
    }

}

#endif