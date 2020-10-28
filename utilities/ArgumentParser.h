#pragma once
#include <string>
#include <map>
#include <functional>
#include <optional>
#include "Property.hpp"
#include "setting_enums.h"

namespace one_bit
{
  class ArgumentParser;
}
class DoctestArgumentParser
{
public:
  int getInt(const std::string& stringToParse, one_bit::ArgumentParser& argParser);
  std::string getString(const std::string& stringToParse, one_bit::ArgumentParser& argParser);
  bool getBool(const std::string& stringToParse, one_bit::ArgumentParser& argParser);
  one_bit::UiMode getUiMode(const std::string& stringToParse, one_bit::ArgumentParser& argParser);
  one_bit::CropRegion getCropRegion(const std::string& stringToParse, one_bit::ArgumentParser& argParser);
};
using string = std::string;
namespace one_bit
{
class ArgumentParser
{
#ifndef DOCTEST_CONFIG_DISABLE
  friend class DoctestArgumentParser;
#endif
  OPTIONAL_PROPERTY(int, height)
  OPTIONAL_PROPERTY(int, width)
  OPTIONAL_PROPERTY(int, gauge_stitches)
  OPTIONAL_PROPERTY(int, gauge_rows)
  OPTIONAL_PROPERTY(string, input_file)
  OPTIONAL_PROPERTY(string, output_file)
  OPTIONAL_PROPERTY(UiMode, use_gui)
  OPTIONAL_PROPERTY(CropRegion, crop_region)
public:
  ArgumentParser();
  bool parseArgs(int argc, char** argv);
private:
  std::map<string, std::function<bool(const string&)>> initParser();

  int parse_delegate_int(const string& in_arg_val);
  string parse_delegate_string(const string& in_arg_val);
  bool parse_delegate_bool(const string& in_arg_val);
  UiMode parse_delegate_UiMode(const string& in_arg_val);
  CropRegion parse_delegate_CropRegion(const string& in_arg_val);
  const std::map<string, std::function<bool(const string&)> > parsers;
};
}