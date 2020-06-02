#include "FloodFillPixelator.h"

#include <string>
#include <unordered_map>

namespace imaging
{
  namespace
  {
    int pixelToInt(const PixelValue& pixel);
    PixelValue intToPixel(int pixelAsInt);
  }

FloodFillPixelator::FloodFillPixelator(const std::string& in_path) : Pixelator(in_path) {}

bool FloodFillPixelator::to_pixels(int rows, int columns)
{
  return binarize() && pixelate(rows, columns);
}

bool FloodFillPixelator::pixelate(int rows, int columns)
{
  ImgData outputFile = pictureBuffer.clone();
  int cellWidth = std::max(pictureBuffer.cols / columns, 0) + 1;
  int cellHeight = std::max(pictureBuffer.rows / rows, 0) + 1;
  for (int r = 0; r < pictureBuffer.rows; r += cellHeight)
  {
    for (int c = 0; c < pictureBuffer.cols; c += cellWidth)
    {
      const int iMax = std::min(r + cellHeight, pictureBuffer.rows);
      const int jMax = std::min(c + cellWidth, pictureBuffer.cols);

      PixelValue targetColor = getMostCommonColor(r, c, iMax, jMax);
      for (int i = r; i < iMax; ++i)
      {
        for (int j = c; j < jMax; ++j)
        {
          PixelValue& targetPixel = outputFile.at<PixelValue>(i, j);
          for (auto channel = 0; channel < targetPixel.channels; ++channel)
          {
            targetPixel[channel] = targetColor[channel];
          }
        }
      }
    }
  }
  pictureBuffer = outputFile.clone();
  return true;
}

PixelValue FloodFillPixelator::getMostCommonColor(int start_row, int start_column, int end_row, int end_column)
{
  std::unordered_map<int, int> counters;
  for (int i = start_row; i < end_row; ++i)
  {
    for (int j = start_column; j < end_column; ++j)
    {
      PixelValue color = pictureBuffer.at<PixelValue>(i, j);
      int colorAsInt = pixelToInt(color);
      if (counters.find(colorAsInt) == counters.end())
      {
        counters.emplace(colorAsInt, 0);
      }
      counters[colorAsInt]++;
    }
  }
  auto mostCommonColor = counters.begin()->first;
  for (auto colorCounter : counters)
  {
    if (colorCounter.second > counters[mostCommonColor])
    {
      mostCommonColor = colorCounter.first;
    }
  }
  return intToPixel(mostCommonColor);
}

namespace
{
  int pixelToInt(const PixelValue& pixel)
  {
    int pixelInt = 0;
    for (auto c = 0; c < pixel.channels; ++c)
    {
      pixelInt += pixel[c];
      pixelInt <<= 8;
    }
    return pixelInt;
  }

  PixelValue intToPixel(int pixelAsInt)
  {
    PixelValue pixel;
    for (auto c = pixel.channels - 1; c >= 0; --c)
    {
      pixelAsInt >>= 8;
      pixel[c] = pixelAsInt % 256;
    }
    return pixel;
  }
}
}