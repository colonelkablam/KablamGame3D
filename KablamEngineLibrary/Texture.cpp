
#include "pch.h"
#include "Texture.h"


// undefine the macros from windows.h
#undef max
#undef min

Texture::Texture(int w, int h, int illum)
	: m_colourArray{ nullptr }, m_glyphArray{ nullptr }, topMipmap{ nullptr }
{
	Initialise(w, h, illum);

	// populate linked list of mipmaps
	GenerateMipmaps();
}

Texture::Texture(std::wstring filePath)
	: m_colourArray{ nullptr }, m_glyphArray{ nullptr }, topMipmap{ nullptr }
{
	if (!LoadFrom(filePath))
	{
		// if failed to load make a 32*32 dark red canvas
		Initialise(32, 32, 0, FG_DARK_MAGENTA);
	}
	// populate linked list of mipmaps
	GenerateMipmaps();
}

// copy constructor TBC
//Texture::Texture(Texture& source)
//{
//}

Texture::~Texture()
{
	delete[] m_colourArray;
	m_colourArray = nullptr;
	delete[] m_glyphArray;
	m_glyphArray = nullptr;

	MipmapLevel* current = topMipmap;
	while (current != nullptr) {
		MipmapLevel* next = current->next;
		delete[] current->colourArray;
		delete[] current->glyphArray;
		current = next;
	}
}

bool Texture::Initialise(int width, int height, bool illumination, short colour)
{
	m_width = width;
	m_height = height;
	m_illumination = illumination;
	m_colourArray = new short[width * height];
	m_glyphArray = new short[width * height];

	for (int i{ 0 }; i < m_width * m_height; i++)
	{
		m_colourArray[i] = colour;
		m_glyphArray[i] = L' ';
	}

	return true;
}

// populate texture instance with full complement of mipmaps
void Texture::GenerateMipmaps() {

	// define top mipmap level
	topMipmap = new MipmapLevel(m_width, m_height, m_illumination);
	// size of arrays in bytes for memcpy_s
	size_t arraySizeBytes = m_width * m_height * sizeof(m_colourArray[0]);
	// populate top mipmap with copy of original texture 
	topMipmap->colourArray = new short[m_width * m_height];
	memcpy_s(topMipmap->colourArray, arraySizeBytes, m_colourArray, arraySizeBytes);
	topMipmap->glyphArray = new short[m_width * m_height];
	memcpy_s(topMipmap->glyphArray, arraySizeBytes, m_glyphArray, arraySizeBytes);
	// link to next mipmap
	topMipmap->next = nullptr;

	// generate mipmaps until 1*1 texture
	MipmapLevel* currentLevel = topMipmap;
	while (currentLevel->width > 1 && currentLevel->height > 1) {
		int nextWidth = currentLevel->width / 2;
		int nextHeight = currentLevel->height / 2;

		MipmapLevel* nextLevel = new MipmapLevel(nextWidth, nextHeight, m_illumination);

		// lower res sample into next level
		Downsample(currentLevel, nextLevel);

		currentLevel->next = nextLevel;  // Link to the new level
		currentLevel = nextLevel;  // Move down to the new level
	}
}

// samples 2x2 block of current level texels to find dominant colour for next level texel
void Texture::Downsample(MipmapLevel* currentLevel, MipmapLevel* nextLevel) {
	int currentWidth = currentLevel->width;
	int nextWidth = nextLevel->width;
	int nextHeight = nextLevel->height;

	for (int y = 0; y < nextHeight; ++y) {
		for (int x = 0; x < nextWidth; ++x) {
			// Corresponding top-left pixel of 2x2 sample in the current level
			int topLeftX = x * 2;
			int topLeftY = y * 2;

			// map to store 4 sampled colours
			std::map<short, int> colourMap;

			// Sample 2x2 block from current level into map
			for (int dy = 0; dy < 2; ++dy) {
				for (int dx = 0; dx < 2; ++dx) {
					int index = (topLeftY + dy) * currentWidth + (topLeftX + dx);
					colourMap[currentLevel->colourArray[index]]++;
				}
			}

			// to count colours
			std::pair<short, int> firstColour{ 0, 0 };
			std::pair<short, int> secondColour{ 0, 0 };

			if (colourMap.size() > 1)
				TwoMainColourCounts(colourMap, firstColour, secondColour);
			else
			{
				// only one colour
				firstColour.first = colourMap.begin()->first;
				secondColour.first = colourMap.begin()->first;
			}


			// Set dominant colour in the next level with BG as second colour
			nextLevel->colourArray[y * nextWidth + x] = firstColour.first;
			nextLevel->glyphArray[y * nextWidth + x] = PIXEL_SOLID;
		}
	}
}

