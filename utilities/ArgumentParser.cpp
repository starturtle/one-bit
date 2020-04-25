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
		set_use_gui(true);
	}

	return true;
}

std::map<std::string,std::function<bool(const std::string&)>> ArgumentParser::initParser()
{
	return std::map<std::string, std::function<bool(const std::string&)> >{
		{ "-rows", std::bind(&ArgumentParser::parse_rows, this, std::placeholders::_1) },
		{ "-columns", std::bind(&ArgumentParser::parse_columns, this, std::placeholders::_1) },
		{ "-infile", std::bind(&ArgumentParser::parse_input_file, this, std::placeholders::_1) },
		{ "-outfile", std::bind(&ArgumentParser::parse_output_file, this, std::placeholders::_1) },
		{ "-gui", std::bind(&ArgumentParser::parse_use_gui, this, std::placeholders::_1) },
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

}