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
		binarize();
		cv::resize(buffer, pictureBuffer, cv::Size(columns_backup, rows_backup));
		return false;
	}
}