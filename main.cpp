#include <iostream>
#include "imaging/FloodFillPixelator.h"
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

	// load image
	imaging::FloodFillPixelator pixelator(argv[1]);
	if (pixelator.empty())
	{
		logger.getLogStream(logging::Level::ERROR) << "Input file [" << argv[1] << "] is empty!" << std::endl;
		return 2;
	}
	logger.getLogStream(logging::Level::INFO) << pixelator.imageStats() << std::endl;
	// convert to binary image
	pixelator.add_color(PixelValue(255, 255, 255));
	pixelator.add_color(PixelValue());
	pixelator.to_pixels(rows, columns);
	// store image as copy
	pixelator.to_file(argv[2]);
	return 0;
}