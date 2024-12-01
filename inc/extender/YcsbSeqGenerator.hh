/*
 * YcsbSeqGenerator.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _YCSB_SEQ_GENERATOR_H
#define _YCSB_SEQ_GENERATOR_H

#include <cstdint>
#include <vector>
#include <string>

#include "YCSB-C/core/generator.h"
#include "YCSB-C/core/uniform_generator.h"
#include "YCSB-C/core/zipfian_generator.h"
#include "YCSB-C/core/scrambled_zipfian_generator.h"
#include "YCSB-C/core/skewed_latest_generator.h"
#include "YCSB-C/core/const_generator.h"

#include "memory/Buffer.hh"

namespace pduck
{
    namespace extender
    {
        /**
         * Class for generating YCSB sequences.
         */
        class YcsbSeqGenerator
        {
        private:
            std::vector<std::uint64_t>                              m_sequence;

            std::unique_ptr<ycsbc::CounterGenerator>                m_insertKeySequence;

            std::unique_ptr<ycsbc::CounterGenerator>                m_keyGenerator;
            std::unique_ptr<ycsbc::Generator<std::uint64_t>>        m_keyChooser;

            // 
            // Generated sequence
            std::uint64_t generateNextKey() noexcept;
            std::uint64_t chooseNextKey() noexcept;

        public:

            YcsbSeqGenerator() noexcept;

            virtual ~YcsbSeqGenerator() = default;

            /**
             * Sets the generator with the specified record count and distribution type.
             * @param p_recordCount The number of records.
             * @param p_distType The distribution type (e.g., "uniform", "zipfian", "latest").
             * @return True if the generator was set successfully, false otherwise.
             */
            virtual bool setGenerator(size_t p_recordCount, std::string p_distType) noexcept;

            /**
             * Resets the generator to its initial state.
             */
            virtual void resetGenerator() noexcept;

            /**
             * Generates a sequence of keys.
             * @param p_numQueryVec The number of keys to generate.
             * @return A reference to the vector containing the generated keys.
             */
            virtual std::vector<std::uint64_t>& generateSequence(size_t p_numQueryVec) noexcept;

            /**
             * Checks the uniqueness of the generated sequence.
             * @return The number of unique keys in the generated sequence.
             */
            virtual size_t checkUniqueIds(std::vector<std::pair<std::uint64_t, size_t>>& p_idsByFreq) noexcept;

            /**
             * Exports the frequency of the generated sequence.
             */
            virtual void exportFrequency() noexcept;

            /**
             * Returns the generated sequence.
             * @return A reference to the generated sequence.
             */
            virtual std::vector<std::uint64_t>& getSequence() noexcept
            {
                return m_sequence;
            }
        };
    }
}

#endif // _YCSB_SEQ_GENERATOR_H