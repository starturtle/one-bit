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

 std::ostream& LogStream::getOutStream()
  {
    return m_outStream;
  }
 Level LogStream::getMinimumLogLevel() const
 {
   return m_minLogLevel;
 }

 Level LogStream::getLogLevel() const
 {
   return m_currentLogLevel;
 }

  template<typename T>
  LogStream& LogStream::operator<<(T arg)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel))
    {
      getOutStream() << arg;
    }
    return *this;
  }

  template<>
  LogStream& LogStream::operator<< <Level>(Level lvl)
  {
    if (isActive(m_currentLogLevel, m_minLogLevel) && (Level::OFF == lvl))
    {
      getOutStream() << std::endl;
    }
    if (lvl >= m_minLogLevel) // if OFF wasn't the highest value in the enum, this would need further handling!
    {
      m_currentLogLevel = lvl;
    }

    // else leave m_currentLogLevel at what it now is (likely OFF)
    return *this;
  }

#define STIXELATOR_LOG_TEMPLATE_SPECIALIZATION(type)  template<> LogStream& LogStream::operator<< <type>(type arg) {if (isActive(m_currentLogLevel, m_minLogLevel)) getOutStream() << arg; return *this;}

  STIXELATOR_LOG_TEMPLATE_SPECIALIZATION(const char*)
  STIXELATOR_LOG_TEMPLATE_SPECIALIZATION(int)
  STIXELATOR_LOG_TEMPLATE_SPECIALIZATION(unsigned)
  STIXELATOR_LOG_TEMPLATE_SPECIALIZATION(std::string)
  STIXELATOR_LOG_TEMPLATE_SPECIALIZATION(double)

#undef STIXELATOR_LOG_TEMPLATE_SPECIALIZATION

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

TEST_CASE("Test is-active check") {
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
  CHECK_FALSE(isActive(logging::Level::OFF, logging::Level::DEBUG));
  CHECK_FALSE(isActive(logging::Level::OFF, logging::Level::NOTE));
  CHECK_FALSE(isActive(logging::Level::OFF, logging::Level::WARNING));
  CHECK_FALSE(isActive(logging::Level::OFF, logging::Level::ERR));
  CHECK_FALSE(isActive(logging::Level::OFF, logging::Level::SELF_DESTRUCT));
}

class StringLogStream : public logging::LogStream
{
  std::stringstream m_testOutStream;
public:
  static StringLogStream& instance()
  {
    static StringLogStream instance;
    return instance;
  }
  StringLogStream() : LogStream(), m_testOutStream{}{}
  std::ostream& getOutStream() final
  {
    return m_testOutStream;
  }
  void clear() { *this << logging::Level::OFF; m_testOutStream.clear(); m_testOutStream.str(""); }
  std::string getText() const { return m_testOutStream.str(); }

  logging::Level current() { return getLogLevel(); }
  logging::Level minimum() { return getMinimumLogLevel(); }
};

template<typename T>
void verify_log_by_type(StringLogStream& testStream, logging::Level logLevel, logging::Level minLevel, bool allowed, T inputOriginal, const std::string& expectedString)
{
  const std::string endString{ expectedString.empty() ? "" : expectedString + "\n" };
  CHECK_EQ(minLevel, testStream.minimum());
  CHECK_EQ(logging::Level::OFF, testStream.current());
  testStream << logLevel;
  CHECK_EQ(minLevel, testStream.minimum());
  CHECK_EQ(allowed ? logLevel : logging::Level::OFF, testStream.current());
  testStream << inputOriginal;
  CHECK_EQ(testStream.getText(), expectedString);
  testStream << logging::Level::OFF;
  CHECK_EQ(minLevel, testStream.minimum());
  CHECK_EQ(logging::Level::OFF, testStream.current());
  CHECK_EQ(testStream.getText(), endString);
  testStream << inputOriginal;
  CHECK_EQ(minLevel, testStream.minimum());
  CHECK_EQ(logging::Level::OFF, testStream.current());
  CHECK_EQ(testStream.getText(), endString);
  testStream.clear();
}

