#pragma once
#include <map>
#include <array>
#include <string>

#include "KablamEngine.h"

constexpr int CHAR_WIDTH = 5;
constexpr int CHAR_HEIGHT = 5;

// CharacterDisplay Class Definition
class TextDisplay
{
private:
	std::map<wchar_t, std::array<int, CHAR_WIDTH* CHAR_HEIGHT>> letterPatterns;

public:
	TextDisplay();

	// Method to add more patterns dynamically
	void AddPattern(wchar_t wc, const std::array<int, CHAR_WIDTH* CHAR_HEIGHT>& pattern);

	// Method to draw a single character at a specific position
	void DisplayChar(KablamEngine& game, int x, int y, short colour, short glyph, wchar_t c);

	// Method to draw a string at a specific position
	void DisplayString(KablamEngine& game, int x, int y, const std::wstring& text, short colour, short glyph);
};

