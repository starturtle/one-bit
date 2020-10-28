#include "QtPixelator.h"
#include "error_codes.h"
#include "logging.h"
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
  unsigned greatest_common_divisor(unsigned a, unsigned b);
  unsigned least_common_multiple(unsigned a, unsigned b);
  unsigned least_common_multiple(const std::vector<unsigned>& values);
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
      logging::LogStream::instance().getLogStream(logging::Level::ERROR) << "Color map is NULL!" << std::endl;
      return errors::PIXELATION_ERROR;
    }
    if (!scalePixels(colorMap))
    {
      logging::LogStream::instance().getLogStream(logging::Level::ERROR) << "Failed to scale for pixelation" << std::endl;
      return errors::PAINT_ERROR;
    }
    drawHelpers();
  }
  else
  {
    logging::LogStream::instance().getLogStream(logging::Level::ERROR) << "Failed to verify input: " << result << std::endl;
  }
  pixelationCreated();
  return errors::NONE;
}

errors::Code QtPixelator::commit()
{
  if (storagePath.isEmpty())
  {
    logging::LogStream::instance().getLogStream(logging::Level::ERROR) << "No output path set!" << std::endl;
    return errors::WRONG_OUTPUT_FILE;
  }
  
  if (resultBuffer.save(storagePath.toLocalFile()))
  {
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "File written" << std::endl;
    return errors::NONE;
  }

  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Could not write result" << std::endl;
  return errors::WRITE_ERROR;
}

errors::Code QtPixelator::setInputImage(const QImage& in_image)
{
  const std::string input{ in_image.isNull() ? " to NULL" : "" };
  logging::LogStream::instance().getLogStream(logging::Level::ERROR) << "Setting input file" << input << std::endl;
  imageBuffer = in_image;
  return imageBuffer.isNull() ? errors::WRONG_INPUT_FILE : errors::NONE;
}

errors::Code QtPixelator::setStoragePath(const QUrl& in_url)
{
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Store to " << in_url.toLocalFile().toStdString() << " on completion." << std::endl;
  
  storagePath = in_url;
  return storagePath.isEmpty() ? errors::WRONG_OUTPUT_FILE : errors::NONE;
}

errors::Code QtPixelator::setStitchSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge)
{
  if (in_width <= 0 || in_height <= 0 || in_rowsPerGauge <= 0 || in_stitchesPerGauge <= 0)
  {
    logging::LogStream::instance().getLogStream(logging::Level::ERROR) << "Bad input " << in_width << "x" << in_height << "cm with " << in_stitchesPerGauge << "st, " << in_rowsPerGauge << "r per 10x10cm" << std::endl;
    return errors::INVALID_IMAGE_SIZES;
  }
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Result will have " << in_width << "x" << in_height << "cm with " << in_stitchesPerGauge << "st, " << in_rowsPerGauge << "r per 10x10cm" << std::endl;
  recomputeSizes(in_width, in_height, in_rowsPerGauge, in_stitchesPerGauge);
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Result will have " << stitchCount << "st, " << rowCount << "r, stixels will measure " << stitchWidth << "x" << stitchHeight << " each" << std::endl;

  return errors::NONE;
}

