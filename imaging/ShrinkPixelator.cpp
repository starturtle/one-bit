#include "ShrinkPixelator.h"

namespace imaging
{
	ShrinkPixelator::ShrinkPixelator(const std::string& in_path) : Pixelator{ in_path } {}

	bool ShrinkPixelator::to_pixels(int rows, int columns)
	{
		ImgData buffer;
		int rows_backup = pictureBuffer.rows;
		int columns_backup = pictureBuffer.cols;
		cv::resize(pictureBuffer, buffer, cv::Size(columns, rows));
		cv::swap(pictureBuffer, buffer);
		binarize();
		cv::swap(pictureBuffer, buffer);
		cv::resize(buffer, pictureBuffer, cv::Size(columns_backup, rows_backup), 0.0, 0.0, cv::InterpolationFlags::INTER_LINEAR_EXACT);
		return true;
	}
}