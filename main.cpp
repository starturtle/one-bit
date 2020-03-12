#include <iostream>
#include "imaging.h"

int main(int argc, char* argv[])
{
	if (argc != 3 && argc != 5)
	{
		std::cout << "Usage: one-bit <infile> <outfile> [<rows> <cols>]" << std::endl;
		return 1;
	}

	uint rows = 50, columns = 70;
	if (argc == 5)
	{
		try 
		{
			rows = std::stoi(argv[3]);
			columns = std::stoi(argv[4]);
		}
		catch (...)
		{
			std::cout << "Either " << argv[3] << " or " << argv[4] << " is not a valid integer." << std::endl;
			return 3;
		}
	}

	// load image
	ImgData imageFile = imaging::load(argv[1]);
	if (imageFile.empty())
	{
		std::cout << "Input file [" << argv[1] << "] is empty!" << std::endl;
		return 2;
	}
	std::cout << "Image has " << imageFile.rows << "x" << imageFile.cols << " pixels and " << imageFile.channels() << " channels" << std::endl;
	// convert to binary image
	std::vector<PixelValue> values{ cv::Vec3b(255, 255, 255), cv::Vec3b() };
	ImgData convertedFile = imaging::binarize(imageFile, values);
	// TODO: pixelate into NxM pixels
	ImgData pixelatedFile = imaging::pixelate(convertedFile, rows, columns);
	// store image as copy
	imaging::store(argv[2], pixelatedFile);
	return 0;
}