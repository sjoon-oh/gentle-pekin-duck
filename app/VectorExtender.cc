/*
 * lru.cc
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 *  Test file for the LRU cache implementation.
 */

#include <algorithm>
#include <random>

#include <fstream>
#include <memory>
#include <string>

#include "utils/Logger.hh"
#include "utils/ArgParser.hh"

#include "extender/VectorReader.hh"
#include "extender/YcsbSeqGenerator.hh"

// 
// Command:
// ./build/bin/pduck-query-extender -n 1000000 -d 100 -p "./dataset/spacev1b/query.bin" -t INT8

bool setupVectorProfile(

    struct pduck::extender::VectorProfile& p_vectorProfile, 
    const std::string& p_vectorType,
    std::int32_t p_dimension
    ) noexcept
{
    if (p_vectorType == "uint8")
    {
        p_vectorProfile.m_type = pduck::extender::VectorType::VECTOR_TYPE_UINT8_T;
        p_vectorProfile.m_size = sizeof(uint8_t);
    }
    else if (p_vectorType == "int8")
    {
        p_vectorProfile.m_type = pduck::extender::VectorType::VECTOR_TYPE_INT8_T;
        p_vectorProfile.m_size = sizeof(int8_t);
    }
    else if (p_vectorType == "float")
    {
        p_vectorProfile.m_type = pduck::extender::VectorType::VECTOR_TYPE_FLOAT_T;
        p_vectorProfile.m_size = sizeof(float);
    }
    else
    {
        pduck::utils::Logger::getInstance().getLogger()->error("Invalid vector type: {}", p_vectorType);
        return false;
    }

    // For now, the m_size does not need to be set,
    // as it will be set when the vectors are loaded.
    p_vectorProfile.m_dimension = p_dimension;

    return true;
}


