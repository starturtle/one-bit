#include "script_mode.h"
#include "imaging/ShrinkPixelator.h"
#include "utilities/error_codes.h"
#include "utilities/logging.h"

namespace script_mode
{
int run_as_script(const one_bit::ArgumentParser& in_params)
{
  logging::logger().setLogLevel(logging::Level::WARNING);
  if ((in_params.has_width() != in_params.has_height()) || (! in_params.has_input_file()) || (! in_params.has_output_file()))
  {
    logging::logger() << logging::Level::ERR << "Usage: one-bit [-gui=\"true|false\"] [-infile=<infile>] [-outfile=<outfile>] [-rows=<rows> -cols=<cols>]" << logging::Level::OFF;
    logging::logger() << logging::Level::ERR << "  Note: infile and outfile are only optional if gui is true!" << logging::Level::OFF;
    return errors::PARSE_FAILED;
  }

  // load image
  imaging::ShrinkPixelator pixelator(in_params.get_input_file());
  pixelator.display("Before");
  if (pixelator.empty())
  {
    logging::logger() << logging::Level::ERR << "Input file [" << in_params.get_input_file() << "] is empty!" << logging::Level::OFF;
    return errors::WRONG_INPUT_FILE;
  }
  
  logging::logger() << logging::Level::NOTE << pixelator.imageStats() << logging::Level::OFF;

  // convert to binary image
  if (!pixelator.prepare())
  {
    logging::logger() << logging::Level::ERR << "Failed to prepare!" << logging::Level::OFF;
    return errors::PREPARATION_ERROR;
  }
  if (!pixelator.to_pixels(in_params.get_width(), in_params.get_height(), in_params.get_gauge_stitches(), in_params.get_gauge_rows(), in_params.get_crop_region()))
  {
    logging::logger() << logging::Level::ERR << "Failed to pixelate!" << logging::Level::OFF;
    return errors::PIXELATION_ERROR;
  }
  pixelator.display("After");
  if (!pixelator.to_file(in_params.get_output_file()))
  {
    logging::logger() << logging::Level::ERR << "Failed to pixelate!" << logging::Level::OFF;
    return errors::PRINT_ERROR;
  }
  return 0;
}
}