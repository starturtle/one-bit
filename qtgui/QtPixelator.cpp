#include "QtPixelator.h"
#include "error_codes.h"
#include "logging.h"
#include "calculus.h"
#include <vector>
#include <set>
#include <optional>
#include <cmath>
#include <algorithm>
#include <QPainter>

namespace
{
  bool hasDuplicates(const std::vector<QColor>& colors);
  bool allValid(const std::vector<QColor>& colors);
  QColor minDiff(const QColor& in_source, const std::vector<QColor>& in_list);
}
 
QtPixelator::QtPixelator(QObject* in_parent)
  : QObject(in_parent) 
  , imageBuffer{}
  , sourcePath{}
  , storagePath{}
  , stitchWidth{0}
  , stitchHeight{0}
  , stitchCount{0}
  , rowCount{0}
  , auxColorPri{QColorConstants::Svg::red}
  , auxColorSec{QColorConstants::Svg::darkgray}
  , helperGrid{5}
  , gridEnabled{true}
{}

errors::Code QtPixelator::run(){
  auto result = checkSettings();
  if (errors::NONE == result)
  {
    QImage colorMap = pixelate();
    if (colorMap.isNull())
    {
      logging::LogStream::instance() << logging::Level::ERR << "Color map is NULL!" << logging::Level::OFF;
      return errors::PIXELATION_ERROR;
    }
    if (!scalePixels(colorMap))
    {
      logging::LogStream::instance() << logging::Level::ERR << "Failed to scale for pixelation" << logging::Level::OFF;
      return errors::PAINT_ERROR;
    }
    drawHelpers();
  }
  else
  {
    logging::LogStream::instance() << logging::Level::ERR << "Failed to verify input: " << result << logging::Level::OFF;
  }
  pixelationCreated();
  return errors::NONE;
}

errors::Code QtPixelator::commit()
{
  if (storagePath.isEmpty())
  {
    logging::LogStream::instance() << logging::Level::ERR << "No output path set!" << logging::Level::OFF;
    return errors::WRONG_OUTPUT_FILE;
  }
  
  if (resultBuffer.save(storagePath.toLocalFile()))
  {
    logging::LogStream::instance() << logging::Level::DEBUG << "File written" << logging::Level::OFF;
    return errors::NONE;
  }

  logging::LogStream::instance() << logging::Level::ERR << "Could not write result" << logging::Level::OFF;
  return errors::WRITE_ERROR;
}

errors::Code QtPixelator::setInputImage(const QImage& in_image)
{
  const std::string input{ in_image.isNull() ? " to NULL" : "" };
  logging::LogStream::instance() << logging::Level::ERR << "Setting input file" << input << logging::Level::OFF;
  imageBuffer = in_image;
  return imageBuffer.isNull() ? errors::WRONG_INPUT_FILE : errors::NONE;
}

errors::Code QtPixelator::setStoragePath(const QUrl& in_url)
{
  logging::LogStream::instance() << logging::Level::DEBUG << "Store to " << in_url.toLocalFile().toStdString() << " on completion." << logging::Level::OFF;
  
  storagePath = in_url;
  return storagePath.isEmpty() ? errors::WRONG_OUTPUT_FILE : errors::NONE;
}

errors::Code QtPixelator::setStitchSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge)
{
  if (in_width <= 0 || in_height <= 0 || in_rowsPerGauge <= 0 || in_stitchesPerGauge <= 0)
  {
    logging::LogStream::instance() << logging::Level::ERR << "Bad input " << in_width << "x" << in_height << "cm with " << in_stitchesPerGauge << "st, " << in_rowsPerGauge << "r per 10x10cm" << logging::Level::OFF;
    return errors::INVALID_IMAGE_SIZES;
  }
  logging::LogStream::instance() << logging::Level::DEBUG << "Result will have " << in_width << "x" << in_height << "cm with " << in_stitchesPerGauge << "st, " << in_rowsPerGauge << "r per 10x10cm" << logging::Level::OFF;
  recomputeSizes(in_width, in_height, in_rowsPerGauge, in_stitchesPerGauge);
  logging::LogStream::instance() << logging::Level::DEBUG << "Result will have " << stitchCount << "st, " << rowCount << "r, stixels will measure " << stitchWidth << "x" << stitchHeight << " each" << logging::Level::OFF;

  return errors::NONE;
}

