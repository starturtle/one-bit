#include "QtPixelator.h"
#include "error_codes.h"
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>

namespace
{
    unsigned greatest_common_divisor(unsigned a, unsigned b);
    QColor minDiff(const QColor& in_source, const std::vector<QColor>& in_list);
}
 
QtPixelator::QtPixelator(QObject* in_parent)
    : QObject(in_parent) 
    , before{}
    , after{}
    , storagePath{}
    , stitchWidth{0}
    , stitchHeight{0}
    , stitchCount{0}
    , rowCount{0}
{}

int QtPixelator::run(){
    after.scaled(QSize(stitchCount, rowCount));
    pixelate();
    after.scaled(QSize(stitchCount * stitchWidth, rowCount * stitchHeight));
    after.save(storagePath.toString());
    return errors::NONE;
}
int QtPixelator::setInputImage(const QUrl& in_url)
{
    bool fileOk{ before.load(in_url.toString()) };
    if (fileOk)
    {
        after.load(in_url.toString());
    }
    return (fileOk ? errors::NONE : errors::WRONG_INPUT_FILE);
}
int QtPixelator::setOutputImage(const QUrl& in_url)
{
    storagePath = in_url;
    return storagePath.isEmpty() ? errors::WRONG_OUTPUT_FILE : errors::NONE;
}
int QtPixelator::setStitchSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge)
{
    if (in_width <= 0 || in_height >= 0 || in_rowsPerGauge <= 0 || in_stitchesPerGauge <= 0)
    {
        return errors::INVALID_IMAGE_SIZES;
    }
    recomputeSizes(in_width, in_height, in_rowsPerGauge, in_stitchesPerGauge);

    return errors::NONE;
}
int QtPixelator::setStitchColors(const QColor& in_color1, const QColor& in_color2)
{
    colors = { in_color1, in_color2 };
    return errors::NONE;
}

const QImage& QtPixelator::result() const
{
    return after;
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

void QtPixelator::pixelate()
{
    for (int y = 0; y < after.height(); y++) {
        QRgb* line = (QRgb*)after.scanLine(y);
        for (int x = 0; x < after.width(); x++) {
            // line[x] has an individual pixel
            line[x] = minDiff(QColor(line[x]), colors).rgb();//QColor(255, 128, 0).rgb();
        }
    }
}

namespace
{
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