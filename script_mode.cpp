#include "script_mode.h"
#include "imaging/ShrinkPixelator.h"
#include "utilities/error_codes.h"
#include "utilities/logging.h"

namespace script_mode
{
int run_as_script(const one_bit::ArgumentParser& in_params)
{
	logging::LogStream logger;
	logger.setLogLevel(logging::Level::WARNING);
	if ((in_params.has_columns() != in_params.has_rows()) || (! in_params.has_input_file()) || (! in_params.has_output_file()))
	{
		logger.getLogStream(logging::Level::ERROR) << "Usage: one-bit [-gui=\"true|false\"] [-infile=<infile>] [-outfile=<outfile>] [-rows=<rows> -cols=<cols>]" << std::endl;
		logger.getLogStream(logging::Level::ERROR) << "    Note: infile and outfile are only optional if gui is true!" << std::endl;
		return errors::PARSE_FAILED;
	}

	// load image
	imaging::ShrinkPixelator pixelator(in_params.get_input_file());
	pixelator.display("Before");
	if (pixelator.empty())
	{
		logger.getLogStream(logging::Level::ERROR) << "Input file [" << in_params.get_input_file() << "] is empty!" << std::endl;
		return errors::WRONG_INPUT_FILE;
	}
	
	logger.getLogStream(logging::Level::INFO) << pixelator.imageStats() << std::endl;

	// convert to binary image
	if (!pixelator.prepare())
	{
		logger.getLogStream(logging::Level::ERROR) << "Failed to prepare!" << std::endl;
		return errors::PREPARATION_ERROR;
	}
	if (!pixelator.to_pixels(in_params.get_rows(), in_params.get_columns()))
	{
		logger.getLogStream(logging::Level::ERROR) << "Failed to pixelate!" << std::endl;
		return errors::PIXELATION_ERROR;
	}
	pixelator.display("After");
	if (!pixelator.to_file(in_params.get_output_file()))
	{
		logger.getLogStream(logging::Level::ERROR) << "Failed to pixelate!" << std::endl;
		return errors::PRINT_ERROR;
	}
	return 0;
}
}