errors::Code QtPixelator::setStitchColors(const std::vector<QColor> in_colors)
{

  colors = { in_colors };
  if (! allValid(in_colors)) return errors::INVALID_COLOR;
  if (hasDuplicates(in_colors)) return errors::DUPLICATE_COLOR;
  logging::LogStream::instance() << logging::Level::DEBUG << "Set stitch colors" << logging::Level::OFF;
  return errors::NONE;
}

int QtPixelator::setHelperSettings(bool gridEnabled, QColor primaryColor, QColor secondaryColor, unsigned gridCount)
{
  this->gridEnabled = gridEnabled;
  auxColorPri = primaryColor;
  auxColorSec = secondaryColor;
  helperGrid = gridCount;
  return errors::NONE;
}

QImage QtPixelator::resultImage() const
{
  return resultBuffer.copy();
}

void QtPixelator::recomputeSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge)
{
  // gauge is 10cm, so there will be a rectangle totaling a size of in_width*in_stitchesPerGauge/10 x in_height*in_rowsPerGauge/10 stixels,
  // where each stixel will have a width and height to be computed from the IRL size of in_stitchesPerGauge/10cm x in_rowsPerGauge/10cm:
  // the first goal, therefore, must be to figure out how to int-ify "stixel" width and height from that aspect ratio, 
  // i.e. making them fractions of the least common multiple with the same aspect ratio as the actual stitch
  // the total size in stixels is therefore the number of stixels (as computed from the first equation) times the correspoding stixel size.
  stitchCount = (unsigned)(ceil((in_width / 10.) * in_stitchesPerGauge));
  rowCount = (unsigned)(ceil((in_height / 10.) * in_rowsPerGauge));
  unsigned stixelLcm{ calculus::least_common_multiple(in_rowsPerGauge, in_stitchesPerGauge) };
  stitchWidth = stixelLcm / in_stitchesPerGauge;
  stitchHeight = stixelLcm / in_rowsPerGauge;
} 

QImage QtPixelator::pixelate()
{
  QImage colorMap = imageBuffer.scaled(QSize(stitchCount, rowCount));
  for (int y = 0; y < colorMap.height(); y++) {
    QRgb* line = (QRgb*)colorMap.scanLine(y);
    for (int x = 0; x < colorMap.width(); x++) {
      // line[x] has an individual pixel
      auto& colorForPixel{ line[x] };
      auto nearestColor = minDiff(QColor(colorForPixel), colors);
      colorForPixel = nearestColor.isValid() ? nearestColor.rgb() : Qt::black;
    }
    logging::LogStream::instance() << logging::Level::DEBUG << "x";
  }
  logging::LogStream::instance() << logging::Level::OFF;
  logging::LogStream::instance() << logging::Level::DEBUG << "Get pixelation template of size " << colorMap.width() << "x" << colorMap.height() << logging::Level::OFF;
  return colorMap;
}

bool QtPixelator::scalePixels(const QImage& colorMap)
{
  if (! ((colorMap.width() == stitchCount) && (colorMap.height() == rowCount))) return false;
  resultBuffer = imageBuffer.scaled(QSize(stitchCount * stitchWidth, rowCount * stitchHeight));
  QPainter qPainter(&resultBuffer);
  for (int y = 0; y < colorMap.height(); y++) {
    QRgb* line = (QRgb*)colorMap.scanLine(y);
    for (int x = 0; x < colorMap.width(); x++) {
      QColor stixelColor{ line[x] };
      qPainter.setPen(gridEnabled ? auxColorSec : stixelColor);
      qPainter.setBrush(stixelColor);
      qPainter.drawRect(x * stitchWidth, y * stitchHeight, stitchWidth, stitchHeight);
      logging::LogStream::instance() << logging::Level::DEBUG << "Draw sec from " << x << "/" << y << " with dimensions " << stitchWidth << "x" << stitchHeight << logging::Level::OFF;
    }
    logging::LogStream::instance() << logging::Level::DEBUG << "x";
  }
  logging::LogStream::instance() << logging::Level::DEBUG << logging::Level::OFF;
  qPainter.end();
  logging::LogStream::instance() << logging::Level::DEBUG << "Pixelation complete" << logging::Level::OFF;
  return true;
}

