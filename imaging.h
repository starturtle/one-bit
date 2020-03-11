#pragma once

#include <opencv2/opencv.hpp>
#include <string>

using ImgData=cv::Mat;
using PixelValue=cv::Vec3b;

namespace imaging
{
	ImgData load(const std::string& in_path);
	bool store(const std::string& in_path, ImgData in_data);
	ImgData binarize(const ImgData& in_image, std::vector<PixelValue> in_resultColors);
}