#pragma once

#include <fstream>

namespace Spoon
{
    class Logger
    {
    public:
        Logger() {}

        static Logger& Get()
        {
            static Logger instance;
            return instance;
        }

        void Initialize(const std::string& logfilePath)
        {
            m_LogFile.open(logfilePath, std::ios::trunc);
            if(!m_LogFile.is_open())
            {
                throw std::runtime_error("Failed to open log file at path: " + logfilePath);
            }
            m_LogFile << std::unitbuf;
            m_LogFile << "Log file initialized." << std::endl;
        }

        void Close()
        {
            if(m_LogFile.is_open())
            {
                m_LogFile << "Closing log file..." << std::endl;
                m_LogFile.close();
            }
        }

        void Write(const std::string& message)
        {
            if(m_LogFile.is_open())
            {
                m_LogFile << "[DEBUG]" << message << std::endl;
            }
        }

    private:
        std::ofstream m_LogFile;
    };
}