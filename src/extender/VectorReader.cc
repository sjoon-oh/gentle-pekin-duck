/*
 * VectorReader.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <fstream>
#include <memory>

#include <array>
#include <fstream>

#include <cassert>

#include <iostream>

#include "memory/Buffer.hh"
#include "extender/VectorReader.hh"


namespace pduck
{
    namespace extender
    {
        bool loadDefaultQueryVectors(
            const char* p_path, 
            struct VectorProfile& p_vectorProfile,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_vectorList) noexcept
        {
            std::fstream vectorFile(p_path, std::ios::binary | std::ios::in);
            
            if (!vectorFile.is_open())
                return false;

            std::uint32_t numVectors;                                           // Number of vectors
            std::uint32_t numDimension;                                         // Dimension of each vector         

            std::uint32_t currentLoadedVectors = 0;                             // Number of vectors loaded

            vectorFile.seekg(0, std::ios::end);                                 // Move to the end of the file
            size_t fileSize = static_cast<size_t>(vectorFile.tellg());          // Get the file size
            
            vectorFile.seekg(0, std::ios::beg);                                 // Move to the beginning of the file, file read start.

            size_t readOffset = 0;
            
            vectorFile.seekg(readOffset, std::ios::beg);                        // Move to next position
            vectorFile.read((char*)&numVectors, sizeof(std::uint32_t));         // Read the number of vectors, 4 bytes
            readOffset += sizeof(std::uint32_t);                        

            vectorFile.seekg(readOffset, std::ios::beg);                        // Move to the next position
            vectorFile.read((char*)&numDimension, sizeof(std::uint32_t));       // Read the dimension of each vector, 4 bytes
            readOffset += sizeof(std::uint32_t);

            assert(p_vectorProfile.m_dimension == numDimension);                // Check the dimension of the vector profile

            size_t readSize = (numDimension * (int32_t)p_vectorProfile.m_size);
            while (fileSize > readOffset)                                       // Read until the end of the file
            {
                p_vectorList.emplace_back(new ::pduck::memory::FixedBuffer(readSize));   
                                                                                // Create a new buffer

                std::uint8_t* targetReadBuffer = p_vectorList.back()->getBlock();

                vectorFile.seekg(readOffset, std::ios::beg);                    // Move to the next unread position
                vectorFile.read((char*)targetReadBuffer, readSize);             // Read the vector (dimension * type size)

                readOffset += readSize;                                         // Update the read offset
    
                currentLoadedVectors++;                                         // Update the number of loaded vectors
            }

            // May have rest to read more, but we don't care about it in this implementation.

            vectorFile.close();
            return true;
        }

        bool loadXvecQueryVectors(
            const char* p_path, 
            struct VectorProfile& p_vectorProfile,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_vectorList) noexcept
        {

            return true;
        }

        bool loadTxtQueryVectors(
            const char* p_path, 
            struct VectorProfile& p_vectorProfile,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_vectorList) noexcept
        {
            return true;
        }

        bool loadDefaultGroundTruth(
            const char* p_path, 
            size_t p_numQueryVec,
            size_t& p_numTopK,
            std::vector<std::unique_ptr<::pduck::memory::FixedBuffer>>& p_groundTruthChunkList) noexcept
        {
            std::fstream gtFile(p_path, std::ios::binary | std::ios::in);

            if (!gtFile.is_open())
                return false;

            std::uint32_t numVectors;                                           // Number of vectors
            std::uint32_t numTopK;                                              // topK

            gtFile.seekg(0, std::ios::end);                                     // Move to the end of the file
            size_t fileSize = static_cast<size_t>(gtFile.tellg());              // Get the file size

            gtFile.seekg(0, std::ios::beg);                                     // Move to the beginning of the file, file read start.

            size_t readOffset = 0;

            gtFile.seekg(readOffset, std::ios::beg);                            // Move to next position
            gtFile.read((char*)&numVectors, sizeof(std::uint32_t));             // Read the number of vectors, 4 bytes
            readOffset += sizeof(std::uint32_t);

            gtFile.seekg(readOffset, std::ios::beg);                            // Move to the next position
            gtFile.read((char*)&numTopK, sizeof(std::uint32_t));                // Read the topK, 4 bytes
            readOffset += sizeof(std::uint32_t);
            
            p_numTopK = numTopK;
            size_t readSize = (numTopK * sizeof(std::uint32_t));

            uint64_t numQuery = 0;
            while (numQuery < p_numQueryVec)                                    // Read until the end of the file
            {
                p_groundTruthChunkList.emplace_back(new ::pduck::memory::FixedBuffer(readSize));
                                                                                // Create a new buffer

                std::uint8_t* targetReadBuffer = p_groundTruthChunkList.back()->getBlock();

                gtFile.seekg(readOffset, std::ios::beg);                        // Move to the next unread position
                gtFile.read((char*)targetReadBuffer, readSize);                 // Read the vector (dimension * type size)

                readOffset += readSize;                                         // Update the read offset

                numQuery++;                                                     // Update the number of loaded vectors
            }

            // May have rest to read more, but we don't care about it in this implementation.

            gtFile.close();
            return true;
        }

    }
}

void pduck::extender::VectorQueryReader::setPath(const char* p_path) noexcept
{
    m_path = p_path;
}

void pduck::extender::VectorQueryReader::setVectorProfile(
    struct pduck::extender::VectorProfile p_vectorProfile) noexcept
{
    m_vectorProfile = p_vectorProfile;
}

bool pduck::extender::VectorQueryReader::loadVectors() noexcept
{
    bool returnValue = loadDefaultQueryVectors(m_path.c_str(), m_vectorProfile, m_vectorList);
    return returnValue;
}

size_t pduck::extender::VectorQueryReader::removeDuplicates() noexcept
{
    // Check the duplicates, by comparing the vectors.
    std::map<size_t, size_t> duplicateMap;                                      // Only to remove multiple counts.
    bool foundDup = false;

    for (size_t curIdx = 0; curIdx < m_vectorList.size() - 1; curIdx++)
    {
        foundDup = false;

        if (duplicateMap.find(curIdx) != duplicateMap.end())                    // If previously viewed, skip.
            continue;

        else 
        {
            for (size_t cmpIdx = curIdx + 1; cmpIdx < m_vectorList.size(); cmpIdx++)
            {
                auto* curBuffer = m_vectorList[curIdx].get();
                auto* cmpBuffer = m_vectorList[cmpIdx].get();

                if (curBuffer->getSize() != cmpBuffer->getSize())               // If the size is different, skip.
                    continue;

                int rc = std::memcmp(                                           // Compare the vectors.
                    curBuffer->getBlock(),
                    cmpBuffer->getBlock(),
                    curBuffer->getSize()
                );

                if (rc == 0)
                {
                    if (!foundDup)                                              // If the first duplicate is found, insert the current index.
                    {
                        foundDup = true;
                        m_uniqueMap.insert(std::make_pair(curIdx, 2));          // Insert the current index and the duplicate index.
                        duplicateMap.insert(std::make_pair(cmpIdx, 0));         // Insert the duplicate index.
                    }
                    else
                    {
                        m_uniqueMap[curIdx]++;                                  // If the current index is already found, increase the count.
                        duplicateMap.insert(std::make_pair(cmpIdx, 0));
                    }
                }
            }
        }

        if (!foundDup)
            m_uniqueMap.insert(std::make_pair(curIdx, 1));
    }

    return m_uniqueMap.size();
}


void pduck::extender::VectorQueryReader::reset() noexcept
{
    m_vectorList.clear();
    m_uniqueMap.clear();

    m_path.clear();
}

void pduck::extender::VectorQueryReader::exportHumanReadable(const char* p_path) noexcept
{
    return;
}

//
// GroundTruthReader
void pduck::extender::GroundTruthReader::setPath(const char* p_path) noexcept
{
    m_path = p_path;
}


bool pduck::extender::GroundTruthReader::loadGroundTruth(size_t p_numQueryVec) noexcept
{
    bool returnValue = loadDefaultGroundTruth(m_path.c_str(), p_numQueryVec, m_topK, m_groundTruthChunkList);
    return returnValue;
}


void pduck::extender::GroundTruthReader::reset() noexcept
{
    m_groundTruthChunkList.clear();
    m_path.clear();
}

void pduck::extender::GroundTruthReader::exportHumanReadable(const char* p_path) noexcept
{
    return;
}