// very simple bubble sort (between two pairs)
void Texture::TwoMainColourCounts(const std::map<short, int>& colourMap, std::pair<short, int>& firstColour, std::pair<short, int>& secondColour)
{
	// iterate through to get two main colours
	for (const auto& pair : colourMap) {
		if (pair.second > firstColour.second) {
			secondColour = firstColour; // Second becomes what used to be the max
			firstColour = pair;
		}
		else if (pair.second > secondColour.second) {
			secondColour = pair;
		}
	}
}

Texture::MipmapLevel* Texture::GetMipmapLevel(float detail) const {
	MipmapLevel* currentLevel = topMipmap;
	int level = 0;

	// determine mipmap level based on 'detail' - distance from texture
	while (currentLevel->next != nullptr && level < detail) {
		currentLevel = currentLevel->next;
		level++;
	}

	return currentLevel;
}

bool Texture::SaveAs(const std::wstring& filePath)
{
	FILE* file = nullptr;
	_wfopen_s(&file, filePath.c_str(), L"wb");
	if (file == nullptr)
		return false;

	// write attributes into the file
	fwrite(&m_width, sizeof(int), 1, file);
	fwrite(&m_height, sizeof(int), 1, file);
	fwrite(&m_illumination, sizeof(int), 1, file);
	// write array data into the file
	fwrite(m_colourArray, sizeof(short), m_width * m_height, file);
	fwrite(m_glyphArray, sizeof(short), m_width * m_height, file);

	fclose(file);

	return true;
}

bool Texture::LoadFrom(const std::wstring& filePath)
{
	// initialise attributes
	m_width = 0;
	m_height = 0;
	m_illumination = 0;
	delete[] m_colourArray;
	delete[] m_glyphArray;

	FILE* file = nullptr;
	_wfopen_s(&file, filePath.c_str(), L"rb");
	if (file == nullptr)
		return false;

	// read characteristics of saved texture
	std::fread(&m_width, sizeof(int), 1, file);
	std::fread(&m_height, sizeof(int), 1, file);
	std::fread(&m_illumination, sizeof(int), 1, file);

	// create an empty texture from loaded data
	Initialise(m_width, m_height, m_illumination);

	// read colours/glyphs into texture
	std::fread(m_colourArray, sizeof(short), m_width * m_height, file);
	std::fread(m_glyphArray, sizeof(short), m_width * m_height, file);

	std::fclose(file);

	return true;

}

short Texture::SampleColour(float x, float y) const
{
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// Scale the normalized coordinates to array indices - handle edge cases
	int ix = static_cast<int>(x * (m_width));
	ix = std::min(ix, m_width - 1);          // Clamp to the maximum valid index

	int iy = static_cast<int>(y * (m_height));
	iy = std::min(iy, m_height - 1);          // Clamp to the maximum valid index

	// Calculate the index in the colour array
	int index = iy * m_width + ix;

	// Return the colour at the index
	return m_colourArray[index];
}


short Texture::SampleGlyph(float x, float y) const
{
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// Scale the normalized coordinates to array indices - handle edge cases
	int ix = static_cast<int>(x * (m_width)) % m_width;
	int iy = static_cast<int>(y * (m_height)) % m_height;

	// Calculate the index in the glyph array
	int index = iy * m_width + ix;

	// Return the glyph at the index
	return m_glyphArray[index];
}

CHAR_INFO Texture::SamplePixel(float x, float y) const
{
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// Scale the normalized coordinates to array indices - handle edge cases
	int ix = static_cast<int>(x * (m_width)) % m_width;
	int iy = static_cast<int>(y * (m_height)) % m_height;

	// Calculate the index in the glyph array
	size_t index = iy * m_width + ix;

	CHAR_INFO pixel{ 0 };
	pixel.Attributes = m_colourArray[index];
	pixel.Char.UnicodeChar = m_glyphArray[index];

	return pixel;
}

