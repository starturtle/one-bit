#include "imaging.h"

namespace imaging
{
	namespace
	{
		double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom);
	}

	ImgData load(const std::string& in_path)
	{
		FILE* file = fopen(in_path.c_str(), "r");
		if (nullptr != file)
		{
			fclose(file);
			return cv::imread(in_path, cv::ImreadModes::IMREAD_COLOR);
		}
		std::cout << "Could not open file [" << in_path << "], doesn't exist or not readable!" << std::endl;
		return ImgData();
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
				output[0] = outValue[0];
				output[1] = outValue[1];
				output[2] = outValue[2];
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
	}
}