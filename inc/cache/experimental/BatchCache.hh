/*
 * BatchCache.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _EXPERIMENTAL_BATCH_CACHE_H
#define _EXPERIMENTAL_BATCH_CACHE_H

#include <vector>
#include <unordered_map>

#include "cache/CacheBase.hh"

namespace pduck
{
    namespace cache
    {
        namespace experimental
        {
            namespace weak
            {
                
                // MmapedFileManager

                class MmapedFileManager
                {
                protected:
                    // This class 

                    std::unordered_map<uint64_t, uint64_t>      m_fileContainer;

                public:
                    MmapedFileManager() noexcept
                    {

                    }

                    virtual ~MmapedFileManager() noexcept
                    {

                    }

                    virtual bool isMmaped(uint64_t p_key) noexcept
                    {
                        return (m_fileContainer.find(p_key) != m_fileContainer.end());
                    }

                    
                }

                class CacheFixedMmappedBuffer : public IDelayableCache
                {
                protected:
                    size_t                                      m_capacity;  // Capacity of the cache
                    size_t                                      m_currSize;  // Current size of the cache    

                    std::unordered_map<
                        uint64_t, 
                        std::unique_ptr<MmappedFixedBuffer>>    m_dataContainer;    // Data container for the cache
                    std::vector<struct CacheObjInfo>            m_delayedContainer; // Delayed container for the cache 

                    virtual bool isCached(uint64_t p_key) noexcept
                    {
                        return (m_dataContainer.find(p_key) != m_dataContainer.end());
                    }

                    // 
                    // Management of evicted elements 



                public:
                    



                }


            }
            

        }

    }
}

#endif