// MIPMAPPING
CHAR_INFO Texture::SamplePixelWithMipmap(float x, float y, float detail) const {

	// Determine appropriate mipmap level based on detail 
	// (0 is the OG texture, 1 is halved res, 2 is halved again and so on...)
	MipmapLevel* level = GetMipmapLevel(detail);

	// Scale x, y to level dimensions and clamp
	x = std::max(0.0f, std::min(x, 1.0f)) * level->width;
	y = std::max(0.0f, std::min(y, 1.0f)) * level->height;

	int ix = std::min(static_cast<int>(x), level->width - 1);
	int iy = std::min(static_cast<int>(y), level->height - 1);

	// Return colour from the selected mipmap level
	size_t index = iy * level->width + ix;

	CHAR_INFO pixel{ 0 };
	pixel.Attributes = level->colourArray[index];
	pixel.Char.UnicodeChar = level->glyphArray[index];

	return pixel;
}

// looks at 4 closest texels and picks dominant (c00) and secondary (c01) colour
CHAR_INFO Texture::LinearInterpolationWithGlyphShading(float x, float y) {

	CHAR_INFO pixel;
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// get decimal value of the relative position in texture grid
	float fx = x * m_width, fy = y * m_height;

	// get grid index (integer part) of TEXEL to be sampled
	int ix = static_cast<int>(fx) % m_width , iy = static_cast<int>(fy) % m_height;

	// get the fractional part within the TEXEL
	float dx = fx - ix, dy = fy - iy;

	// get the colour at the grid index (TEXEL hit by xy)
	short c00 = GetColour(ix, iy);

	// init. delta (this will be the distance from the center of the MAIN TEXEL
	float delta{ 0.0f };
	// init. 2nd colour & glyph
	short c01{ BG_CYAN }; // default

	// set secondary colour and alter delta accordingly
	SetColourAndDeltaFromSecondaryTexel(ix, iy, dx, dy, c00, c01, delta);

	// assign glyph shade depending on delta
	short glyph{ GetGlyphFromDelta(delta) };

	// assign values to pixel to display pixel
	pixel.Attributes = c00 | (c01 << 4 );
	pixel.Char.UnicodeChar = glyph;

	return pixel;
}

