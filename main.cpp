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
	auto imageFile = imaging::load(argv[1]);
	// convert to binary image
	auto convertedFile = imaging::binarize(imageFile);
	// store image as copy
	imaging::store(argv[2], imageFile);
	return 0;
}