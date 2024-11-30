/*
 * timer.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>
#include <vector>

#include <fstream>

namespace pduck
{
    namespace utils
    {
        /**
         *
         * The Timestamp class provides methods to record the start and stop times
         * using std::chrono::steady_clock. It can be used to measure the duration
         * between two points in time.
         */
        class Timestamp
        {
        public:
            std::chrono::steady_clock::time_point m_timeStart;      // Start time point.
            std::chrono::steady_clock::time_point m_timeEnd;        // End time point.

            Timestamp() = default;
            virtual ~Timestamp() = default;

            void recordStart() noexcept
            {
                m_timeStart = std::chrono::steady_clock::now();
            }

            void recordStop() noexcept
            {
                m_timeEnd = std::chrono::steady_clock::now();
            }
        };

        /**
         * @class TimestampList
         *
         * The TimestampList class provides methods to record multiple start and stop
         * times using a vector of Timestamp objects. It also provides a method to
         * calculate the elapsed time between two time points.
         */
        class TimestampList
        {
        private:
            std::vector<Timestamp> m_timeList; // List of Timestamp objects.
            
            /**
             * @brief Calculates the elapsed time in milliseconds.
             *
             * This function calculates the elapsed time between two time points
             * in milliseconds.
             *
             * @param p_start The start time point.
             * @param p_end The end time point.
             * @return The elapsed time in milliseconds.
             */
            double getElapsedMs(
                std::chrono::steady_clock::time_point p_start, 
                std::chrono::steady_clock::time_point p_end) noexcept
            {
                return ((std::chrono::duration_cast<std::chrono::microseconds>(p_end - p_start).count() * 1.0) / 1000.000);
            }

        public:
            /**
             * @brief Default constructor for TimestampList.
             */
            TimestampList() = default;

            /**
             * @brief Virtual destructor for TimestampList.
             */
            virtual ~TimestampList() = default;

            /**
             * @brief Records the start time for a new Timestamp.
             * This function creates a new Timestamp object, records the current time
             * as the start time, and adds it to the list.
             */
            void recordStart() noexcept
            {
                m_timeList.push_back(Timestamp());
                m_timeList.back().recordStart();
            }

            /**
             * @brief Records the stop time for the most recent Timestamp.
             * This function records the current time as the stop time for the most
             * recent Timestamp object in the list.
             */
            void recordStop() noexcept
            {
                m_timeList.back().recordStop();
            }
            
            /**
             * @brief Clears the list of Timestamp objects.
             */
            void recordClear() noexcept
            {
                m_timeList.clear();
            }

            /**
             * @brief Dumps the elapsed times to a file.
             * This function calculates the elapsed time between the start and stop
             * times for each Timestamp object in the list and writes the results to
             * a file.
             *
             * @param p_filenmae The name of the file to write the elapsed times to.
             */
            void dumpElapsedTimes(const char* p_filenmae) noexcept
            {
                std::fstream exportFile(p_filenmae, std::ios::out);

                if (!exportFile.is_open())
                {
                    return;
                }

                for (int i = 0; i < m_timeList.size(); i++)
                {
                    double elapsedMs = getElapsedMs(m_timeList[i].m_timeStart, m_timeList[i].m_timeEnd);
                    exportFile << elapsedMs << std::endl;
                }

                exportFile.close();
            }

            /**
             * @brief Returns a vector of elapsed times.
             * This function calculates the elapsed time between the start and stop
             * times for each Timestamp object in the list and returns the results as
             * a vector of doubles.
             *
             * @return A vector of elapsed times in milliseconds.
             */
            std::vector<double> getElapsedTimes() noexcept
            {
                std::vector<double> elapsedTimes;

                for (int i = 0; i < m_timeList.size(); i++)
                {
                    double elapsedMs = getElapsedMs(m_timeList[i].m_timeStart, m_timeList[i].m_timeEnd);
                    elapsedTimes.push_back(elapsedMs);
                }

                return elapsedTimes;
            }
        };
    }
}

#endif // _TIMER_H