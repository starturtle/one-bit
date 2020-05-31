#pragma once

namespace errors
{
	using Code = int;
	Code constexpr NONE = 0;
	Code constexpr PARSE_FAILED = 1;
	Code constexpr WRONG_INPUT_FILE = 2;
	Code constexpr INVALID_IMAGE_SIZES = 3;
	Code constexpr PREPARATION_ERROR = 4;
	Code constexpr PIXELATION_ERROR = 5;
	Code constexpr PRINT_ERROR = 6;
	Code constexpr WRONG_OUTPUT_FILE = 7;
	Code constexpr QT_ERROR = 8;
	Code constexpr PAINT_ERROR = 9;
	Code constexpr WRITE_ERROR = 10;
	Code constexpr NOT_IMPLEMENTED = -1;
}