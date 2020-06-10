#include <iostream>
#include "utilities/error_codes.h"
#include "utilities/ArgumentParser.h"
#ifdef USE_QT5
#include "qtgui/UiApplication.h"
#endif
#include "script_mode.h"

int main(int argc, char* argv[])
{
  one_bit::ArgumentParser parser;
  if (! parser.parseArgs(argc, argv)) return errors::PARSE_FAILED;
#ifdef USE_QT5
  if (parser.get_use_gui() == one_bit::UiMode::QT) return gui_mode::run_as_window(argc, argv, parser);
#endif
  return script_mode::run_as_script(parser);
}