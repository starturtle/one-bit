#pragma once
#include <string>
#include <map>
#include <functional>
#include <optional>
#include "Property.hpp"

using string = std::string;
namespace one_bit
{
class ArgumentParser
{
	OPTIONAL_PROPERTY(int, rows)
	OPTIONAL_PROPERTY(int, columns)
	OPTIONAL_PROPERTY(string, input_file)
	OPTIONAL_PROPERTY(string, output_file)
	OPTIONAL_PROPERTY(bool, use_gui)
public:
	ArgumentParser();
	bool parseArgs(int argc, char** argv);
private:
	std::map<string, std::function<bool(const string&)>> initParser();

	int parse_delegate_int(const string& in_arg_val);
	string parse_delegate_string(const string& in_arg_val);
	bool parse_delegate_bool(const string& in_arg_val);
	const std::map<string, std::function<bool(const string&)> > parsers;
};
}