#include "ArgumentParser.h"
#include <string>
#include <stdexcept>
#include <algorithm>
#include <vector>

namespace one_bit
{
ArgumentParser::ArgumentParser(): parsers{initParser()}{}

bool ArgumentParser::parseArgs(int argc, char** argv)
{
  for (int argNo=1; argNo < argc; ++argNo)
  {
    std::string argument{argv[argNo]};
    
    auto equalsSign = argument.find("=");
    if (equalsSign == std::string::npos) continue;
    // assume the argument will be handled by the UI base application if it doesn't have name=value pattern
    
    std::string argName = argument.substr(0, equalsSign);
    const std::string argVal = argument.substr(equalsSign + 1);
    try
    {
      std::function<bool(const string&)> parser = parsers.at(argName);
      if (! parser(argVal)) return false; // argument should have been parsed but parsing failed
      // else argument parsed successfully
    }
    catch(...)
    {
      // assume the argument will be handled by the UI base application if not in parser keys
    }
  }
  
  if (!has_use_gui())
  {
#ifdef USE_QT5
    set_use_gui(UiMode::QT);
#else
    set_use_gui(UiMode::NONE);
#endif
  }

  return true;
}

std::map<std::string,std::function<bool(const std::string&)>> ArgumentParser::initParser()
{
  return std::map<std::string, std::function<bool(const std::string&)> >{
    { "-height", std::bind(&ArgumentParser::parse_height, this, std::placeholders::_1) },
    { "-width", std::bind(&ArgumentParser::parse_width, this, std::placeholders::_1) },
    { "-gauge-st", std::bind(&ArgumentParser::parse_gauge_stitches, this, std::placeholders::_1)},
    { "-gauge-rw", std::bind(&ArgumentParser::parse_gauge_rows, this, std::placeholders::_1) },
    { "-infile", std::bind(&ArgumentParser::parse_input_file, this, std::placeholders::_1) },
    { "-outfile", std::bind(&ArgumentParser::parse_output_file, this, std::placeholders::_1) },
    { "-gui", std::bind(&ArgumentParser::parse_use_gui, this, std::placeholders::_1) },
    { "-crop-region", std::bind(&ArgumentParser::parse_crop_region, this, std::placeholders::_1)}
  };
}

  int ArgumentParser::parse_delegate_int(const string& in_arg_val)
  {
    return std::stoi(in_arg_val);
  }

  string ArgumentParser::parse_delegate_string(const string& in_arg_val)
  {
    return in_arg_val;
  }

  bool ArgumentParser::parse_delegate_bool(const string& in_arg_val)
  {
    static const std::vector<std::string> true_values{ "true", "True", "TRUE", "t", "y", "1" };
    static const std::vector<std::string> false_values{ "false", "False", "FALSE", "f", "n", "0" };
    if (std::find(true_values.begin(), true_values.end(), in_arg_val) != true_values.end())
      return true;
    if (std::find(false_values.begin(), false_values.end(), in_arg_val) != false_values.end())
      return false;
    throw std::invalid_argument(in_arg_val + " doesn't evaluate to bool");
  }

  UiMode ArgumentParser::parse_delegate_UiMode(const string& in_arg_val)
  {
    if (in_arg_val == "NONE")  return UiMode::NONE;
#ifdef USE_QT5
    if (in_arg_val == "QT")  return UiMode::QT;
#endif
    throw std::invalid_argument(in_arg_val + " is not a valid UI mode enum name");
  }

