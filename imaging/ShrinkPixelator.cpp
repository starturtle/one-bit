#include "ShrinkPixelator.h"
namespace imaging
{
	ShrinkPixelator::ShrinkPixelator(const std::string& in_path) : Pixelator{ in_path } {}

	bool ShrinkPixelator::to_pixels(int rows, int columns)
	{
		return false;
	}
}