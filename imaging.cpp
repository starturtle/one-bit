#include "imaging.h"

namespace imaging
{
	namespace
	{
		double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom);
	}

	ImgData load(const std::string& in_path)
	{
		return cv::imread(in_path, cv::IMREAD_COLOR);
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
		ImgData hsvFile;
		cvtColor(in_image, hsvFile, cv::COLOR_BGR2HSV);
		CV_Assert(hsvFile.channels() == 3);
		for (int i = 0; i < hsvFile.rows; ++i) {
			for (int j = 0; j < hsvFile.cols; ++j) {
				PixelValue outValue = in_resultColors[0];
				PixelValue content = hsvFile.at<cv::Vec3b>(j, i);
				for (auto color : in_resultColors)
				{
					if (colorDistance(color, content) < colorDistance(outValue, content))
					{
						outValue = color;
					}
				}
				content = outValue;
			}
		}
		ImgData outputFile;
		cvtColor(hsvFile, outputFile, cv::COLOR_HSV2BGR);
		return outputFile;
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
	}
}