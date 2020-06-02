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
  if (std::find(false_values.begin(), true_values.end(), in_arg_val) != false_values.end())
    return false;
  throw std::invalid_argument(in_arg_val + " doesn't evaluate to bool");
}

one_bit::UiMode ArgumentParser::parse_delegate_UiMode(const string& in_arg_val)
{
  if (in_arg_val == "NONE")  return UiMode::NONE;
#ifdef USE_QT5
  if (in_arg_val == "QT")  return UiMode::QT;
#endif
#ifdef USE_CEGUI
  if (in_arg_val == "CEGUI") return UiMode::CEGUI;
#endif
  throw std::invalid_argument(in_arg_val + " is not a valid UI mode enum name");
}

one_bit::CropRegion ArgumentParser::parse_delegate_CropRegion(const string& in_arg_val)
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
  return CropRegion::TOP_LEFT;
}
}