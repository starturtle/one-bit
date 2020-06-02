#pragma once
#include "Pixelator.h"

namespace imaging
{
    class FloodFillPixelator : public Pixelator
    {
    public:
        FloodFillPixelator(const std::string& in_path);
        bool to_pixels(int rows, int columns) final;
    private:
        bool pixelate(int rows, int columns);
        PixelValue getMostCommonColor(int start_row, int start_column, int end_row, int end_column);
    };

}
