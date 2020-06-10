#include "QtPixelator.h"
#include "error_codes.h"
#include "logging.h"
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>
#include <QPainter>

namespace
{
  bool hasDuplicates(const std::vector<QColor>& colors);
  bool allValid(const std::vector<QColor>& colors);
  unsigned greatest_common_divisor(unsigned a, unsigned b);
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
{}

errors::Code QtPixelator::run(){
  auto result = checkSettings();
  if (errors::NONE == result)
  {
    QImage colorMap = pixelate();
    if (colorMap.isNull()) return errors::PIXELATION_ERROR;
    if (! scalePixels(colorMap)) return errors::PAINT_ERROR;
    if (! imageBuffer.save(storagePath.toLocalFile())) result = errors::WRITE_ERROR;
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "File written" << std::endl;
  }
  return result;
}
errors::Code QtPixelator::setInputImage(const QUrl& in_url)
{
  sourcePath = in_url;
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Load from " << in_url.toLocalFile().toStdString() << " on completion." << std::endl;
  return (imageBuffer.load(in_url.toLocalFile()) ? errors::NONE : errors::WRONG_INPUT_FILE);
}

errors::Code QtPixelator::setOutputImage(const QUrl& in_url)
{
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Store to " << in_url.toLocalFile().toStdString() << " on completion." << std::endl;
  if ((sourcePath == in_url) && !(!sourcePath.isEmpty()))
  {
    // TODO: show confirm dialog "are you sure you want to overwrite?"
  }
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

const QImage& QtPixelator::result() const
{
  return imageBuffer;
}

void QtPixelator::recomputeSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge)
{
  // gauge is 10cm, so there will be a rectangle totaling a size of in_width*in_stitchesPerGauge/10 x in_height*in_rowsPerGauge/10 stixels,
  // where each stixel will have a width and height to be computed from the IRL size of in_stitchesPerGauge/10cm x in_rowsPerGauge/10cm:
  // the first goal, therefore, must be to figure out how to int-ify "stixel" width and height from that aspect ratio, 
  // i.e. making them fractions of the least common multiple with the same aspect ratio as the actual stitch
  // the total size in stixels is therefore the number of stixels (as computed from the first equation) times the correspoding stixel size.
  unsigned stixelGcd{ greatest_common_divisor(in_rowsPerGauge, in_stitchesPerGauge) };
  stitchWidth = in_stitchesPerGauge / stixelGcd;
  stitchHeight = in_rowsPerGauge / stixelGcd;
  stitchCount = (unsigned)(ceil((in_width / 10.) * in_stitchesPerGauge));
  rowCount = (unsigned)(ceil((in_height / 10.) * in_rowsPerGauge));
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
  imageBuffer = imageBuffer.scaled(QSize(stitchCount * stitchWidth, rowCount * stitchHeight));
  QPainter qPainter(&imageBuffer);
  for (int y = 0; y < colorMap.height(); y++) {
    QRgb* line = (QRgb*)colorMap.scanLine(y);
    for (int x = 0; x < colorMap.width(); x++) {
      QColor stixelColor{ line[x] };
      qPainter.setPen(stixelColor);
      qPainter.setBrush(stixelColor);
      qPainter.drawRect(x * stitchWidth, y * stitchHeight, stitchWidth, stitchHeight);
    }
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "x";
  }
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << std::endl;
  qPainter.end();
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Pixelation complete"<< std::endl;
  return true;
}

errors::Code QtPixelator::checkSettings()
{
  if (imageBuffer.isNull())
  {
    return errors::WRONG_INPUT_FILE;
  }
  if (storagePath.isEmpty())
  {
    return errors::WRONG_OUTPUT_FILE;
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
      auto& nextColor = color1;
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
    unsigned result = number / divisor;
    if (result * divisor == number)
    {
      return result;
    }
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
      if (std::find(divisors_larger.begin(), divisors_larger.end(), divisor) != divisors_larger.end())
      {
        return divisor;
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

  double colorDistance(const QColor& one, const QColor& other)
  {
    uchar hueDiff = abs(one.hsvHue() - other.hsvHue());
    uchar saturationDiff = abs(one.hsvSaturation() - other.hsvSaturation());
    uchar valueDiff = abs(one.value() - other.value());
    return hueDiff + 10. * (saturationDiff + 10. * valueDiff);
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