errors::Code QtPixelator::setStitchColors(const std::vector<QColor> in_colors)
{

  colors = { in_colors };
  if (! allValid(in_colors)) return errors::INVALID_COLOR;
  if (hasDuplicates(in_colors)) return errors::DUPLICATE_COLOR;
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Set stitch colors" << std::endl;
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
  unsigned stixelLcm{ least_common_multiple(in_rowsPerGauge, in_stitchesPerGauge) };
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
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "x";
  }
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << std::endl;
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Get pixelation template of size " << colorMap.width() << "x" << colorMap.height() << std::endl;
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
      logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Draw sec from " << x << "/" << y << " with dimensions " << stitchWidth << "x" << stitchHeight << std::endl;
    }
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "x";
  }
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << std::endl;
  qPainter.end();
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Pixelation complete"<< std::endl;
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
      logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Draw pri from " << x << "/" << y << " with dimensions " << primaryGridWidth << "x" << primaryGridHeight << std::endl;
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

  std::optional<unsigned> divides(unsigned number, unsigned divisor)
  {
    if (number == 0 || divisor == 0)
    {
      return 0;
    }
    unsigned result = number / divisor;
    if (result * divisor == number)
    {
      return result;
    }
    return std::optional<unsigned>{};
  }
  
  std::set<unsigned> divisors(unsigned x)
  {
    std::set<unsigned> divisors;
    for (unsigned factor = 1; factor <= sqrt(x); ++factor)
    {
      auto result = divides(x, factor);
      if (result.has_value())
      {
        divisors.insert(factor);
        divisors.insert(result.value());
      }
    }
    return divisors;
  }

  unsigned search_greatest_common_divisor(unsigned smallerValue, unsigned largerValue)
  {
    auto divisors_smaller = divisors(smallerValue);
    auto divisors_larger = divisors(largerValue);

    if (divisors_smaller.empty())
    {
      return 0;
    }

    for (auto divisor = divisors_smaller.rbegin(); divisor != divisors_smaller.rend(); ++divisor)
    {
      if (std::find(divisors_larger.begin(), divisors_larger.end(), *divisor) != divisors_larger.end())
      {
        return *divisor;
      }
    }
    return 1;
  }

  unsigned greatest_common_divisor(unsigned a, unsigned b)
  {
    if (a == b)
    {
      return a;
    }
    if (a < b)
    {
      if (divides(b, a).has_value())
      {
        return a;
      }
      return search_greatest_common_divisor(a, b);
    }
    else
    {
      if (divides(a, b).has_value())
      {
        return b;
      }
      return search_greatest_common_divisor(b, a);
    }
  }

  unsigned least_common_multiple(unsigned a, unsigned b)
  {
    if (a == 0 || b == 0)
    {
      return 0;
    }
    if (divides(b, a).has_value())
    {
      return b;
    }
    if (divides(a, b).has_value())
    {
      return a;
    }
    unsigned gcd = greatest_common_divisor(a, b);
    return a * b / gcd;//gcd * (a / gcd) * (b / gcd);
  }

  unsigned least_common_multiple(const std::vector<unsigned>& values)
  {
    if (values.size() < 3)
    {
      // break condition for recursion
      if (values.empty())
      {
        return 0;
      }
      if (1 == values.size())
      {
        return values[0];
      }
      return least_common_multiple(values[0], values[1]);
    }

    // recursive call
    std::vector<unsigned> next;
    unsigned oneValue = 0;
    for (auto i = 0; i < values.size()/2; ++i)
    {
      next.push_back(least_common_multiple(values[2*i], values[2*i + 1]));
    }
    if (values.size() % 2 > 0)
    {
      next.push_back(values.back());
    }
    return least_common_multiple(next);
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

TEST_CASE("test divides") {
  auto result = divides(25, 3);
  CHECK(! result.has_value());

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
  CHECK(! result.has_value());
}

TEST_CASE("test divisors") {
  std::set<unsigned> result;
  
  result = divisors(5);
  CHECK_EQ(result.size(), 2);
  CHECK_NE(result.find(1), result.end());
  CHECK_NE(result.find(5), result.end());

  result = divisors(16);
  CHECK_EQ(result.size(), 5);
  CHECK_NE(result.find(1), result.end());
  CHECK_NE(result.find(2), result.end());
  CHECK_NE(result.find(4), result.end());
  CHECK_NE(result.find(8), result.end());
  CHECK_NE(result.find(16), result.end());

  result = divisors(24);
CHECK_EQ(result.size(), 8);
CHECK_NE(result.find(1), result.end());
CHECK_NE(result.find(2), result.end());
CHECK_NE(result.find(3), result.end());
CHECK_NE(result.find(4), result.end());
CHECK_NE(result.find(6), result.end());
CHECK_NE(result.find(8), result.end());
CHECK_NE(result.find(12), result.end());
CHECK_NE(result.find(24), result.end());

result = divisors(36);
CHECK_EQ(result.size(), 9);
CHECK_NE(result.find(1), result.end());
CHECK_NE(result.find(2), result.end());
CHECK_NE(result.find(3), result.end());
CHECK_NE(result.find(4), result.end());
CHECK_NE(result.find(6), result.end());
CHECK_NE(result.find(9), result.end());
CHECK_NE(result.find(12), result.end());
CHECK_NE(result.find(18), result.end());
CHECK_NE(result.find(36), result.end());

result = divisors(8192); // 2^13
CHECK_EQ(result.size(), 14);
unsigned factorInlist = 1;
for (auto i = 0; i < 14; ++i)
{
  CHECK_NE(result.find(factorInlist), result.end());
  factorInlist *= 2;
}

result = divisors(262144); // 2^18
CHECK_EQ(result.size(), 19);
factorInlist = 1;
for (auto i = 0; i < 19; ++i)
{
  CHECK_NE(result.find(factorInlist), result.end());
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

TEST_CASE("test vector-based least_common_multiple")
{
  CHECK_EQ(least_common_multiple({ 16, 8 }), 16);
  CHECK_EQ(least_common_multiple({ 8, 16 }), 16);
  CHECK_EQ(least_common_multiple({ 12, 16, 18 }), 144);
  CHECK_EQ(least_common_multiple({ 1, 15 }), 15);
  CHECK_EQ(least_common_multiple({ 36, 24, 9 }), 72);
  CHECK_EQ(least_common_multiple({ 243, 1024 }), 243 * 1024); // don't share factors
  CHECK_EQ(least_common_multiple({ 125, 243, 1024 }), 125 * 243 * 1024); // don't share factors

  CHECK_EQ(least_common_multiple({ 0, 18, 9 }), 0);
  CHECK_EQ(least_common_multiple({ 18, 0 }), 0);
  CHECK_EQ(least_common_multiple({ 0, 0 }), 0);
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