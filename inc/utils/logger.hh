// 
// Author: Sukjoon Oh

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace pduck 
{
    namespace utils 
    {

        class Logger {
        private:
            std::string                         m_loggerName;
            spdlog::logger*                     m_sharedLogger;
            // std::shared_ptr<spdlog::logger>     m_fileLogger;

        public:
            // Private constructor to prevent instantiation
            Logger(const char* p_loggerName) noexcept
                : m_loggerName(p_loggerName)
            {
                m_sharedLogger = spdlog::stdout_color_mt(std::string(p_loggerName)).get();
                // m_fileLogger = spdlog::basic_logger_mt("flog", "logfile.log");

                spdlog::set_pattern("[%n:%^%l%$] %v");
            }

            // Delete copy constructor and assignment operator
            Logger(const Logger&) = delete;
            Logger& operator=(const Logger&) = delete;

            static Logger& getInstance() noexcept
            {
                static Logger global_logger("log");
                return global_logger;
            }

            spdlog::logger* getLogger() noexcept
            {
                return m_sharedLogger;
            }

            // spdlog::logger* getFileLogger() noexcept
            // {
            //     return m_fileLogger.get();
            // }
        };

    }
}



#endif