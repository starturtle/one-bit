#include "logging.h"

namespace
{
  bool isActive(logging::Level current, logging::Level min)
  {
    return (current != logging::Level::OFF) && (current >= min);
  }
}

namespace logging
{

  LogStream& LogStream::instance()
  {
    static LogStream instance;
    return instance;
  }
  LogStream::LogStream() : m_outStream{ std::cout }, m_minLogLevel{ Level::ERROR }, m_currentLogLevel{ Level::OFF }{}

  void LogStream::setLogLevel(Level logLevel) { 
    m_minLogLevel = logLevel; 
  }

  template<typename T>
  LogStream& LogStream::operator<<(T arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      m_outStream << arg;
    }
    return *this;
  }

  template<>
  LogStream& LogStream::operator<<<Level>(Level lvl)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel) && (Level::OFF == lvl))
    {
      m_outStream << std::endl;
    }
    if (lvl >= m_minLogLevel)
    {
      m_currentLogLevel = lvl;
    }
    return *this;
  }
  template<>
  LogStream& LogStream::operator<<<const char*>(const char *arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      m_outStream << arg;
    }
    return *this;
  }
  template<>
  LogStream& LogStream::operator<<<int>(int arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      m_outStream << arg;
    }
    return *this;
  }
  template<>
  LogStream& LogStream::operator<<<unsigned>(unsigned arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      m_outStream << arg;
    }
    return *this;
  }
  template<>
  LogStream& LogStream::operator<<<std::string>(std::string arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      m_outStream << arg;
    }
    return *this;
  }
  template<>
  LogStream& LogStream::operator<<<double>(double arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      m_outStream << arg;
    }
    return *this;
  }
}