#pragma once
#include "Pixelator.h"
namespace imaging
{
class ShrinkPixelator : public Pixelator
{
public:
	ShrinkPixelator(const std::string& in_path);
	bool to_pixels(int rows, int columns) final;
};
}