void QtPixelator::drawHelpers()
{
  if(!gridEnabled)
  {
    return;
  }
  QPainter qPainter(&resultBuffer);
  qPainter.setPen(auxColorPri);
  unsigned primaryGridWidth = helperGrid * stitchWidth;
  unsigned primaryGridHeight = helperGrid * stitchHeight;
  if (resultBuffer.width() < 0 || resultBuffer.height() < 0)
  {
    throw std::runtime_error("trying to draw helper lines on picture with negative dimensions!");
  }
  for (unsigned x = 0; x < (unsigned)resultBuffer.width(); x += primaryGridWidth)
  {
    for (unsigned y = 0; y < (unsigned)resultBuffer.height(); y += primaryGridHeight)
    {
      qPainter.drawRect(x, y, primaryGridWidth, primaryGridHeight);
      logging::LogStream::instance() << logging::Level::DEBUG << "Draw pri from " << x << "/" << y << " with dimensions " << primaryGridWidth << "x" << primaryGridHeight << logging::Level::OFF;
    }
  }
  qPainter.drawLine(0, resultBuffer.height() - 1, resultBuffer.width() - 1, resultBuffer.height() - 1);
  qPainter.drawLine(resultBuffer.width() - 1, 0, resultBuffer.width() - 1, resultBuffer.height() - 1);
  qPainter.end();
}

errors::Code QtPixelator::checkSettings()
{
  if (imageBuffer.isNull())
  {
    return errors::WRONG_INPUT_FILE;
  }
  if (stitchWidth <= 0 || stitchHeight <= 0 || stitchCount <= 0 || rowCount <= 0)
  {
    return errors::INVALID_IMAGE_SIZES;
  }
  return errors::NONE;
}

namespace
{
  bool hasDuplicates(const std::vector<QColor>& colors)
  {
    for (auto& color1 = colors.begin(); color1 != colors.end(); ++color1)
    {
      auto nextColor = color1;
      ++nextColor;
      if (std::find(nextColor, colors.end(), *color1) != colors.end()) return true;
    }
    return false;
  }

  bool allValid(const std::vector<QColor>& colors)
  {
    return std::any_of(colors.begin(), colors.end(), [](const QColor& color) {return color.isValid(); });
  }
  static const double pi{ std::atan(1) * 4 };

  struct HsvPoint
  {
    const double rc;
    const double pl;
    const double v;

    static HsvPoint fromColor(const QColor& in_color)
    {
      int hue, sat, val;
      in_color.getHsl(&hue, &sat, &val);
      return HsvPoint(hue, sat, val);
    }

    HsvPoint(int hue, int sat, int val)
      : rc{ 127. + std::cos(pi * hue / 180.) * sat / 2 }
      , pl{ 127. + std::sin(pi * hue / 180.) * sat / 2 }
      , v{ 1. * val }
    { 
    }
  };

  double colorDistance(const QColor& one, const QColor& other)
  {
    HsvPoint p1 = HsvPoint::fromColor(one);
    HsvPoint p2 = HsvPoint::fromColor(other);

    return std::sqrt(std::pow(p1.rc - p2.rc, 2) + std::pow(p1.pl - p2.pl, 2) + std::pow(p1.v - p2.v, 2));
  }

  QColor minDiff(const QColor& in_source, const std::vector<QColor>& in_list)
  {
    double diff = std::numeric_limits<double>::max();
    QColor returnValue{};
    for (auto color : in_list)
    {
      double currDiff = colorDistance(in_source, color);
      if (currDiff < diff)
      {
        returnValue = color;
        diff = currDiff;
      }
    }
    return returnValue;
  }
}
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iomanip>
#include <sstream>

TEST_CASE("test hasDuplicates") {
  CHECK(hasDuplicates({ QColorConstants::Svg::red, QColorConstants::Svg::red }));

  CHECK(! hasDuplicates({ QColorConstants::Svg::red, QColorConstants::Svg::blue }));

  std::vector<QColor>colors = { QColorConstants::Svg::red, QColorConstants::Svg::blue, QColorConstants::Svg::magenta };
  CHECK(!hasDuplicates(colors));

  colors.push_back(QColorConstants::Svg::limegreen);
  CHECK(!hasDuplicates(colors));

  colors[1] = QColorConstants::Svg::limegreen;
  CHECK(hasDuplicates(colors));

  colors[3] = QColorConstants::Svg::red;
  CHECK(hasDuplicates(colors));
}