int main(int argc, char* argv[])
{
    pduck::utils::Logger::getInstance().getLogger()->info("app::VectorExtender");
    pduck::utils::ArgumentParser argParser;

    argParser.addIntOption(
            "number,n", 
            "Number of requests to be extended");
    argParser.addIntOption(
            "dimension,d", 
            "Dimension of the vectors");
    argParser.addStringOption(
            "type,t", 
            "Vector type");
    argParser.addStringOption(
            "input-query,iq", 
            "Path to the input vector file");
    argParser.addStringOption(
            "output-query,oq", 
            "Path to the output vector file");
        argParser.addStringOption(
            "input-gt,ig", 
            "Path to the input vector file");
    argParser.addStringOption(
            "output-gt,og", 
            "Path to the output vector file");

    argParser.parseArgs(argc, argv);

    std::int32_t extendNumber   = argParser.getIntArgument("number");
    std::int32_t dimension      = argParser.getIntArgument("dimension");
    std::string vectorType      = argParser.getStringArgument("type");

    std::string inputQPath      = argParser.getStringArgument("input-query");
    std::string outputQPath     = argParser.getStringArgument("output-query");

    std::string inputGTPath     = argParser.getStringArgument("input-gt");
    std::string outputGTPath    = argParser.getStringArgument("output-gt");

    // 
    // 
    // Convert the vector type to lowercase.
    for(char &c: vectorType) c = c | ' '; // similar to: c = tolower(c);

    // Create a vector reader
    std::unique_ptr<pduck::extender::IVectorReader> vectorQueryReader 
        = std::make_unique<pduck::extender::VectorQueryReader>();

    std::unique_ptr<pduck::extender::GroundTruthReader> groundTruthReader 
        = std::make_unique<pduck::extender::GroundTruthReader>();

    struct pduck::extender::VectorProfile vectorProfile;

    // Set the path, for the query vectors
    vectorQueryReader->setPath(inputQPath.c_str());
    groundTruthReader->setPath(inputGTPath.c_str());

    if (::setupVectorProfile(vectorProfile, vectorType, dimension) == false)
        return -1;
    
    vectorQueryReader->setVectorProfile(vectorProfile);

    // Load the vectors
    size_t loadedSize = 0, uniqueSize = 0;
    if (vectorQueryReader->loadVectors() == false)
    {
        pduck::utils::Logger::getInstance().getLogger()->error("Failed to load vectors.");
        return -1;
    }

    loadedSize = vectorQueryReader->loadedSize();

    if (groundTruthReader->loadGroundTruth(loadedSize) == false)
    {
        pduck::utils::Logger::getInstance().getLogger()->error("Failed to load ground truth.");
        return -1;
    }

    pduck::utils::Logger::getInstance().getLogger()->info("Successfully loaded the vectors.");

    
    pduck::utils::Logger::getInstance().getLogger()->info("Loaded query size: {}", loadedSize);

    uniqueSize = vectorQueryReader->removeDuplicates();
    pduck::utils::Logger::getInstance().getLogger()->info("Unique query size: {}", uniqueSize);

    pduck::utils::Logger::getInstance().getLogger()->info("Successfully loaded the ground truth, size: {}", 
        groundTruthReader->loadedSize());


    // Generate Sequence
    std::unique_ptr<pduck::extender::YcsbSeqGenerator> seqGenerator 
        = std::make_unique<pduck::extender::YcsbSeqGenerator>();

    std::uint64_t recordCount = uniqueSize;                         // This number is what YCSB assumes as the number of records the underlying KVS has.
    std::string distribution = "zipfian";

    // 
    // When generating a sequence, the number of unique vectors must be the same as the origianl vector number.
    // If the generated sequence has smaller number of unique vectors, the sequence is not valid.
    // In this case, the sequence generation should be repeated until the number of unique vectors exceed as the original vector number.
    //

    // We start by the number of recordCount, but we extend it to the number by 1.
    std::uint64_t extendedRecordCount = recordCount;
    std::vector<std::pair<std::uint64_t, size_t>> sequenceIdsByFreq;

    while (true)
    {
        seqGenerator->resetGenerator();
        seqGenerator->setGenerator(extendedRecordCount, distribution);

        std::vector<std::uint64_t>& sequence = seqGenerator->generateSequence(extendNumber);
        size_t uniqueKeys = seqGenerator->checkUniqueIds(sequenceIdsByFreq);

        if (uniqueKeys < recordCount)
        {
            // Although the sequence is generated, the sequence size should be larger than the 
            // extendNum, because only the uniques will be considered.
            // Thus, IDs that are additionally generated should be removed.

            pduck::utils::Logger::getInstance().getLogger()->info("New record count ({}), unique count ({})", extendedRecordCount, uniqueKeys);
            
            // We increase by 0.5 percent of the original record count.
            // We set the increase number to be the same as the extendedRecordCount
            // extendedRecordCount += recordCount / 500;
            extendedRecordCount += recordCount / 100;
        }
        else
        {
            pduck::utils::Logger::getInstance().getLogger()->info(
                "Final sequence of size {} using {}, unique keys {}/{}, sequenceIdsByFreq size {}", 
                extendedRecordCount,
                sequence.size(), 
                recordCount,
                uniqueKeys,
                sequenceIdsByFreq.size()
                );
                
            break;
        }
    }

    // Map the vector sequence to the original vectors.
    // The mapping is random.
    // If the number of generated IDs is larger than the number of original vectors,
    // IDs that have the least frequency are removed.
    seqGenerator->exportFrequency();
    
    // 
    // The mapping is done by the caller.
    size_t mappingLimit = recordCount - (loadedSize - uniqueSize);

    std::unordered_map<std::uint64_t, size_t> vectorMapper;
    std::unordered_map<std::uint64_t, size_t> removedIds;

    size_t loaderIndex = 0;
    
    for (auto& elem: sequenceIdsByFreq)
    {
        if (loaderIndex < recordCount)
            vectorMapper[elem.first] = loaderIndex;

        else
            removedIds[elem.first] = loaderIndex;
    
        loaderIndex++;
    }

    // Removed IDs
    std::vector<std::uint64_t> refinedSequence;
    std::vector<std::uint64_t>& sequence = seqGenerator->getSequence();
    
    pduck::utils::Logger::getInstance().getLogger()->info(
        "sequence: {}, vectorMapper size: {}, removedIDs size: {}, sequenceIdsByFreq size: {}", 
        sequence.size(), vectorMapper.size(), removedIds.size(), sequenceIdsByFreq.size()
        );
    
    for (auto& elem: sequence)
    {   
        // If the element is not in the removed IDs, add it to the refined sequence.
        if (removedIds.find(elem) == removedIds.end())
            refinedSequence.emplace_back(elem);
    }

    pduck::utils::Logger::getInstance().getLogger()->info(
        "Refined sequence of size {}", 
        refinedSequence.size()
        );
    
    // Debugs
    // Top 10
    pduck::utils::Logger::getInstance().getLogger()->info("Top 10:");
    for (int i = 0; i < 10; i++)
        pduck::utils::Logger::getInstance().getLogger()->info("Mapped : <{}, {}> --> {}", sequenceIdsByFreq[i].first, sequenceIdsByFreq[i].second, i);
    
    // 
    // ---- Here, the mapping is done.
    // ---- The refined sequence is the sequence that is mapped to the original vectors.
    // Now, we export the refined sequence to the output file.

    {
        std::fstream exportQueryFile(outputQPath, std::ios::out | std::ios::binary);
        std::fstream exportGTFile(outputGTPath, std::ios::out | std::ios::binary);

        if (!exportQueryFile.is_open())
        {
            pduck::utils::Logger::getInstance().getLogger()->error("Failed to open the output query file.");
            return -1;
        }

        if (!exportGTFile.is_open())
        {
            pduck::utils::Logger::getInstance().getLogger()->error("Failed to open the output ground truth file.");
            return -1;
        }


        std::uint32_t numVectors        = static_cast<std::int32_t>(refinedSequence.size());
        std::uint32_t numDimension      = static_cast<std::int32_t>(dimension);
        std::uint32_t numTopK           = static_cast<std::int32_t>(groundTruthReader->getTopK());

        pduck::utils::Logger::getInstance().getLogger()->info("Preparing to export: size {}, dim {}, topk {}", numVectors, numDimension, numTopK);

        size_t queryFileWriteOffset = 0;
        size_t gtFileWriteOffset = 0;

        exportQueryFile.seekp(queryFileWriteOffset, std::ios::beg);
        exportQueryFile.write((char*)&numVectors, sizeof(std::int32_t));            // Query file: Write the first 4 bytes as the number of vectors.
        queryFileWriteOffset += sizeof(std::int32_t);

        exportGTFile.seekp(gtFileWriteOffset, std::ios::beg);
        exportQueryFile.write((char*)&numDimension, sizeof(std::int32_t));         // Query file: Write the second 4 bytes as the dimension of the vectors.
        queryFileWriteOffset += sizeof(std::int32_t);

        exportGTFile.seekp(gtFileWriteOffset, std::ios::beg);
        exportGTFile.write((char*)&numVectors, sizeof(std::int32_t));               // GT file: Write the first 4 bytes as the number of vectors.
        gtFileWriteOffset += sizeof(std::int32_t);
        
        exportGTFile.seekp(gtFileWriteOffset, std::ios::beg);
        exportGTFile.write((char*)&numTopK, sizeof(std::int32_t));                  // GT file: Write the second 4 bytes as the number of top-k.
        gtFileWriteOffset += sizeof(std::int32_t);

        // 
        // Export the refined sequence
        // vectorQueryReader->

        auto& vectorList = vectorQueryReader->getVectorList();
        auto& groundTruthList = groundTruthReader->getGroundTruthList();

        for (auto& elem: refinedSequence)
        {   
            uint8_t* queryBuffer = vectorList[vectorMapper[elem]]->getBlock();
            size_t queryBufferSize = vectorList[vectorMapper[elem]]->getSize();

            uint8_t* gtBuffer = groundTruthList[vectorMapper[elem]]->getBlock();
            size_t gtBufferSize = groundTruthList[vectorMapper[elem]]->getSize();

            // 
            // Export the query vector
            exportQueryFile.seekp(queryFileWriteOffset, std::ios::beg);
            exportQueryFile.write((char*)queryBuffer, queryBufferSize);
            queryFileWriteOffset += queryBufferSize;

            //
            // Export the ground truth vector
            exportGTFile.seekp(gtFileWriteOffset, std::ios::beg);
            exportGTFile.write((char*)gtBuffer, gtBufferSize);
            gtFileWriteOffset += gtBufferSize;
        }

        pduck::utils::Logger::getInstance().getLogger()->info(
            "Exported query file size: {}, ground truth file size: {}", 
                queryFileWriteOffset, gtFileWriteOffset
            );

        exportQueryFile.close();
        exportGTFile.close();
    }

}