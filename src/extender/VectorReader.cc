/*
 * VectorReader.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <fstream>
#include <memory>

#include <array>
#include <fstream>

#include <cassert>

#include "memory/Buffer.hh"
#include "extender/VectorReader.hh"


namespace pduck
{
    namespace extender
    {
        bool loadDefaultVectors(
            const char* p_path, 
            struct VectorProfile& p_vectorProfile,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_vectorList) noexcept
        {
            std::fstream vectorFile(p_path, std::ios::binary | std::ios::in);
            
            if (!vectorFile.is_open())
                return false;

            std::uint32_t numVectors;                                       // Number of vectors
            std::uint32_t numDimension;                                     // Dimension of each vector

            assert(p_vectorProfile.m_dimension == numDimension);            // Check the dimension of the vector profile

            std::uint32_t currentLoadedVectors = 0;                         // Number of vectors loaded

            vectorFile.seekg(0, std::ios::end);                             // Move to the end of the file
            size_t fileSize = static_cast<size_t>(vectorFile.tellg());      // Get the file size

            // 
            // Move to the beginning of the file, file read start.
            vectorFile.seekg(0, std::ios::beg);

            size_t readOffset = 0;
            
            vectorFile.seekg(readOffset, std::ios::beg);                    // Move to next position
            vectorFile.read((char*)&numVectors, sizeof(std::uint32_t));     // Read the number of vectors, 4 bytes
            readOffset += sizeof(std::uint32_t);                        

            vectorFile.seekg(readOffset, std::ios::beg);                    // Move to the next position
            vectorFile.read((char*)&numDimension, sizeof(std::uint32_t));   // Read the dimension of each vector, 4 bytes
            readOffset += sizeof(std::uint32_t);

            size_t readSize = (numDimension * (int32_t)p_vectorProfile.m_size);
            while (fileSize > readOffset)                                   // Read until the end of the file
            {
                p_vectorList.emplace_back(new ::pduck::memory::FixedBuffer(readSize));   
                                                                            // Create a new buffer

                std::uint8_t* targetReadBuffer = p_vectorList.back()->getAddr();

                vectorFile.seekg(readOffset, std::ios::beg);                // Move to the next unread position
                vectorFile.read((char*)targetReadBuffer, readSize);         // Read the vector (dimension * type size)

                readOffset += readSize;                                     // Update the read offset
    
                currentLoadedVectors++;                                     // Update the number of loaded vectors
            }

            // May have rest to read more, but we don't care about it in this implementation.

            vectorFile.close();
            return true;
        }

        bool loadXvecVectors(
            const char* p_path, 
            struct VectorProfile& p_vectorProfile,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_vectorList) noexcept
        {

            return true;
        }

        bool loadTxtVectors(
            const char* p_path, 
            struct VectorProfile& p_vectorProfile,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_vectorList) noexcept
        {
            return true;
        }

    }
}

void pduck::extender::VectorReader::setPath(const char* p_path) noexcept
{
    m_path = p_path;
}

void pduck::extender::VectorReader::setVectorProfile(
    struct pduck::extender::VectorProfile p_vectorProfile) noexcept
{
    m_vectorProfile = p_vectorProfile;
}

bool pduck::extender::VectorReader::loadVectors() noexcept
{
    bool returnValue = loadDefaultVectors(m_path.c_str(), m_vectorProfile, m_vectorList);


    return returnValue;
}

bool pduck::extender::VectorReader::saveVectors(const char* p_path) noexcept
{
    return 0;
}