// takes a reference to second colour and delta for modification
void Texture::SetColourAndDeltaFromSecondaryTexel(int ix, int iy, float dx, float dy, short primaryColour, short& secondaryColour, float& delta) {

	//	 BOTTOM RIGHT quadrant of texel
	if (dx >= 0.5f && dy >= 0.5f) {
		if (dx > dy) { // RIGHT takes precedence
			secondaryColour = m_colourArray[iy * m_width + (ix + 1) % m_width]; // Wrap around horizontally
			delta = dx - 0.5f;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[((iy + 1) % m_height) * m_width + ix]; // Wrap around vertically
				delta = dy - 0.5f;
			}
		}
		else { // DOWN takes precedence
			secondaryColour = m_colourArray[((iy + 1) % m_height) * m_width + ix]; // Wrap around vertically
			delta = dy - 0.5f;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[iy * m_width + (ix + 1) % m_width]; // Wrap around horizontally
				delta = dx - 0.5f;
			}
		}
	}
	// TOP RIGHT quadrant of texel
	else if (dx >= 0.5f && dy <= 0.5f) {
		if (1.0f - dx < dy) { // RIGHT takes precedence over UP
			secondaryColour = m_colourArray[iy * m_width + (ix + 1) % m_width]; // Wrap around horizontally
			delta = dx - 0.5f;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[((iy - 1 + m_height) % m_height) * m_width + ix]; // Wrap around vertically
				delta = 0.5f - dy;
			}
		}
		else { // UP takes precedence
			secondaryColour = m_colourArray[((iy - 1 + m_height) % m_height) * m_width + ix]; // Wrap around vertically
			delta = 0.5f - dy;
			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[iy * m_width + (ix + 1) % m_width]; // Wrap around horizontally
				delta = dx - 0.5f;
			}
		}
	}
	// BOTTOM LEFT quadrant of texel
	else if (dx <= 0.5f && dy >= 0.5f) {
		if (dx < 1.0f - dy) { // LEFT takes precedence
			secondaryColour = m_colourArray[iy * m_width + ((ix - 1 + m_width) % m_width)]; // Wrap around horizontally
			delta = 0.5f - dx;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[((iy + 1) % m_height) * m_width + ix]; // Wrap around vertically
				delta = dy - 0.5f;
			}
		}
		else { // DOWN takes precedence
			secondaryColour = m_colourArray[((iy + 1) % m_height) * m_width + ix]; // Wrap around vertically
			delta = dy - 0.5f;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[iy * m_width + ((ix - 1 + m_width) % m_width)]; // Wrap around horizontally
				delta = 0.5f - dx;
			}
		}
	}
	// TOP LEFT quadrant of texel
	else if (dx <= 0.5f && dy <= 0.5f) {
		if (dx < dy) { // LEFT takes precedence over UP
			secondaryColour = m_colourArray[iy * m_width + ((ix - 1 + m_width) % m_width)]; // Wrap around horizontally
			delta = 0.5f - dx;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[((iy - 1 + m_height) % m_height) * m_width + ix]; // Wrap around vertically
				delta = 0.5f - dy;
			}
		}
		else { // UP takes precedence
			secondaryColour = m_colourArray[((iy - 1 + m_height) % m_height) * m_width + ix]; // Wrap around vertically
			delta = 0.5f - dy;

			if (primaryColour == secondaryColour)
			{
				secondaryColour = m_colourArray[iy * m_width + ((ix - 1 + m_width) % m_width)]; // Wrap around horizontally
				delta = 0.5f - dx;
			}
		}
	}
}

short Texture::GetGlyphFromDelta(float delta) {
	// Determine the glyph based on delta
	if (delta < 0.30)
		return PIXEL_SOLID;
	else if (delta < 0.45f)
		return PIXEL_THREEQUARTERS;
	else
		return PIXEL_HALF;
}

int Texture::GetIllumination() const
{
	return m_illumination;
}

bool Texture::SetColour(int x, int y, short colour)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return false;
	else
		m_colourArray[y * m_width + x] = colour;

	return true;
}

bool Texture::SetGlyph(int x, int y, short glyph)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return false;
	else
		m_glyphArray[y * m_width + x] = glyph;

	return true;
}

bool Texture::SetPixel(int x, int y, CHAR_INFO pixel)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return false;
	else
	{
		m_colourArray[y * m_width + x] = pixel.Attributes;
		m_glyphArray[y * m_width + x] = pixel.Char.UnicodeChar;
	}

	return true;
}

short Texture::GetColour(int x, int y) const
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return FG_BLACK;
	else
		return m_colourArray[y * m_width + x];
}

short Texture::GetGlyph(int x, int y) const
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return L' ';
	else
		return m_glyphArray[y * m_width + x];
}

CHAR_INFO Texture::GetPixel(int x, int y ) const
{
	// 
	CHAR_INFO pixel{ 0 };
	short colour{ FG_MAGENTA | BG_DARK_MAGENTA };
	short glyph{ L'X' };

	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return pixel;
	else
	{
		pixel.Attributes = m_colourArray[y * m_width + x];
		pixel.Char.UnicodeChar = m_glyphArray[y * m_width + x];
		return pixel;
	}
}

int Texture::GetWidth() const
{
	return m_width;
}

int Texture::GetHeight() const
{
	return m_height;
}

// merge another texture over (ignoring 'empty' glyphs)
void Texture::MergeOther(const Texture* other)
{
	for (size_t i{ 0 }; i < m_width * m_height; i++)
		if (other->m_glyphArray[i] != L' ')
			m_colourArray[i] = other->m_colourArray[i];
}


void Texture::Clear(short colour, short glyph)
{
	for (size_t i{ 0 }; i < m_width * m_height; i++)
	{
		m_colourArray[i] = colour;
		m_glyphArray[i] = glyph;
	}
}

