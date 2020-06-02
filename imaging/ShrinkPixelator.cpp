#include "ShrinkPixelator.h"

namespace imaging
{
  ShrinkPixelator::ShrinkPixelator(const std::string& in_path) : Pixelator{ in_path } {}

  bool ShrinkPixelator::to_pixels(int width, int height, int gauge_stitches, int gauge_rows)
  {
    ImgData buffer;
    int rows_backup = pictureBuffer.rows;
    int columns_backup = pictureBuffer.cols;
    int columns = width * gauge_stitches / 10;
    int rows = height * gauge_rows / 10;
    try
    {
      cv::resize(pictureBuffer, buffer, cv::Size(columns, rows));
      cv::swap(pictureBuffer, buffer);
      binarize();
      cv::swap(pictureBuffer, buffer);
      cv::resize(buffer, pictureBuffer, cv::Size(columns_backup, rows_backup), 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
      return true;
    }
    catch (...)
    {
      return false;
    }
  }
}