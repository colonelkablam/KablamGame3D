
#pragma once

#include <string>
#include <map>
#include <array>

// from olc @ https://www.onelonecoder.com
enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
	// intensity
	BG_INTENSITY = 0x0080,
	FG_INTENSITY = 0x0008
};

// from olc @ https://www.onelonecoder.com
enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class Texture
{
private:
	int m_width;
	int m_height;
	int m_illumination;
	short* m_colourArray;
	short* m_glyphArray;

	// linked list to lower res textures
	struct MipmapLevel {
		int width, height;
		int illuminated;
		short* colourArray;
		short* glyphArray;
		MipmapLevel* next;  // Pointer to the next mipmap level (lower resolution)

		// constructor
		MipmapLevel(int w, int h, int illum) : width(w), height(h), illuminated(illum), next(nullptr) {
			// Initialize arrays, e.g., with default color and glyph
			colourArray = new short[w * h] {FG_DARK_MAGENTA};
			glyphArray = new short[w * h] {L'X'};
		}

		~MipmapLevel() {
			delete[] colourArray;
			colourArray = nullptr;
			delete[] glyphArray;
			glyphArray = nullptr;
		}
	};

	MipmapLevel* topMipmap; // start at highest res

public:

	// constructors
	Texture(int w = 32, int h = 32, int illumination = 0);

	// from saved file
	Texture(std::wstring sFilePath = L".\\Textures\\Untitled.txr");

	// DELETE Copy constructor
	Texture(const Texture& other) = delete;

	// destructor
	~Texture();

	// member methods
private:
	bool Initialise(int w, int h, bool illuminated = false, short forgroundColour = FG_BLACK);
	
	void GenerateMipmaps();

	MipmapLevel* copyMipmapLevels(const MipmapLevel* source);

	void Downsample(MipmapLevel* currentLevel, MipmapLevel* nextLevel);
	
	void TwoMainColourCounts(const std::map<short, int>& colourMap, std::pair<short, int>& firstColour, std::pair<short, int>& secondColour);

public:
	bool SaveAs(const std::wstring& sFilePath);
	bool LoadFrom(const std::wstring& sFilePath);

	// no mipmap
	short SampleColour(float x, float) const;
	short SampleGlyph(float x, float y) const;
	CHAR_INFO SamplePixel(float x, float y) const;

	// mipmap
	CHAR_INFO SamplePixelWithMipmap(float x, float y, float detail) const;

private:
	MipmapLevel* GetMipmapLevel(float detail) const;

	// filtering sampling
public:
	CHAR_INFO LinearInterpolationWithGlyphShading(float x, float y);

private:
	void SetColourAndDeltaFromSecondaryTexel(int ix, int iy, float dx, float dy, short primaryColour, short& secondaryColour, float& delta);
	short GetGlyphFromDelta(float delta);

public:
	int GetIllumination() const;

	bool SetColour(int x, int y, short colour);

	bool SetGlyph(int x, int y, short glyph);

	bool SetPixel(int x, int y, CHAR_INFO pixel);

	short GetColour(int x, int y) const;

	short GetGlyph(int x, int y) const;

	CHAR_INFO GetPixel(int x, int y) const;

	int GetWidth() const;

	int GetHeight() const;

	Texture* MergeOther(const Texture* other);

	void Clear(short colour = 0, short glyph = L' ');

};







