#include "Pixelator.h"
namespace
{
  double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom);
}
namespace imaging
{

  Pixelator::Pixelator(const std::string& in_path)
    : pictureBuffer{ cv::imread(in_path, cv::ImreadModes::IMREAD_COLOR) }
    , resultColors{}
  {

  }
  bool Pixelator::run(const std::string& in_writePath, const int in_width, const int in_height, const int in_gauge_stitches, const int in_gauge_rows, const one_bit::CropRegion in_cropRegion)
  {
    return prepare() && to_pixels(in_width, in_height, in_gauge_stitches, in_gauge_rows, in_cropRegion) && to_file(in_writePath);
  }
  bool Pixelator::prepare()
  {
    add_color(PixelValue(0, 0, 255));
    add_color(PixelValue());
    return true;
  }

  bool Pixelator::to_file(const std::string& in_path) const
  {
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    return cv::imwrite(in_path, pictureBuffer, compression_params);
  }

  void Pixelator::add_color(PixelValue in_color)
  {
    if (std::find(resultColors.begin(), resultColors.end(), in_color) == resultColors.end())
    {
      resultColors.push_back(in_color);
    }
  }
  bool Pixelator::empty() const
  {
    return pictureBuffer.empty();
  }
  std::string Pixelator::imageStats() const
  {
    std::stringstream outputStream;
    outputStream << "Image has " << pictureBuffer.rows << "x" << pictureBuffer.cols << " pixels and " << pictureBuffer.channels() << " channels";
    return outputStream.str();
  }

  bool Pixelator::display(const std::string& in_imageName)
  {
    bool hasPicture = ((! pictureBuffer.empty()) && (nullptr != pictureBuffer.data));
    if (hasPicture)
    {
      cv::namedWindow(in_imageName, cv::WINDOW_AUTOSIZE);
      cv::imshow(in_imageName, pictureBuffer);
      cv::waitKey(0);
    }
    return hasPicture;
  }

  bool Pixelator::binarize()
  {
    ImgData hsvFile(pictureBuffer.cols, pictureBuffer.rows, pictureBuffer.type());
    pictureBuffer.convertTo(hsvFile, cv::COLOR_BGR2HSV);
    CV_Assert(hsvFile.channels() == 3);
    ImgData outputFile = pictureBuffer.clone();
    for (int i = 0; i < hsvFile.rows; ++i) {
      for (int j = 0; j < hsvFile.cols; ++j) {
        PixelValue outValue = resultColors[0];
        PixelValue& content = hsvFile.at<cv::Vec3b>(i, j);
        PixelValue& output = outputFile.at<cv::Vec3b>(i, j);
        for (auto color : resultColors)
        {
          if (colorDistance(color, content) < colorDistance(outValue, content))
          {
            outValue = color;
          }
        }
        for (auto channel = 0; channel < output.channels; ++channel)
        {
          output[channel] = outValue[channel];
        }
      }
    }
    pictureBuffer = outputFile.clone();
    return true;
  }

}
namespace
{
  static const double pi{ std::atan(1) * 4 };

  struct HsvPoint
  {
    const double rc;
    const double pl;
    const double v;

    static HsvPoint fromColor(const PixelValue& in_color)
    {
      return HsvPoint(in_color[0], in_color[1], in_color[2]);
    }

    HsvPoint(int hue, int sat, int val)
      : rc{ 127. + std::cos(pi * hue / 90.) * sat / 2 }
      , pl{ 127. + std::sin(pi * hue / 90.) * sat / 2 }
      , v{ 1. * val }
    {
    }
  };
  double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom)
  {
    HsvPoint p1 = HsvPoint::fromColor(in_colorTo);
    HsvPoint p2 = HsvPoint::fromColor(in_colorFrom);

    return std::sqrt(std::pow(p1.rc - p2.rc, 2) + std::pow(p1.pl - p2.pl, 2) + std::pow(p1.v - p2.v, 2));
  }
}
#if defined(DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN) && (! defined(IGNORE_BASE_CLASS_TESTS))
#include <doctest.h>

class ColorTestPixelator : public imaging::Pixelator
  {
  public:
    bool to_pixels(int, int, int, int, const one_bit::CropRegion) final { return true; }
    ColorTestPixelator() : Pixelator("") { }
    bool hasColor(const PixelValue& in_pixelColor) const {
      for (const auto& color : resultColors)
      {
        if (color == in_pixelColor) return true;
      }
      return false;
    }
    size_t colorCount() const { return resultColors.size(); }
};

class FakeImageBufferTestPixelator : public imaging::Pixelator
{
public:
  bool to_pixels(int, int, int, int, const one_bit::CropRegion) final { return theResult; }
  FakeImageBufferTestPixelator() : Pixelator(""), theResult{ true } { pictureBuffer = ImgData(200, 200, CV_8UC3, cv::Scalar{ 60, 0, 255, 0 }); }
  void setReturnValue(bool returnThis) { theResult = returnThis; }
private:
  bool theResult;
};

TEST_CASE("test pixelator run") {
  ColorTestPixelator emptyPixer;
  CHECK_THROWS(emptyPixer.run("invalid out path", 10, 10, 20, 20, one_bit::CropRegion::TOP_LEFT)); // should fail on writing
  FakeImageBufferTestPixelator pixer;
  CHECK(pixer.run("valid_out_path.png", 10, 10, 20, 20, one_bit::CropRegion::TOP_LEFT)); // should succeed
  pixer.setReturnValue(false);
  CHECK_FALSE(pixer.run("valid_out_path.png", 10, 10, 20, 20, one_bit::CropRegion::TOP_LEFT)); // should fail
  pixer.setReturnValue(true);
  CHECK(pixer.run("valid_out_path.png", 10, 10, 20, 20, one_bit::CropRegion::TOP_LEFT)); // should succeed again
}

