/*
 * VectorReader.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _VECTOR_READER_H
#define _VECTOR_READER_H

#include <string>
#include <vector>
#include <map>

#include "memory/Buffer.hh"

namespace pduck
{
    namespace extender
    {   
        enum class VectorType {
            VECTOR_TYPE_UNKNOWN                 = 0,
            VECTOR_TYPE_UINT8_T                 ,
            VECTOR_TYPE_INT8_T                  ,
            VECTOR_TYPE_FLOAT_T                 
        };

        struct VectorProfile
        {
            VectorType                          m_type;
            size_t                              m_size;
            size_t                              m_dimension;
        };

        class IVectorReader
        {
        protected:

        public:
            virtual void setPath(const char* p_path) noexcept = 0;

            virtual void setVectorProfile(struct VectorProfile p_vectorProfile) noexcept = 0;

            virtual bool loadVectors() noexcept = 0;

            virtual size_t loadedSize() noexcept = 0;

            virtual std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& getVectorList() noexcept = 0;

            virtual size_t removeDuplicates() noexcept = 0;

            virtual std::map<size_t, size_t>& getUniqueMap() noexcept = 0;

            virtual void reset() noexcept = 0;

            virtual void exportHumanReadable(const char* p_path) noexcept = 0;
        };


        class VectorQueryReader : public IVectorReader
        {
        protected:
            struct VectorProfile                m_vectorProfile;
            std::string                         m_path;

            std::vector<
                std::unique_ptr<::pduck::memory::FixedBuffer>> 
                                                m_vectorList;

            std::map<size_t, size_t>            m_uniqueMap;

        public:
            VectorQueryReader() noexcept
            {

            }

            virtual ~VectorQueryReader() = default;

            virtual void setPath(const char* p_path) noexcept override;

            virtual void setVectorProfile(struct VectorProfile p_vectorProfile) noexcept override;

            virtual bool loadVectors() noexcept override;

            virtual size_t loadedSize() noexcept override
            {
                return m_vectorList.size();
            }

            virtual std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& getVectorList() noexcept override
            {
                return m_vectorList;
            }

            virtual std::map<size_t, size_t>& getUniqueMap() noexcept override
            {
                return m_uniqueMap;
            }

            virtual size_t removeDuplicates() noexcept override;

            virtual void reset() noexcept override;
        };


        class GroundTruthReader
        {
        protected:
            std::string                         m_path;
            std::vector<
                std::unique_ptr<::pduck::memory::FixedBuffer>> 
                                                m_groundTruthChunkList;
            
            size_t                              m_topK;

        public:
            GroundTruthReader() noexcept
            {

            }

            virtual ~GroundTruthReader() = default;

            virtual void setPath(const char* p_path) noexcept;

            virtual bool loadGroundTruth(size_t p_numQueryVec) noexcept;

            virtual size_t loadedSize() noexcept
            {
                return m_groundTruthChunkList.size();
            }

            virtual size_t getTopK() noexcept
            {
                return m_topK;
            }

            virtual std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& getGroundTruthList() noexcept
            {
                return m_groundTruthChunkList;
            }

            virtual void reset() noexcept;

            virtual void exportHumanReadable(const char* p_path) noexcept;
        };


    }
}

#endif