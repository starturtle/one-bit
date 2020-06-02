#pragma once
#include "Pixelator.h"
namespace imaging
{
class ShrinkPixelator : public Pixelator
{
public:
  ShrinkPixelator(const std::string& in_path);
  bool to_pixels(int width, int height, int gauge_stitches, int gauge_rows) final;
};
}

