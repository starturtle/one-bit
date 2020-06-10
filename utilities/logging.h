#pragma once
#include <string>
#include <iostream>
#include <fstream>

namespace logging
{
  enum class Level : int
  {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    SELF_DESTRUCT,
  };

  class LogStream
  {
    std::ostream& m_outStream;
    std::ofstream m_nullStream;
    Level m_minLogLevel;
    LogStream();
  public:
    static LogStream& instance();
    void setLogLevel(Level logLevel);
    std::ostream& getLogStream(Level logLevel);
  };
}