  CropRegion ArgumentParser::parse_delegate_CropRegion(const string& in_arg_val)
  {
    if (in_arg_val == "TOP_LEFT") return CropRegion::TOP_LEFT;
    if (in_arg_val == "TOP") return CropRegion::TOP;
    if (in_arg_val == "TOP_RIGHT") return CropRegion::TOP_RIGHT;
    if (in_arg_val == "LEFT") return CropRegion::LEFT;
    if (in_arg_val == "CENTER") return CropRegion::CENTER;
    if (in_arg_val == "RIGHT") return CropRegion::RIGHT;
    if (in_arg_val == "BOTTOM_LEFT") return CropRegion::BOTTOM_LEFT;
    if (in_arg_val == "BOTTOM") return CropRegion::BOTTOM;
    if (in_arg_val == "BOTTOM_RIGHT") return CropRegion::BOTTOM_RIGHT;
    // value is optional, defaults to TOP_LEFT.
    return one_bit::CropRegion::TOP_LEFT;
  }
}

#include <doctest.h>

int DoctestArgumentParser::getInt(const std::string& stringToParse, one_bit::ArgumentParser& argParser)
{
#ifdef DOCTEST_CONFIG_DISABLE
  return -1;
#else
  return argParser.parse_delegate_int(stringToParse);
#endif
}

std::string DoctestArgumentParser::getString(const std::string& stringToParse, one_bit::ArgumentParser& argParser)
{
#ifdef DOCTEST_CONFIG_DISABLE
  return stringToParse;
#else
  return argParser.parse_delegate_string(stringToParse);
#endif
}
bool DoctestArgumentParser::getBool(const std::string& stringToParse, one_bit::ArgumentParser& argParser)
{
#ifdef DOCTEST_CONFIG_DISABLE
  return false;
#else
  return argParser.parse_delegate_bool(stringToParse);
#endif
}
one_bit::UiMode DoctestArgumentParser::getUiMode(const std::string& stringToParse, one_bit::ArgumentParser& argParser)
{
#ifdef DOCTEST_CONFIG_DISABLE
  return one_bit::UiMode::NONE;
#else
  return argParser.parse_delegate_UiMode(stringToParse);
#endif
}
one_bit::CropRegion DoctestArgumentParser::getCropRegion(const std::string& stringToParse, one_bit::ArgumentParser& argParser)
{
#ifdef DOCTEST_CONFIG_DISABLE
  return one_bit::CropRegion::TOP_LEFT;
#else
  return argParser.parse_delegate_CropRegion(stringToParse);
#endif
}

TEST_CASE("test integer parsing") {
  DoctestArgumentParser argParser;
  one_bit::ArgumentParser parserToTest;
  CHECK_THROWS(argParser.getInt("", parserToTest));
  CHECK_THROWS(argParser.getInt("One", parserToTest));
  CHECK_EQ(argParser.getInt("0", parserToTest), 0);
  CHECK_EQ(argParser.getInt("1", parserToTest), 1);
  CHECK_EQ(argParser.getInt("2147483647", parserToTest), 2147483647);
  CHECK_THROWS(argParser.getInt("2147483648", parserToTest));
  CHECK_EQ(argParser.getInt("-1", parserToTest), -1);
  CHECK_EQ(argParser.getInt("-2147483648", parserToTest), -2147483648);
}
TEST_CASE("test string parsing") {
  DoctestArgumentParser argParser;
  one_bit::ArgumentParser parserToTest;
  std::vector<string> testStrings{
    "", 
    "test", 
    "0xDEADBEEF", 
    //"öäuß", 
    "1234", 
    "string with spaces",
    //"this is a very long string that exceeds 256 characters, so we'll try and figure out whether the parser can cope with that, but it is very hard to come up with that amount of text, so this is mainly gibberish, and perhaps it's all in vain for parser failure",
    //"$p€cial!", 
    "_underscore", 
  };
  for (auto& str : testStrings)
  {
    CHECK_EQ(argParser.getString(str, parserToTest), str);
  }
}

