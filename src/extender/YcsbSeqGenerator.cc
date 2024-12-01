/*
 * YcsbSeqGenerator.
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <cstdint>
#include <unordered_map>

#include <fstream>

#include "extender/YcsbSeqGenerator.hh"

pduck::extender::YcsbSeqGenerator::YcsbSeqGenerator() noexcept
{
    resetGenerator();
}

/**
 * Generates the next key in the sequence.
 * @return The next key as a uint64_t.
 */
std::uint64_t 
pduck::extender::YcsbSeqGenerator::generateNextKey() noexcept
{
    std::uint64_t keyNum = m_keyGenerator->Next();
    return keyNum;
}

/**
 * Chooses the next key based on the distribution type.
 * @return The chosen key as a uint64_t.
 */
std::uint64_t 
pduck::extender::YcsbSeqGenerator::chooseNextKey() noexcept
{
    std::uint64_t keyNum = 0;
    if (m_keyChooser.get() != nullptr)
    {   
        do
        {
            keyNum = m_keyChooser->Next();
        } 
        while (keyNum > m_insertKeySequence->Last());
    }
    return keyNum;
}

/**
 * Sets the generator with the specified record count and distribution type.
 * @param p_recordCount The number of records.
 * @param p_distType The distribution type (e.g., "uniform", "zipfian", "latest").
 * @return True if the generator was set successfully, false otherwise.
 */
bool
pduck::extender::YcsbSeqGenerator::setGenerator(size_t p_recordCount, std::string p_distType) noexcept
{
    m_insertKeySequence->Set(p_recordCount);

    // Make the p_distType lowercase.
    for (char &c : p_distType) c = c | ' ';

    if (p_distType == "uniform")
    {
        m_keyChooser.reset(new ycsbc::UniformGenerator(0, p_recordCount - 1));
    }

    else if (p_distType == "zipfian")
    {
        // Do not assume any inserts for now.
        m_keyChooser.reset(new ycsbc::ScrambledZipfianGenerator(p_recordCount));
    }

    else if (p_distType == "latest")
    {
        m_keyChooser.reset(new ycsbc::SkewedLatestGenerator(*m_insertKeySequence));
    }

    else
    {
        m_keyChooser.reset();
        return false;
    }

    return true;
}

/**
 * Resets the generator to its initial state.
 */
void 
pduck::extender::YcsbSeqGenerator::resetGenerator() noexcept
{
    m_sequence.clear();

    // Make default
    m_insertKeySequence.reset(new ycsbc::CounterGenerator(3));
    m_keyGenerator.reset(new ycsbc::CounterGenerator(0));

    m_keyChooser.reset();
}

/**
 * Generates a sequence of keys.
 * @param p_numVec The number of keys to generate.
 * @return A reference to the vector containing the generated keys.
 */
std::vector<std::uint64_t>&
pduck::extender::YcsbSeqGenerator::generateSequence(size_t p_numVec) noexcept
{
    for (size_t count = 0; count < p_numVec; count++)
        m_sequence.emplace_back(chooseNextKey());

    // This class just generates the sequence with the given distribution set.
    // Mapping of external vectors should be done by the caller.
    // Use retuned sequence ID to map the vectors.

    return m_sequence;
}

/**
 * Checks the uniqueness of the generated sequence.
 * @return The number of unique keys in the generated sequence.
 */
size_t
pduck::extender::YcsbSeqGenerator::checkUniqueIds(
    std::vector<std::pair<std::uint64_t, size_t>>& p_idsByFreq
) noexcept
{
    // Extract unique keys
    std::unordered_map<std::uint64_t, size_t> uniqueKeys;
    for (std::uint64_t& key: m_sequence)
    {
        if (uniqueKeys.find(key) == uniqueKeys.end())
            uniqueKeys[key] = 1;
            
        else 
            uniqueKeys[key]++;
    }

    // Insert to the vector
    p_idsByFreq = std::vector<std::pair<std::uint64_t, size_t>>(
        uniqueKeys.begin(), uniqueKeys.end()
    );

    // Sort the vector by frequency (descending order)
    std::sort(
        p_idsByFreq.begin(),
        p_idsByFreq.end(),
        [](const std::pair<std::uint64_t, size_t>& a, const std::pair<std::uint64_t, size_t>& b) {
            return a.second > b.second;
        }
    );

    return uniqueKeys.size();
}


void
pduck::extender::YcsbSeqGenerator::exportFrequency() noexcept
{

    std::vector<std::pair<std::uint64_t, size_t>> uniqueKeys;
    checkUniqueIds(uniqueKeys);

    // 
    // Export to files to visualize in descending count order.
    std::fstream outputFile("sequence-freqs.csv", std::ios::out);
    if (!outputFile)
        return;

    for (const auto& pair: uniqueKeys)
        outputFile << pair.first << "\t" << pair.second << std::endl;

    outputFile.close();
}