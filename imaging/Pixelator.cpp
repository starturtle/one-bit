#include "Pixelator.h"
namespace imaging
{

	Pixelator::Pixelator(const std::string& in_path)
		: pictureBuffer{ cv::imread(in_path, cv::ImreadModes::IMREAD_COLOR) }
		, resultColors{}
	{

	}

	bool Pixelator::to_file(const std::string& in_path) const
	{
		std::vector<int> compression_params;
		compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);
		return cv::imwrite(in_path, pictureBuffer, compression_params);
	}

	void Pixelator::add_color(PixelValue in_color)
	{
		if (std::find(resultColors.begin(), resultColors.end(), in_color) == resultColors.end())
		{
			resultColors.push_back(in_color);
		}
	}
	bool Pixelator::empty() const
	{
		return pictureBuffer.empty();
	}
	int Pixelator::imgWidth() const
	{
		return pictureBuffer.cols;
	}
	int Pixelator::imgHeight() const
	{
		return pictureBuffer.rows;
	}
	std::string Pixelator::imageStats() const
	{
		std::stringstream outputStream;
		outputStream << "Image has " << pictureBuffer.rows << "x" << pictureBuffer.cols << " pixels and " << pictureBuffer.channels() << " channels";
		return outputStream.str();
	}
	//
}