TEST_CASE("test pixelator prepare") {
  ColorTestPixelator pixer;
  PixelValue white(0, 0, 255);
  PixelValue black(0, 0, 0);
  // initially no colors set
  CHECK_EQ(pixer.colorCount(), 0);
  CHECK_FALSE(pixer.hasColor(white));
  CHECK_FALSE(pixer.hasColor(black));

  // preparing sets two colors: black and white
  CHECK(pixer.prepare());
  CHECK_EQ(pixer.colorCount(), 2);
  CHECK(pixer.hasColor(white));
  CHECK(pixer.hasColor(black));

  // preparing again doesn't change the color list
  CHECK(pixer.prepare());
  CHECK_EQ(pixer.colorCount(), 2);
  CHECK(pixer.hasColor(white));
  CHECK(pixer.hasColor(black));
}

TEST_CASE("test pixelator to_file") {
  FakeImageBufferTestPixelator pixer;
  CHECK(pixer.to_file("valid_out_path.png")); // should succeed
  CHECK_THROWS(pixer.to_file("invalid out path")); // should throw because can't write there
}

TEST_CASE("test adding color to pixelator") {
  ColorTestPixelator pixer;
  CHECK_EQ(pixer.colorCount(), 0);

  size_t added_colors = 0;
  std::vector<PixelValue> colors{ PixelValue(150, 255, 255), PixelValue(90, 255, 255), PixelValue(30, 255, 255) };
  for (const auto& color : colors)
  {
    // add the new color
    pixer.add_color(color);
    ++added_colors;

    // check that the color list so far is OK
    CHECK_EQ(pixer.colorCount(), added_colors);
    for (size_t idx = 0; idx < colors.size(); ++idx)
    {
      CHECK_EQ(pixer.hasColor(colors[idx]), idx < added_colors);
    }

    // try to re-add the new color, make sure it doesn't change anything
    pixer.add_color(color);
    CHECK_EQ(pixer.colorCount(), added_colors);

    for (size_t idx = 0; idx < colors.size(); ++idx)
    {
      CHECK_EQ(pixer.hasColor(colors[idx]), idx < added_colors);
    }
  }
}

TEST_CASE("test pixelator empty check") {
  ColorTestPixelator emptyPixer;
  CHECK(emptyPixer.empty());
  FakeImageBufferTestPixelator pixerWithBuffer;
  CHECK_FALSE(pixerWithBuffer.empty());
}

TEST_CASE("test pixelation image stats output") {
  ColorTestPixelator emptyPixer;
  CHECK_EQ(emptyPixer.imageStats(), "Image has 0x0 pixels and 1 channels");
  FakeImageBufferTestPixelator pixerWithBuffer;
  CHECK_EQ(pixerWithBuffer.imageStats(), "Image has 200x200 pixels and 3 channels");
}

#include <sstream>

const std::string pixelValueToHexString(const PixelValue& in_color)
{
  std::stringstream formatted{};
  formatted << "h" << std::dec << (int) in_color[0] << "|s" << (int)in_color[1] << "|v" << (int)in_color[2];
  return formatted.str();
}

using idxType = int;

TEST_CASE("test color difference calculus")
{
  const std::vector<std::pair<std::string, PixelValue> > colors{
    {"black",PixelValue(0, 0, 0) },
    {"red", PixelValue(0, 255, 255) },
    {"blue", PixelValue(120, 255, 255) },
    {"green", PixelValue(60, 255, 255) },
    {"yellow", PixelValue(30, 255, 255) },
    {"cyan", PixelValue(90, 255, 255) },
    {"magenta", PixelValue(150, 255, 255) },
    {"white", PixelValue(0, 0, 255) },
  };
  std::map<std::pair<idxType, idxType>, double> distancesByIndex;
  for (idxType first = 0; first < colors.size(); ++first)
  {
    for (idxType second = 0; second <= first; ++second)
    {
      auto [c1name, c1] = colors[first];
      auto [c2name, c2] = colors[second];

      const double pi = std::atan(1) * 4;
      double a1 = pi * c1[0] / 90.;
      double a2 = pi * c2[0] / 90.;
      double h1{ 1. * c1[2] }, h2{ 1. * c2[2] };
      std::vector<double> pos1{ 127. + std::cos(a1) * c1[1] / 2, 127. + std::sin(a1) * c1[1] / 2, h1 }, pos2{ 127. + std::cos(a2) * c2[1] / 2, 127. + std::sin(a2) * c2[1] / 2, h2 };

      double targetDistance{ std::sqrt(std::pow(pos1[0] - pos2[0], 2) + std::pow(pos1[1] - pos2[1], 2) + std::pow(pos1[2] - pos2[2], 2)) };
      distancesByIndex.insert_or_assign(std::make_pair(first, second), targetDistance);
    }
  }
  // colors should have distance 0 to themselves
  for (auto [position, distance] : distancesByIndex)
  {
    auto [c1name, color1] = colors[position.first];
    auto [c2name, color2] = colors[position.second];
    std::cout << "Expecting distance " << distance << " for " << c1name << " (" << pixelValueToHexString(color1) << ") and " << c2name << " (" << pixelValueToHexString(color2) << ")" << std::endl;
    CHECK_LT(std::abs(colorDistance(color1, color2) - distance), 0.01);
  }
}

#endif