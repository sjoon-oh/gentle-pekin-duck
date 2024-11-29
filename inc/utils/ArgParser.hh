/*
 * argument.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#include <string>
#include <boost/program_options.hpp>

 namespace pduck
 {
    namespace utils
    {
        namespace po = boost::program_options;

        class ArgumentParser final
        {
        private:
            Logger                      m_logger;

            po::options_description     m_desc;
            po::variables_map           m_vm;

        public:
            ArgumentParser() noexcept
                : m_logger("parser"), m_desc("Allowed options")
            {

            }

            ~ArgumentParser() noexcept
            {

            }

            void addOption(const char* p_option, const char* p_description) noexcept
            {
                m_desc.add_options()
                    (p_option, p_description);
            }

            void addIntOption(const char* p_option, const char* p_description) noexcept
            {
                m_desc.add_options()
                    (p_option, po::value<int>(), p_description);
            }

            void addDoubleOption(const char* p_option, const char* p_description) noexcept
            {
                m_desc.add_options()
                    (p_option, po::value<double>(), p_description);
            }

            void addStringOption(const char* p_option, const char* p_description) noexcept
            {
                m_desc.add_options()
                    (p_option, po::value<std::string>(), p_description);
            }

            void parseArgs(int argc, char* argv[])
            {
                try 
                {
                    po::store(po::parse_command_line(argc, argv, m_desc), m_vm);
                    po::notify(m_vm);
                } 
                catch (const po::error& e) 
                {
                    m_logger.getLogger()->warn("Error parsing arguments: {}", e.what());
                }
            }

            int getIntArgument(const char* p_option) noexcept
            {
                if (m_vm.count(p_option))
                    return m_vm[p_option].as<int>();

                else
                    m_logger.getLogger()->warn("{} was not set.", p_option);
                    
                return 0;
            }

            double getDoubleArgument(const char* p_option) noexcept
            {
                if (m_vm.count(p_option))
                    return m_vm[p_option].as<double>();

                else
                    m_logger.getLogger()->warn("{} was not set.", p_option);
                    
                return 0;
            }

            std::string getStringArgument(const char* p_option) noexcept
            {
                if (m_vm.count(p_option))
                    return m_vm[p_option].as<std::string>();

                else
                    m_logger.getLogger()->warn("{} was not set.", p_option);
                    
                return "";
            }

        };
    }
 }