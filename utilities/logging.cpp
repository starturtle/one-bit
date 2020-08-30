#include "logging.h"

namespace logging
{

  LogStream& LogStream::instance()
  {
    static LogStream instance;
    return instance;
  }
  LogStream::LogStream() : m_outStream{ std::cout }, m_nullStream{}, m_minLogLevel{ Level::SELF_DESTRUCT }
  {
    m_nullStream.setstate(std::ios_base::badbit);
  }

  void LogStream::setLogLevel(Level logLevel) { 
    m_minLogLevel = logLevel; 
  }

  std::ostream& LogStream::getLogStream(Level logLevel)
  {
    if (logLevel >= m_minLogLevel)
    {
      return m_outStream;
    }
    return m_nullStream;
  }
}