const std::string qcolorToHexString(const QColor& in_color)
{
  std::stringstream formatted{};
  formatted << "#" << std::setfill('0') << std::setw(2) << std::hex << in_color.red() << std::setw(2) << in_color.green() << std::setw(2) << in_color.blue();
  formatted << ", as HSV: " << std::dec << in_color.hsvHue() << "/" << in_color.hsvSaturation() << "/" << in_color.value();
  return formatted.str();
}

using idxType = int;

TEST_CASE("test color difference calculus")
{
  const std::vector<std::pair<std::string, QColor> > colors { 
    {"red", QColorConstants::Svg::red}, 
    {"blue", QColorConstants::Svg::blue}, 
    {"green", QColorConstants::Svg::green}, 
    {"white", QColorConstants::Svg::white}, 
    {"yellow", QColorConstants::Svg::yellow}, 
    {"black", QColorConstants::Svg::black },
    {"cyan", QColorConstants::Svg::cyan },
    {"magenta", QColorConstants::Svg::magenta },
  };
  std::map<std::pair<idxType, idxType>, double> distancesByIndex;
  for (idxType first = 0; first < colors.size(); ++first)
  {
    for (idxType second = 0; second <= first; ++second)
    {
      auto [c1name, c1] = colors[first];
      auto [c2name, c2] = colors[second];
      int c1h, c1s, c1v;
      int c2h, c2s, c2v;
      c1.getHsl(&c1h, &c1s, &c1v);
      c2.getHsl(&c2h, &c2s, &c2v);

      const double pi = std::atan(1) * 4;
      double a1 = pi * c1h / 180.;
      double a2 = pi * c2h / 180.;
      double h1{ 1. * c1v }, h2{ 1. * c2v };
      std::vector<double> pos1{ 127. + std::cos(a1) * c1s / 2, 127. + std::sin(a1) * c1s / 2, h1 }, pos2{ 127. + std::cos(a2) * c2s / 2, 127. + std::sin(a2) * c2s / 2, h2 };

      double targetDistance{ std::sqrt(std::pow(pos1[0] - pos2[0], 2) + std::pow(pos1[1] - pos2[1], 2) + std::pow(pos1[2] - pos2[2], 2)) };
      distancesByIndex.insert_or_assign(std::make_pair(first, second), targetDistance);
    }
  }
  // colors should have distance 0 to themselves
  for (auto [position, distance] : distancesByIndex)
  {
    auto [c1name, color1] = colors[position.first];
    auto [c2name, color2] = colors[position.second];
    std::cout << "Expecting distance " << distance << " for " << c1name << " (" << qcolorToHexString(color1) << ") and " << c2name << " (" << qcolorToHexString(color2) << ")" << std::endl;
    CHECK_EQ(colorDistance(color1, color2), distance);
  }
}

#include <tuple>

TEST_CASE("test minimum difference finder")
{
  const std::vector<QColor> colors{ QColorConstants::Svg::red, QColorConstants::Svg::blue, QColorConstants::Svg::green, QColorConstants::Svg::white, QColorConstants::Svg::yellow, QColorConstants::Svg::magenta };
  for (auto color : colors)
  {
    std::cout << qcolorToHexString(color) << std::endl;
  }

  const std::vector < std::tuple<std::string, QColor, QColor> > testData{
    {"orange", QColorConstants::Svg::orange, QColorConstants::Svg::yellow},
    {"orangered", QColorConstants::Svg::orangered, QColorConstants::Svg::red},
    {"white", QColorConstants::Svg::white, QColorConstants::Svg::white},
    {"purple", QColorConstants::Svg::purple, QColorConstants::Svg::magenta},
    {"aqua", QColorConstants::Svg::aqua, QColorConstants::Svg::blue},
    {"black", QColorConstants::Svg::black, QColorConstants::Svg::green},
  };

  for (auto testSet : testData)
  {
    auto [name, in_color, out_color] = testSet;
    auto targetColor = minDiff(in_color, colors);
    std::cout << "Target color " << name << " " << qcolorToHexString(in_color) << std::endl;
    std::cout << "maps to " << qcolorToHexString(targetColor) << std::endl;
    CHECK_EQ(targetColor, out_color);
  }
}
#endif