template<typename T>
void verify_log_by_type(StringLogStream& testStream, logging::Level minLevel, T inputOriginal, const std::string& inputAsString, const std::vector<logging::Level>& supported, const std::vector<logging::Level>& unsupported)
{
  for (auto& level : unsupported)
  {
    verify_log_by_type(testStream, level, minLevel, false, inputOriginal, std::string());
  }
  for (auto& level : supported)
  {
    verify_log_by_type(testStream, level, minLevel, true, inputOriginal, inputAsString);
  }
}

void test_log_level_logging(logging::Level level_to_test, const std::vector<logging::Level>& supported, const std::vector<logging::Level>& unsupported)
{
  StringLogStream testStream;
  static const std::string testString{ "Test" };
  testStream.setLogLevel(level_to_test);

  // all-in-one-go logging
  for (auto& level : unsupported)
  {
    testStream << level << testString << logging::Level::OFF;
    CHECK_EQ(testStream.getText(), std::string());
    testStream.clear();
  }
  for (auto& level : supported)
  {
    testStream << level << testString << logging::Level::OFF;
    CHECK_EQ(testStream.getText(), testString + "\n");
    testStream.clear();
  }

  // supported log levels do all-in-one-go logging for strings
  verify_log_by_type(testStream, level_to_test, testString, testString, supported, unsupported);
  // supported log levels do all-in-one-go logging for integers
  int testInputInt{ 15 };
  verify_log_by_type(testStream, level_to_test, testInputInt, std::to_string(testInputInt), supported, unsupported);

  // supported log levels do all-in-one-go logging for unsigned
  unsigned testInputUnsigned{ 15 };
  verify_log_by_type(testStream, level_to_test, testInputUnsigned, std::to_string(testInputUnsigned), supported, unsupported);

  // supported log levels do all-in-one-go logging for C-string
  const char* testInputCString = "FunnyString";
  verify_log_by_type(testStream, level_to_test, testInputCString, std::string(testInputCString), supported, unsupported);

  // supported log levels do all-in-one-go logging for double
  double testInputDouble{ 15.5 };
  std::string testInputString = std::to_string(testInputDouble);
  testInputString.erase(testInputString.find_last_not_of('0') + 1, std::string::npos); // remove trailing zeroes because that's what stringstream does, too
  verify_log_by_type(testStream, level_to_test, testInputDouble, testInputString, supported, unsupported);
}

#include <sstream>
TEST_CASE("test level logic")
{
  test_log_level_logging(logging::Level::OFF, 
    {}, 
    { logging::Level::DEBUG, logging::Level::NOTE, logging::Level::WARNING, logging::Level::ERR, logging::Level::SELF_DESTRUCT }
  );
  test_log_level_logging(logging::Level::SELF_DESTRUCT, 
    { logging::Level::SELF_DESTRUCT }, 
    { logging::Level::DEBUG, logging::Level::NOTE, logging::Level::WARNING, logging::Level::ERR }
  );
  test_log_level_logging(logging::Level::ERR, 
    { logging::Level::ERR, logging::Level::SELF_DESTRUCT }, 
    { logging::Level::DEBUG, logging::Level::NOTE, logging::Level::WARNING }
  );
  test_log_level_logging(logging::Level::WARNING, 
    { logging::Level::WARNING, logging::Level::ERR, logging::Level::SELF_DESTRUCT }, 
    { logging::Level::DEBUG, logging::Level::NOTE }
  );
  test_log_level_logging(logging::Level::NOTE, 
    { logging::Level::NOTE, logging::Level::WARNING, logging::Level::ERR, logging::Level::SELF_DESTRUCT }, 
    { logging::Level::DEBUG }
  );
  test_log_level_logging(logging::Level::DEBUG, 
    { logging::Level::DEBUG, logging::Level::NOTE, logging::Level::WARNING, logging::Level::ERR, logging::Level::SELF_DESTRUCT }, 
    {}
  );
}

#endif