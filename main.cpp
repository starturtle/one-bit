#include <iostream>
#include "imaging/ShrinkPixelator.h"
#include "logging.h"

int main(int argc, char* argv[])
{
	logging::LogStream logger;
	logger.setLogLevel(logging::Level::WARNING);
	if (argc != 3 && argc != 5)
	{
		logger.getLogStream(logging::Level::ERROR) << "Usage: one-bit <infile> <outfile> [<rows> <cols>]" << std::endl;
		return 1;
	}

	// load image
	imaging::ShrinkPixelator pixelator(argv[1]);
	if (pixelator.empty())
	{
		logger.getLogStream(logging::Level::ERROR) << "Input file [" << argv[1] << "] is empty!" << std::endl;
		return 2;
	}
	
	logger.getLogStream(logging::Level::INFO) << pixelator.imageStats() << std::endl;

	unsigned rows = 50, columns = 70;
	if (argc == 5)
	{
		try
		{
			rows = std::stoi(argv[3]);
			columns = std::stoi(argv[4]);
		}
		catch (...)
		{
			logger.getLogStream(logging::Level::ERROR) << "Either " << argv[3] << " or " << argv[4] << " is not a valid integer." << std::endl;
			return 3;
		}
	}

	// convert to binary image
	if (!pixelator.prepare())
	{
		logger.getLogStream(logging::Level::ERROR) << "Failed to prepare!" << std::endl;
		return 4;
	}
	if (!pixelator.to_pixels(rows, columns))
	{
		logger.getLogStream(logging::Level::ERROR) << "Failed to pixelate!" << std::endl;
		return 5;
	}
	if (!pixelator.to_file(argv[2]))
	{
		logger.getLogStream(logging::Level::ERROR) << "Failed to pixelate!" << std::endl;
		return 6;
	}
	return 0;
}