TEST_CASE("test bool parsing") {
  DoctestArgumentParser argParser;
  one_bit::ArgumentParser parserToTest;
  std::vector<string> true_values{ "true", "True", "TRUE", "t", "y", "1" };
  std::vector<string> false_values{ "false", "False", "FALSE", "f", "n", "0" };
  std::vector<string> invalid_values{ "fals", "falsee", "Fals", "Falsee", "FALS", "FALSEE", "FaLsE", "falsf", "tru", "truee", "Tru", "Truee", "TRU", "TRUEE", "TrUe", "truf", "w", "r", "00", "01", "2", "-1", "false.", "true." };
  for (auto& str : true_values)
  {
    CHECK(argParser.getBool(str, parserToTest));
  }
  for (auto& str : false_values)
  {
    CHECK(! argParser.getBool(str, parserToTest));
  }
  for (auto& str : invalid_values)
  {
    CHECK_THROWS(argParser.getBool(str, parserToTest));
  }
}

TEST_CASE("test UiMode parsing") {
  DoctestArgumentParser argParser;
  one_bit::ArgumentParser parserToTest;
  CHECK_EQ(argParser.getUiMode("NONE", parserToTest), one_bit::UiMode::NONE);
#ifdef USE_QT5
  CHECK_EQ(argParser.getUiMode("QT", parserToTest), one_bit::UiMode::QT);
#else
  CHECK_THROWS(argParser.getUiMode("QT", parserToTest));
#endif
  CHECK_THROWS(argParser.getUiMode("Qt", parserToTest));
  CHECK_THROWS(argParser.getUiMode("CEGUI", parserToTest));
  CHECK_THROWS(argParser.getUiMode("GTK", parserToTest));
  CHECK_THROWS(argParser.getUiMode("", parserToTest));
  CHECK_THROWS(argParser.getUiMode("PINK_ELEPHANT", parserToTest));
  CHECK_THROWS(argParser.getUiMode("QTGUI", parserToTest));
  CHECK_THROWS(argParser.getUiMode("NO", parserToTest));
}

TEST_CASE("test CropRegion parsing") {
  DoctestArgumentParser argParser;
  one_bit::ArgumentParser parserToTest;
  CHECK_EQ(argParser.getCropRegion("TOP_LEFT", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("TOP", parserToTest), one_bit::CropRegion::TOP);
  CHECK_EQ(argParser.getCropRegion("TOP_RIGHT", parserToTest), one_bit::CropRegion::TOP_RIGHT);
  CHECK_EQ(argParser.getCropRegion("TOPF", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("TABLE_TOP", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("TO", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("TOP_L", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("TOP_R", parserToTest), one_bit::CropRegion::TOP_LEFT);

  CHECK_EQ(argParser.getCropRegion("LEF", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("LEFT", parserToTest), one_bit::CropRegion::LEFT);
  CHECK_EQ(argParser.getCropRegion("LEFT_HANDED", parserToTest), one_bit::CropRegion::TOP_LEFT);

  CHECK_EQ(argParser.getCropRegion("CENTE", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("CENTER", parserToTest), one_bit::CropRegion::CENTER);
  CHECK_EQ(argParser.getCropRegion("CENTERFOLD", parserToTest), one_bit::CropRegion::TOP_LEFT);

  CHECK_EQ(argParser.getCropRegion("RIGH", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("RIGHT", parserToTest), one_bit::CropRegion::RIGHT);
  CHECK_EQ(argParser.getCropRegion("WRIGHT", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("RIGHT_WRONG", parserToTest), one_bit::CropRegion::TOP_LEFT);

  CHECK_EQ(argParser.getCropRegion("BOTTO", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("BOTTOM_LEFT", parserToTest), one_bit::CropRegion::BOTTOM_LEFT);
  CHECK_EQ(argParser.getCropRegion("BOTTOM", parserToTest), one_bit::CropRegion::BOTTOM);
  CHECK_EQ(argParser.getCropRegion("BOTTOM_RIGHT", parserToTest), one_bit::CropRegion::BOTTOM_RIGHT);
  CHECK_EQ(argParser.getCropRegion("BIKINI_BOTTOM", parserToTest), one_bit::CropRegion::TOP_LEFT);
  CHECK_EQ(argParser.getCropRegion("BOTTOMLINE", parserToTest), one_bit::CropRegion::TOP_LEFT);
}