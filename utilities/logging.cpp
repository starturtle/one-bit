#include "logging.h"

namespace
{
  bool isActive(logging::Level current, logging::Level min);
}

namespace logging
{

  LogStream& LogStream::instance()
  {
    static LogStream instance;
    return instance;
  }
  LogStream::LogStream() : m_outStream{ std::cout }, m_minLogLevel{ Level::ERR }, m_currentLogLevel{ Level::OFF }{}

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
#include <vector>
namespace
{
  bool isActive(logging::Level current, logging::Level min)
  {
    return (current != logging::Level::OFF) && (current >= min);
  }
}


#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <vector>

TEST_CASE("Test is-active check")
{
  std::vector<logging::Level> levels({ 
    logging::Level::DEBUG,
    logging::Level::NOTE,
    logging::Level::WARNING,
    logging::Level::ERR,
    logging::Level::SELF_DESTRUCT,
    logging::Level::OFF
  });
  for (size_t minLevelIdx = 0; minLevelIdx < levels.size(); ++minLevelIdx)
  {
    for (size_t logIdx = 0; logIdx < levels.size(); ++logIdx)
    {
      CHECK_EQ(isActive(levels[logIdx], levels[minLevelIdx]), logIdx >= minLevelIdx && levels[logIdx] != logging::Level::OFF);
    }
  }
}

#endif