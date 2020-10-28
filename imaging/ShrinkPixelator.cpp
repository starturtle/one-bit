#include "ShrinkPixelator.h"
#include <optional>

namespace
{
  unsigned least_common_multiple(unsigned a, unsigned b);
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
    unsigned stixelLcm{ least_common_multiple(gauge_rows, gauge_stitches) };
    unsigned stitchWidth = stixelLcm / gauge_stitches;
    unsigned stitchHeight = stixelLcm / gauge_rows;
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

  unsigned least_common_multiple(unsigned a, unsigned b)
  {
    unsigned gcd = greatest_common_divisor(a, b);
    return a * b / gcd; //gcd * (a / gcd) * (b / gcd);
  }
}

#include <doctest.h>

TEST_CASE("test divides") {
  auto result = divides(25, 3);
  CHECK(!result.has_value());

  result = divides(24, 3);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 8);

  result = divides(3, 3);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 1);

  result = divides(16, 4);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 4);

  result = divides(0, 2);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 0);

  result = divides(5, 0);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 0);

  result = divides(0, 0);
  CHECK(result.has_value());
  CHECK_EQ(result.value(), 0);

  result = divides(1953125, 16777216);
  CHECK(!result.has_value());
}

TEST_CASE("test divisors") {
  std::vector<unsigned> result;

  result = divisors(5);
  CHECK_EQ(result.size(), 2);
  CHECK_NE(std::find(result.begin(), result.end(), 1), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),5), result.end());

  result = divisors(16);
  CHECK_EQ(result.size(), 5);
  CHECK_NE(std::find(result.begin(), result.end(),1), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),2), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),4), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),8), result.end()); 
  CHECK_NE(std::find(result.begin(), result.end(),16), result.end());

  result = divisors(24);
  CHECK_EQ(result.size(), 8);
  CHECK_NE(std::find(result.begin(), result.end(),1), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),2), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),3), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),4), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),6), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),8), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),12), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),24), result.end());

  result = divisors(36);
  CHECK_EQ(result.size(), 9);
  CHECK_NE(std::find(result.begin(), result.end(),1), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),2), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),3), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),4), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),6), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),9), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),12), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),18), result.end());
  CHECK_NE(std::find(result.begin(), result.end(),36), result.end());

  result = divisors(8192); // 2^13
  CHECK_EQ(result.size(), 14);
  unsigned factorInlist = 1;
  for (auto i = 0; i < 14; ++i)
  {
    CHECK_NE(std::find(result.begin(), result.end(),factorInlist), result.end());
    factorInlist *= 2;
  }

  result = divisors(262144); // 2^18
  CHECK_EQ(result.size(), 19);
  factorInlist = 1;
  for (auto i = 0; i < 19; ++i)
  {
    CHECK_NE(std::find(result.begin(), result.end(),factorInlist), result.end());
    factorInlist *= 2;
  }
}

TEST_CASE("test search_greatest_common_divisor") {
  CHECK_EQ(search_greatest_common_divisor(5, 3), 1);
  CHECK_EQ(search_greatest_common_divisor(59049, 262144), 1);
  CHECK_EQ(search_greatest_common_divisor(1024, 262144), 1024);
  CHECK_EQ(search_greatest_common_divisor(24, 36), 12);
  CHECK_EQ(search_greatest_common_divisor(0, 36), 0);
}

TEST_CASE("test greatest_common_divisor") {
  CHECK_EQ(greatest_common_divisor(5, 3), 1);
  CHECK_EQ(greatest_common_divisor(262144, 59049), 1);
  CHECK_EQ(greatest_common_divisor(262144, 1024), 1024);
  CHECK_EQ(greatest_common_divisor(1024, 262144), 1024);
  CHECK_EQ(greatest_common_divisor(36, 24), 12);
  CHECK_EQ(greatest_common_divisor(36, 0), 0);
}

TEST_CASE("test pair-based least_common_multiple") {
  CHECK_EQ(least_common_multiple(16, 8), 16);
  CHECK_EQ(least_common_multiple(8, 16), 16);
  CHECK_EQ(least_common_multiple(16, 18), 144);
  CHECK_EQ(least_common_multiple(1, 15), 15);
  CHECK_EQ(least_common_multiple(36, 24), 72);
  CHECK_EQ(least_common_multiple(243, 1024), 243 * 1024); // power of 3 and power of 2 don't share factors

  CHECK_EQ(least_common_multiple(0, 18), 0);
  CHECK_EQ(least_common_multiple(18, 0), 0);
  CHECK_EQ(least_common_multiple(0, 0), 0);
}
