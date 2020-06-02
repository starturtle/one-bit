#include "Pixelator.h"
namespace imaging
{
    namespace
    {
        double colorDistance(PixelValue in_colorTo, PixelValue in_colorFrom);
    }

    Pixelator::Pixelator(const std::string& in_path)
        : pictureBuffer{ cv::imread(in_path, cv::ImreadModes::IMREAD_COLOR) }
        , resultColors{}
    {

    }
    bool Pixelator::run(const std::string& in_writePath, const int rows, const int columns)
    {
        return prepare() && to_pixels(rows, columns) && to_file(in_writePath);
    }
    bool Pixelator::prepare()
    {
        add_color(PixelValue(255, 255, 255));
        add_color(PixelValue());
        return true;
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
    std::string Pixelator::imageStats() const
    {
        std::stringstream outputStream;
        outputStream << "Image has " << pictureBuffer.rows << "x" << pictureBuffer.cols << " pixels and " << pictureBuffer.channels() << " channels";
        return outputStream.str();
    }

    bool Pixelator::display(const std::string& in_imageName)
    {
        bool hasPicture = ((! pictureBuffer.empty()) && (nullptr != pictureBuffer.data));
        if (hasPicture)
        {
            cv::namedWindow(in_imageName, cv::WINDOW_AUTOSIZE);
            cv::imshow(in_imageName, pictureBuffer);
            cv::waitKey(0);
        }
        return hasPicture;
    }

    bool Pixelator::binarize()
    {
        ImgData hsvFile(pictureBuffer.cols, pictureBuffer.rows, pictureBuffer.type());
        pictureBuffer.convertTo(hsvFile, cv::COLOR_BGR2HSV);
        CV_Assert(hsvFile.channels() == 3);
        ImgData outputFile = pictureBuffer.clone();
        for (int i = 0; i < hsvFile.rows; ++i) {
            for (int j = 0; j < hsvFile.cols; ++j) {
                PixelValue outValue = resultColors[0];
                PixelValue& content = hsvFile.at<cv::Vec3b>(i, j);
                PixelValue& output = outputFile.at<cv::Vec3b>(i, j);
                for (auto color : resultColors)
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
        pictureBuffer = outputFile.clone();
        return true;
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