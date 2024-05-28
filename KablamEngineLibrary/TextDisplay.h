#pragma once
#include <map>
#include <array>
#include <string>

#include "KablamEngine.h"

constexpr int CHAR_WIDTH_SMALL = 5;
constexpr int CHAR_HEIGHT_SMALL = 5;
constexpr int CHAR_WIDTH_LARGE = 10;
constexpr int CHAR_HEIGHT_LARGE = 10;

// CharacterDisplay Class Definition
class TextDisplay
{
private:
	std::map<wchar_t, std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>> letterPatternsSmall;
	std::map<wchar_t, std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>> letterPatternsLarge;

public:
	TextDisplay();

	// Methods to add more patterns dynamically
	void AddPatternSmall(wchar_t wc, const std::array<int, CHAR_WIDTH_SMALL * CHAR_HEIGHT_SMALL>& pattern);
	void AddPatternLarge(wchar_t wc, const std::array<int, CHAR_WIDTH_LARGE * CHAR_HEIGHT_LARGE>& pattern);

	// Method to draw a single character at a specific position
	void DisplayChar(KablamEngine& game, int x, int y, short colour, short glyph, wchar_t c, int scale = 1, bool useLargePattern = false);

	// Method to draw a string at a specific position
	void DisplayString(KablamEngine& game, int x, int y, const std::wstring& text, short colour, short glyph, int scale = 1, bool useLargePattern = false);
};

