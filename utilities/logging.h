#pragma once
#include <string>
#include <iostream>
#include <fstream>

namespace logging
{
  enum class Level : int
  {
    DEBUG = 0,
    NOTE,
    WARNING,
    ERR,
    SELF_DESTRUCT,
    OFF,
  };

  class LogStream
  {
  public:
    static LogStream& instance();
    void setLogLevel(Level logLevel);
    template<typename T>
    LogStream& operator<<(T arg);
  protected:
    LogStream();
    virtual std::ostream& getOutStream();
    Level getMinimumLogLevel() const;
    Level getLogLevel() const;
  private:
    std::ostream& m_outStream;
    Level m_minLogLevel;
    Level m_currentLogLevel;
  };

  LogStream& logger();
}