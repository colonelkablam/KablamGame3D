#include "pch.h"
#include "TextDisplay.h"

TextDisplay::TextDisplay()
{
	// Initialize character patterns A-Z, 0-9, 
	letterPatternsSmall = {
			{L' ', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0
			}},
			{L'.', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 1, 0, 0, 0
			}},
			{L'A', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1
			}},
			{L'B', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0
			}},
			{L'C', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'D', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 0, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 1, 0,
				1, 1, 1, 0, 0
			}},
			{L'E', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'F', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0
			}},
			{L'G', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 0, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'H', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1
			}},
			{L'I', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'J', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 0, 0, 0, 1,
				0, 0, 0, 0, 1,
				0, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'K', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 0, 0, 1, 0,
				1, 1, 1, 0, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 0, 1
			}},
			{L'L', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'M', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 1, 0, 1, 1,
				1, 0, 1, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1
			}},
			{L'N', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 1, 0, 0, 1,
				1, 0, 1, 0, 1,
				1, 0, 0, 1, 1,
				1, 0, 0, 0, 1
			}},
			{L'O', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				0, 1, 1, 1, 0
			}},
			{L'P', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0
			}},
			{L'Q', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 1, 0,
				0, 1, 1, 0, 1
			}},
			{L'R', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 0, 1
			}},
			{L'S', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				0, 1, 1, 1, 0,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 0
			}},
			{L'T', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0
			}},
			{L'U', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				0, 1, 1, 1, 0
			}},
			{L'V', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				0, 1, 0, 1, 0,
				0, 0, 1, 0, 0
			}},
			{L'W', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 1, 0, 1,
				1, 1, 0, 1, 1,
				1, 0, 0, 0, 1
			}},
			{L'X', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				0, 1, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 1, 0, 1, 0,
				1, 0, 0, 0, 1
			}},
			{L'Y', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				0, 1, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0
			}},
			{L'Z', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 1, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'0', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'1', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				0, 0, 1, 0, 0,
				0, 1, 1, 0, 0,
				1, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'2', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'3', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'4', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 0, 0, 0, 1
			}},
			{L'5', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'6', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'7', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 0, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 1, 0, 0, 0
			}},
			{L'8', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'9', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{ L'?', std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 1, 1, 1, 1,
				0, 0, 0, 0, 0,
				0, 0, 1, 0, 0
			}}
	};

	// Initialize character patterns 0-9
	letterPatternsLarge = {
			{L'?', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0
			}},
			{L'A', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
				0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0
			}},
			{L'B', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'C', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'D', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 0, 0, 0
			}},
			{L'E', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0
			}},
			{L'F', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0
			}},
			{L'G', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 1, 1, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'H', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0
			}},
			{L'I', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'J', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 0, 0, 0
			}},
			{ L'K', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
				1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0
			}},
			{ L'L', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0
			}},
			{ L'M', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
				1, 0, 1, 0, 0, 0, 0, 1, 0, 1,
				1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
				1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1
			}},
			{ L'N', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 1, 1, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 1, 1, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 1, 1, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 1, 1, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 1, 1, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 1, 1, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 1
			}},
			{ L'O', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{ L'P', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0
			}},
			{ L'Q', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 1, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 1, 0, 1, 0, 0,
				0, 1, 1, 1, 1, 0, 0, 0, 1, 0
			}},
			{ L'R', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0
			}},
			{ L'S', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{ L'T', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0
			}},
			{ L'U', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 0
			}},
			{ L'V', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 1, 1, 1, 1, 0, 0, 0
			}},
			{ L'W', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
				1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
				0, 1, 1, 0, 0, 0, 0, 1, 1, 0
			}},
			{ L'X', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1
			}},
			{ L'Y', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0
			}},
			{ L'Z', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0
			}},
			{L' ', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			}},
			{L'.', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 
				0, 0, 1, 1, 0, 0, 0, 0, 0, 0
			}},
			{L'0', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 1, 0,
				1, 0, 0, 0, 0, 1, 0, 0, 1, 0,
				1, 0, 0, 0, 1, 0, 0, 0, 1, 0,
				1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
				1, 0, 1, 0, 0, 0, 0, 0, 1, 0,
				1, 1, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'1', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
				0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'2', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
				1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0
			}},
			{L'3', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'4', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0
			}},
			{L'5', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'6', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'7', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 0, 0, 0, 0
			}},
			{L'8', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
			{L'9', std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>{
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 0, 0
			}},
	};
}

// Method to add more patterns dynamically
void TextDisplay::AddPatternSmall(wchar_t wc, const std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>& pattern)
{
	letterPatternsSmall[toupper(wc)] = pattern;
}

void TextDisplay::AddPatternLarge(wchar_t wc, const std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>& pattern)
{
	letterPatternsLarge[toupper(wc)] = pattern;
}

// Method to draw a single character at a specific position
template<size_t PatternSize>
void DisplayCharTemplate(KablamEngine& game, int x, int y, short colour, short glyph, wchar_t wc, int scale, const std::map<wchar_t, std::array<int, PatternSize>>& patterns, int charWidth, int charHeight)
{
	auto it = patterns.find(toupper(wc));
	const auto& pattern = (it != patterns.end()) ? it->second : patterns.at(L'?');

	for (int i = 0; i < charWidth; ++i)
	{
		for (int j = 0; j < charHeight; ++j)
		{
			if (pattern[j * charWidth + i] == 1)
			{
				int drawX = x + i * scale;
				int drawY = y + j * scale;

				if (drawX >= 0 && drawX < game.GetConsoleWidth() &&
					drawY >= 0 && drawY < game.GetConsoleHeight())
				{
					game.DrawBlock(drawX, drawY, scale, colour, glyph);
				}			
			}
		}
	}
}

// Method to draw a single character at a specific position - using a template function above 
void TextDisplay::DisplayChar(KablamEngine& game, int x, int y, short colour, short glyph, wchar_t wc, int scale, bool useLargePattern)
{
	if (useLargePattern)
	{
		DisplayCharTemplate(game, x, y, colour, glyph, wc, scale, letterPatternsLarge, CHAR_WIDTH_LARGE, CHAR_HEIGHT_LARGE);
	}
	else
	{
		DisplayCharTemplate(game, x, y, colour, glyph, wc, scale, letterPatternsSmall, CHAR_WIDTH_SMALL, CHAR_HEIGHT_SMALL);
	}
}

// Method to draw a string at a specific position
void TextDisplay::DisplayString(KablamEngine& game, int x, int y, const std::wstring& text, short colour, short glyph, int scale, bool useLargePattern)
{
	int charWidth = useLargePattern ? CHAR_WIDTH_LARGE : CHAR_WIDTH_SMALL;
	int xOffset = x;
	for (wchar_t wc : text)
	{
		DisplayChar(game, xOffset, y, colour, glyph, wc, scale, useLargePattern);
		xOffset += (charWidth * scale) + scale; // Adjust spacing between characters based on scale
	}
}

