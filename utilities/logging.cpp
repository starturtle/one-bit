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
  LogStream& LogStream::operator<< <Level>(Level lvl)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel) && (Level::OFF == lvl))
    {
      m_outStream << std::endl;
    }
    if (lvl >= m_minLogLevel) // if OFF wasn't the highest value in the enum, this would need further handling!
    {
      m_currentLogLevel = lvl;
    }

    // else leave m_currentLogLevel at what it now is (likely OFF)
    return *this;
  }

#define LOG_TEMPLATE_SPECIALIZATION(type)  template<> LogStream& LogStream::operator<< <type>(type arg) {if (isActive(m_currentLogLevel, m_minLogLevel)) m_outStream << arg; return *this;}

  LOG_TEMPLATE_SPECIALIZATION(const char*)
  LOG_TEMPLATE_SPECIALIZATION(int)
  LOG_TEMPLATE_SPECIALIZATION(unsigned)
  LOG_TEMPLATE_SPECIALIZATION(std::string)
  LOG_TEMPLATE_SPECIALIZATION(double)

#undef LOG_TEMPLATE_SPECIALIZATION

  LogStream& logger() { return LogStream::instance(); }
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
  // everything prints to DEBUG
  CHECK(isActive(logging::Level::DEBUG, logging::Level::DEBUG));
  CHECK(isActive(logging::Level::NOTE, logging::Level::DEBUG));
  CHECK(isActive(logging::Level::WARNING, logging::Level::DEBUG));
  CHECK(isActive(logging::Level::ERR, logging::Level::DEBUG));
  CHECK(isActive(logging::Level::SELF_DESTRUCT, logging::Level::DEBUG));

  // only DEBUG should get blocked when log level is NOTE or higher
  CHECK_FALSE(isActive(logging::Level::DEBUG, logging::Level::NOTE));
  CHECK(isActive(logging::Level::NOTE, logging::Level::NOTE));
  CHECK(isActive(logging::Level::WARNING, logging::Level::NOTE));
  CHECK(isActive(logging::Level::ERR, logging::Level::NOTE));
  CHECK(isActive(logging::Level::SELF_DESTRUCT, logging::Level::NOTE));

  // only DEBUG and NOTE should get blocked when log level is WARNING or higher
  CHECK_FALSE(isActive(logging::Level::DEBUG, logging::Level::WARNING));
  CHECK_FALSE(isActive(logging::Level::NOTE, logging::Level::WARNING));
  CHECK(isActive(logging::Level::WARNING, logging::Level::WARNING));
  CHECK(isActive(logging::Level::ERR, logging::Level::WARNING));
  CHECK(isActive(logging::Level::SELF_DESTRUCT, logging::Level::WARNING));

  // only error, self destruct and newline should log when logging is ERR or higher
  CHECK_FALSE(isActive(logging::Level::DEBUG, logging::Level::ERR));
  CHECK_FALSE(isActive(logging::Level::NOTE, logging::Level::ERR));
  CHECK_FALSE(isActive(logging::Level::WARNING, logging::Level::ERR));
  CHECK(isActive(logging::Level::ERR, logging::Level::ERR));
  CHECK(isActive(logging::Level::SELF_DESTRUCT, logging::Level::ERR));

  // only self destruct and newline should log when logging is at SELF_DESTRUCT
  CHECK_FALSE(isActive(logging::Level::DEBUG, logging::Level::SELF_DESTRUCT));
  CHECK_FALSE(isActive(logging::Level::NOTE, logging::Level::SELF_DESTRUCT));
  CHECK_FALSE(isActive(logging::Level::WARNING, logging::Level::SELF_DESTRUCT));
  CHECK_FALSE(isActive(logging::Level::ERR, logging::Level::SELF_DESTRUCT));
  CHECK(isActive(logging::Level::SELF_DESTRUCT, logging::Level::SELF_DESTRUCT));

  // nothing at all should log when logging is globally OFF
  CHECK_FALSE(isActive(logging::Level::DEBUG, logging::Level::OFF));
  CHECK_FALSE(isActive(logging::Level::NOTE, logging::Level::OFF));
  CHECK_FALSE(isActive(logging::Level::WARNING, logging::Level::OFF));
  CHECK_FALSE(isActive(logging::Level::ERR, logging::Level::OFF));
  CHECK_FALSE(isActive(logging::Level::SELF_DESTRUCT, logging::Level::OFF));
  CHECK_FALSE(isActive(logging::Level::OFF, logging::Level::OFF));

  // even if logging is globally enabled, outside log messages (current log level is set to OFF) logging should not get printed.
  CHECK(isActive(logging::Level::OFF, logging::Level::DEBUG));
  CHECK(isActive(logging::Level::OFF, logging::Level::NOTE));
  CHECK(isActive(logging::Level::OFF, logging::Level::WARNING));
  CHECK(isActive(logging::Level::OFF, logging::Level::ERR));
  CHECK(isActive(logging::Level::OFF, logging::Level::SELF_DESTRUCT));
}

#endif