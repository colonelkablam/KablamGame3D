#include "pch.h"
#include "TextDisplay.h"

TextDisplay::TextDisplay()
{
	// Initialize character patterns A-Z, 0-9
	letterPatterns = {
		{L'A', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				0, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1
			}},
			{L'B', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0
			}},
			{L'C', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'D', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 0, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 1, 0,
				1, 1, 1, 0, 0
			}},
			{L'E', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'F', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0
			}},
			{L'G', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 0, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'H', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1
			}},
			{L'I', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'J', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				0, 0, 0, 0, 1,
				0, 0, 0, 0, 1,
				0, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'K', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 0, 0, 1, 0,
				1, 1, 1, 0, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 0, 1
			}},
			{L'L', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'M', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 1, 0, 1, 1,
				1, 0, 1, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1
			}},
			{L'N', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 1, 0, 0, 1,
				1, 0, 1, 0, 1,
				1, 0, 0, 1, 1,
				1, 0, 0, 0, 1
			}},
			{L'O', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				0, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				0, 1, 1, 1, 0
			}},
			{L'P', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 0,
				1, 0, 0, 0, 0
			}},
			{L'Q', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				0, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 1, 0,
				0, 1, 1, 0, 1
			}},
			{L'R', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 0,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 0,
				1, 0, 0, 1, 0,
				1, 0, 0, 0, 1
			}},
			{L'S', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				0, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				0, 1, 1, 1, 0,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 0
			}},
			{L'T', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0
			}},
			{L'U', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				0, 1, 1, 1, 0
			}},
			{L'V', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				0, 1, 0, 1, 0,
				0, 0, 1, 0, 0
			}},
			{L'W', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 1, 0, 1,
				1, 1, 0, 1, 1,
				1, 0, 0, 0, 1
			}},
			{L'X', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				0, 1, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 1, 0, 1, 0,
				1, 0, 0, 0, 1
			}},
			{L'Y', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				0, 1, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 1, 0, 0
			}},
			{L'Z', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				0, 0, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 1, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'0', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'1', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				0, 0, 1, 0, 0,
				0, 1, 1, 0, 0,
				1, 0, 1, 0, 0,
				0, 0, 1, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'2', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1
			}},
			{L'3', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'4', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 0, 0, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 0, 0, 0, 1
			}},
			{L'5', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'6', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'7', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				0, 0, 0, 1, 0,
				0, 0, 1, 0, 0,
				0, 1, 0, 0, 0
			}},
			{L'8', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}},
			{L'9', std::array<int, CHAR_WIDTH * CHAR_HEIGHT>{
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
				0, 0, 0, 0, 1,
				1, 1, 1, 1, 1
			}}
	};
}

// Method to add more patterns dynamically
void TextDisplay::AddPattern(wchar_t wc, const std::array<int, CHAR_WIDTH* CHAR_HEIGHT>& pattern)
{
	letterPatterns[toupper(wc)] = pattern;
}

// Method to draw a single character at a specific position
void TextDisplay::DisplayChar(KablamEngine& game, int x, int y, short colour, short glyph, wchar_t wc)
{
	auto it = letterPatterns.find(toupper(wc));
	if (it != letterPatterns.end())
	{
		const auto& pattern = it->second;
		for (int i = 0; i < CHAR_WIDTH; ++i)
		{
			for (int j = 0; j < CHAR_HEIGHT; ++j)
			{
				if (pattern[j * CHAR_WIDTH + i] == 1)
				{
					int drawX = x + i;
					int drawY = y + j;

					if (drawX >= 0 && drawX < game.GetConsoleWidth() &&
						drawY >= 0 && drawY < game.GetConsoleHeight())
					{
						game.DrawPoint(drawX, drawY, colour, glyph);
					}
				}
			}
		}
	}
}

// Method to draw a string at a specific position
void TextDisplay::DisplayString(KablamEngine& game, int x, int y, const std::wstring& text, short colour, short glyph)
{
	int xOffset = x;
	for (wchar_t wc : text)
	{
		DisplayChar(game, xOffset, y, colour, glyph, wc);
		xOffset += CHAR_WIDTH + 1; // Add spacing between characters
	}
}

