#pragma once

namespace errors
{
	int constexpr NONE = 0;
	int constexpr PARSE_FAILED = 1;
	int constexpr WRONG_INPUT_FILE = 2;
	int constexpr INVALID_IMAGE_SIZES = 3;
	int constexpr PREPARATION_ERROR = 4;
	int constexpr PIXELATION_ERROR = 5;
	int constexpr PRINT_ERROR = 6;
	int constexpr WRONG_OUTPUT_FILE = 7;
	int constexpr QT_ERROR = 8;
	int constexpr NOT_IMPLEMENTED = -1;
}