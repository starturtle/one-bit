#pragma once
#include <opencv2/opencv.hpp>
#include <string>

using ImgData=cv::Mat;
using PixelValue=cv::Vec3b;

namespace imaging
{
	class Pixelator
	{
	public:
		Pixelator(const std::string& in_path);
		bool to_file(const std::string& in_path) const;
		void add_color(PixelValue in_color);
		bool empty() const;
		int imgWidth() const;
		int imgHeight() const;
		virtual bool to_pixels(int rows, int columns) = 0;
		virtual std::string imageStats() const;
	protected:
		ImgData pictureBuffer;
		std::vector<PixelValue> resultColors;
	};

}