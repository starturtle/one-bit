#include <iostream>
#include "utilities/error_codes.h"
#include "utilities/ArgumentParser.h"
#include "gui/UiApplication.h"
#include "script_mode.h"

int main(int argc, char* argv[])
{
	one_bit::ArgumentParser parser;
	if (! parser.parseArgs(argc, argv)) return errors::PARSE_FAILED;
	if (parser.get_use_gui()) return gui_mode::run_as_window(argc, argv, parser);
	return script_mode::run_as_script(parser);
}