#include "ShrinkPixelator.h"
#include <optional>

namespace
{
  unsigned greatest_common_divisor(unsigned a, unsigned b);
}

namespace imaging
{
  ShrinkPixelator::ShrinkPixelator(const std::string& in_path) : Pixelator{ in_path } {}

  bool ShrinkPixelator::to_pixels(int in_width, int in_height, int gauge_stitches, int gauge_rows, const one_bit::CropRegion in_cropRegion)
  {
    ImgData buffer;
    const double aspect_ratio_epsilon = 0.01;
    int rows_backup = pictureBuffer.rows;
    int columns_backup = pictureBuffer.cols;
    double aspect_ratio_old = (rows_backup * 1.) / columns_backup; 
    unsigned stixelGcd{ greatest_common_divisor(gauge_rows, gauge_stitches) };
    unsigned stitchWidth = gauge_stitches / stixelGcd;
    unsigned stitchHeight = gauge_rows / stixelGcd;
    unsigned stitchCount = (unsigned)(ceil((in_width / 10.) * gauge_stitches));
    unsigned rowCount = (unsigned)(ceil((in_height / 10.) * gauge_rows));
    double aspect_ratio_new = (in_height * 1.) / in_width;
    bool crop_vertical = (aspect_ratio_old - aspect_ratio_new) > aspect_ratio_epsilon;
    bool crop_horizontal = (aspect_ratio_new - aspect_ratio_old) > aspect_ratio_epsilon;
    try
    {
      cv::Rect pictureBufferROI;
      int pictureWidth = stitchWidth * stitchCount;
      int pictureHeight = stitchHeight * rowCount;
      pictureBufferROI.width = pictureWidth;
      pictureBufferROI.height = pictureHeight;
      if (crop_horizontal)
      {
        switch (in_cropRegion)
        {
        case one_bit::CropRegion::BOTTOM_LEFT:
        case one_bit::CropRegion::LEFT:
        case one_bit::CropRegion::TOP_LEFT:
          pictureBufferROI.x = 0;
          break;
        case one_bit::CropRegion::BOTTOM_RIGHT:
        case one_bit::CropRegion::RIGHT:
        case one_bit::CropRegion::TOP_RIGHT:
          pictureBufferROI.x = pictureBuffer.cols - pictureWidth;
          break;
        default:
          pictureBufferROI.x = (pictureBuffer.cols - pictureWidth) / 2;
          break;
        }
        pictureBufferROI.y = 0;
      }
      else if (crop_vertical)
      {
        switch (in_cropRegion)
        {
        case one_bit::CropRegion::TOP_LEFT:
        case one_bit::CropRegion::TOP:
        case one_bit::CropRegion::TOP_RIGHT:
          pictureBufferROI.y = 0;
          break;
        case one_bit::CropRegion::BOTTOM_LEFT:
        case one_bit::CropRegion::BOTTOM:
        case one_bit::CropRegion::BOTTOM_RIGHT:
          pictureBufferROI.y = pictureBuffer.rows - pictureHeight;
          break;
        default:
          pictureBufferROI.y = (pictureBuffer.rows - pictureHeight) / 2;
          break;
        }
        pictureBufferROI.x = 0;
      }
      auto crop = pictureBuffer(pictureBufferROI);
      cv::resize(crop, buffer, cv::Size(stitchCount, rowCount));
      cv::swap(pictureBuffer, buffer);
      binarize();
      cv::swap(pictureBuffer, buffer);
      cv::resize(buffer, pictureBuffer, cv::Size(pictureWidth, pictureHeight), 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
      return true;
    }
    catch (...)
    {
      return false;
    }
  }
}

namespace
{
  std::optional<unsigned> divides(unsigned number, unsigned divisor)
  {
    unsigned result = number / divisor;
    if (result * divisor == number) return result;
    return std::optional<unsigned>{};
  }

  std::vector<unsigned> divisors(unsigned x)
  {
    std::vector<unsigned> divisors;
    for (unsigned factor = 1; factor < sqrt(x); ++factor)
    {
      auto result = divides(x, factor);
      if (result.has_value())
      {
        divisors.push_back(factor);
        divisors.push_back(result.value());
      }
    }
    return divisors;
  }

  unsigned search_greatest_common_divisor(unsigned smallerValue, unsigned largerValue)
  {
    auto divisors_smaller = divisors(smallerValue);
    auto divisors_larger = divisors(largerValue);
    std::sort(divisors_smaller.begin(), divisors_smaller.end(), [](unsigned one, unsigned other) { return other < one; });
    std::sort(divisors_larger.begin(), divisors_larger.end());
    for (auto divisor : divisors_smaller)
    {
      if (std::find(divisors_larger.begin(), divisors_larger.end(), divisor) != divisors_larger.end()) return divisor;
    }
    return 1;
  }

  unsigned greatest_common_divisor(unsigned a, unsigned b)
  {
    if (a == b) return a;
    if (a < b)
    {
      if (divides(b, a).has_value()) return a;
      return search_greatest_common_divisor(a, b);
    }
    else
    {
      if (divides(a, b).has_value()) return b;
      return search_greatest_common_divisor(b, a);
    }
  }
}
