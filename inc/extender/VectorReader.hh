/*
 * VectorReader.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _VECTOR_READER_H
#define _VECTOR_READER_H

#include <string>
#include <vector>

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

            virtual bool saveVectors(const char* p_path) noexcept = 0;

        };

        class VectorReader : public IVectorReader
        {
        protected:
            struct VectorProfile                m_vectorProfile;
            std::string                         m_path;

            std::vector<
                std::unique_ptr<::pduck::memory::FixedBuffer>> 
                                                m_vectorList;

            ::pduck::utils::Logger              m_logger;

        public:
            VectorReader() noexcept
                : m_logger("vector-reader")
            {

            }

            virtual ~VectorReader() = default;

            virtual void setPath(const char* p_path) noexcept override;

            virtual void setVectorProfile(struct VectorProfile p_vectorProfile) noexcept override;

            virtual bool loadVectors() noexcept override;

            virtual bool saveVectors(const char* p_path) noexcept override;



        };
    }
}

#endif