#include "ShrinkPixelator.h"
#include "calculus.h"
#include <optional>
#include <set>

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
    unsigned stixelLcm{ calculus::least_common_multiple(gauge_rows, gauge_stitches) };
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

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
class TestShrinkPixelator : public imaging::ShrinkPixelator
{
public:
  TestShrinkPixelator() : ShrinkPixelator("") { pictureBuffer = ImgData(200, 200, CV_8UC3, cv::Scalar{ 60, 0, 255, 0 }); }
  auto pictureColumns() const { return pictureBuffer.cols; }
  auto pictureRows() const { return pictureBuffer.rows; }
};

TEST_CASE("test pixelation") {
  TestShrinkPixelator pixor;
  pixor.prepare();
  CHECK(pixor.to_pixels(8, 8, 20, 18, one_bit::CropRegion::CENTER));
  MESSAGE("This is a stub. The class must be rewritten in order to be unit testable properly.");
}

#endif