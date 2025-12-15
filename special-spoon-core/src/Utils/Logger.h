#pragma once

#include "Core/Core.h"
#include <fstream>
#include <vector>

namespace Spoon
{
    class SPOON_API Logger
    {
    public:
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

            for(const auto& message : m_EarlyMessageBuffer)
            {
                m_LogFile << message << std::endl;
            }
            m_EarlyMessageBuffer.clear();
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
            else
            {
                m_EarlyMessageBuffer.push_back(message);
            }
        }

    private:
        Logger() {}
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::ofstream m_LogFile;
        std::vector<std::string> m_EarlyMessageBuffer;
    };
}