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
		virtual bool run(const std::string& in_writePath, const int rows, const int columns);
		bool prepare();
		bool to_file(const std::string& in_path) const;
		void add_color(PixelValue in_color);
		bool empty() const;
		virtual bool to_pixels(int in_rows, int in_columns) = 0;
		virtual std::string imageStats() const;
		virtual bool display(const std::string& in_imageName);

	protected:
		bool binarize();

		ImgData pictureBuffer;
		std::vector<PixelValue> resultColors;
	};

}