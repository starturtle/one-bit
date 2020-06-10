#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "setting_enums.h"

using ImgData=cv::Mat;
using PixelValue=cv::Vec3b;

namespace imaging
{
  class Pixelator
  {
  public:
    Pixelator(const std::string& in_path); 
    virtual bool run(const std::string& in_writePath, const int in_width, const int in_height, const int in_gauge_stitches, const int in_gauge_rows, const one_bit::CropRegion in_cropRegion);
    bool prepare();
    bool to_file(const std::string& in_path) const;
    void add_color(PixelValue in_color);
    bool empty() const;
    virtual bool to_pixels(int width, int height, int gauge_stitches, int gauge_rows, const one_bit::CropRegion in_cropRegion) = 0;
    virtual std::string imageStats() const;
    virtual bool display(const std::string& in_imageName);

  protected:
    bool binarize();

    ImgData pictureBuffer;
    std::vector<PixelValue> resultColors;
  };

}