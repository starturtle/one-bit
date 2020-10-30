#include <iostream>
#define DOCTEST_CONFIG_DISABLE
#include "utilities/error_codes.h"
#include "utilities/ArgumentParser.h"
#ifdef USE_QT5
#include "qtgui/UiApplication.h"
#endif

int main(int argc, char* argv[])
{
  one_bit::ArgumentParser parser;
  if (! parser.parseArgs(argc, argv)) return errors::PARSE_FAILED;
  return gui_mode::run_as_window(argc, argv, parser);
}