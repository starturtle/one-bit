#include <iostream>
#include "imaging.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: one-bit <infile> <outfile>" << std::endl;
		return 1;
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
	// store image as copy
	imaging::store(argv[2], convertedFile);
	return 0;
}