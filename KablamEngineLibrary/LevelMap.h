
#pragma once

#include <string>
#include <map>
#include <array>


class Level
{

private:
	int m_map_width;
	int m_map_height;

	int* m_floorArray;
	int* m_wallArray;
	int* m_ceilingArray;

	int* m_npcArray;
	int* m_itemArray;

	std::vector levelTextures

public:

	// constructors

	// dimensions and illumination needed
	Level(int w = 64, int h = 64);

	// from saved file
	Level(std::wstring sFilePath);

	// Copy constructor
	Level(const Level& other);

	// copyassignment operator
	Level& operator=(const Level& other);

	// destructor
	~Level();

	// member methods
private:
	bool Initialise(int w, int h);

public:
	bool SaveAs(const std::wstring& sFilePath);
	bool LoadFrom(const std::wstring& sFilePath);

	short SampleFloor(float x, float) const;
	short SampleGlyph(float x, float y) const;


	bool SetColour(int x, int y, short colour);

	bool SetGlyph(int x, int y, short glyph);

	bool SetPixel(int x, int y, CHAR_INFO pixel);

	short GetColour(int x, int y) const;

	short GetGlyph(int x, int y) const;

	CHAR_INFO GetPixel(int x, int y) const;

	int GetWidth() const;

	int GetHeight() const;

	void Clear(int colour = 0, short glyph = L' ');

};







