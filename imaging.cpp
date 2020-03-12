#include "imaging.h"

#include <algorithm>
#include <unordered_map>

namespace imaging
{
	namespace
	{
		double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom);
		PixelValue getMostCommonColor(const ImgData& in_image, int start_row, int start_column, int end_row, int end_column);
		int pixelToInt(const PixelValue& pixel);
		PixelValue intToPixel(int pixelAsInt);
	}

	ImgData load(const std::string& in_path)
	{
		return cv::imread(in_path, cv::ImreadModes::IMREAD_COLOR);
	}
	bool store(const std::string& out_path, ImgData in_data)
	{
		std::vector<int> compression_params;
		compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);
		return cv::imwrite(out_path, in_data, compression_params);
	}

	ImgData binarize(const ImgData& in_image, std::vector<PixelValue> in_resultColors)
	{
		ImgData hsvFile(in_image.cols, in_image.rows, in_image.type());
		in_image.convertTo(hsvFile, cv::COLOR_BGR2HSV);
		CV_Assert(hsvFile.channels() == 3);
		ImgData outputFile = in_image.clone();
		for (int i = 0; i < hsvFile.rows; ++i) {
			for (int j = 0; j < hsvFile.cols; ++j) {
				PixelValue outValue = in_resultColors[0];
				PixelValue& content = hsvFile.at<cv::Vec3b>(i, j);
				PixelValue& output = outputFile.at<cv::Vec3b>(i, j);
				for (auto color : in_resultColors)
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
		return outputFile.clone();
	}

	ImgData pixelate(const ImgData& in_image, int rows, int columns)
	{
		ImgData outputFile = in_image.clone();
		int cellWidth = std::max(in_image.cols / columns, 0) + 1;
		int cellHeight = std::max(in_image.rows / rows, 0) + 1;
		for (int r = 0; r < in_image.rows; r += cellHeight)
		{
			for (int c = 0; c < in_image.cols; c += cellWidth)
			{
				const int iMax = std::min(r + cellHeight, in_image.rows);
				const int jMax = std::min(c + cellWidth, in_image.cols);
				
				PixelValue targetColor = getMostCommonColor(in_image, r, c, iMax, jMax);
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
		return outputFile.clone();
	}

	namespace
	{
		double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom)
		{
			uchar hueDiff = abs(in_colorTo[0] - in_colorFrom[0]);
			uchar saturationDiff = abs(in_colorTo[1] - in_colorFrom[1]);
			uchar valueDiff = abs(in_colorTo[2] - in_colorFrom[2]);
			return hueDiff + 10. * (saturationDiff + 10. * valueDiff);
		}

		PixelValue getMostCommonColor(const ImgData& in_image, int start_row, int start_column, int end_row, int end_column)
		{
			std::unordered_map<int, int> counters;
			for (int i = start_row; i < end_row; ++i)
			{
				for (int j = start_column; j < end_column; ++j)
				{
					PixelValue color = in_image.at<PixelValue>(i, j);
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