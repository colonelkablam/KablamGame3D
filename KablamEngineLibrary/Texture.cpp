
#include "pch.h"
#include "Texture.h"


// undefine the macros from windows.h
#undef max
#undef min

Texture::Texture(int w, int h, bool illum)
	: m_colourArray{ nullptr }, m_glyphArray{ nullptr }
{
	Initialise(w, h, illum);
}

Texture::Texture(std::wstring filePath)
	: m_colourArray{ nullptr }, m_glyphArray{ nullptr }
{
	if (!LoadFrom(filePath))
	{
		// if failed to load make a 32*32 dark red canvas
		Initialise(32, 32, false, FG_DARK_MAGENTA);
	}
}

// copy constructor TBC
//Texture::Texture(Texture& source)
//{
//}

Texture::~Texture()
{
	delete[] m_colourArray;
	delete[] m_glyphArray;
}

bool Texture::Initialise(int width, int height, bool illumination, short colour)
{
	m_width = width;
	m_height = height;
	m_illuminated = illumination;
	m_colourArray = new short[width * height];
	m_glyphArray = new short[width * height];

	for (int i{ 0 }; i < m_width * m_height; i++)
	{
		m_colourArray[i] = colour;
		m_glyphArray[i] = L' ';
	}
	return true;
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
	fwrite(&m_illuminated, sizeof(bool), 1, file);
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
	m_illuminated = false;
	delete[] m_colourArray;
	delete[] m_glyphArray;

	FILE* file = nullptr;
	_wfopen_s(&file, filePath.c_str(), L"rb");
	if (file == nullptr)
		return false;

	// read characteristics of saved texture
	std::fread(&m_width, sizeof(int), 1, file);
	std::fread(&m_height, sizeof(int), 1, file);
	std::fread(&m_illuminated, sizeof(bool), 1, file);

	// create an empty texture from loaded data
	Initialise(m_width, m_height, m_illuminated);

	// read colours/glyphs into texture
	std::fread(m_colourArray, sizeof(short), m_width * m_height, file);
	std::fread(m_glyphArray, sizeof(short), m_width * m_height, file);

	std::fclose(file);

	return true;

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

void Texture::BilinearInterpolationWithGlyphShading(float x, float y, CHAR_INFO& pixel)
{
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// needed to find the dx/dy within the TEXEL
	float fx = x * m_width;
	float fy = y * m_height;

	// Convert to integer indices - allow wrap around
	int ix = static_cast<int>(fx) % m_width;
	int iy = static_cast<int>(fy) % m_height;

	// Calculate distance from TEXEL edge
	float dx = fx - ix;
	float dy = fy - iy;

	// Sample the center FG colour texel
	short c00 = m_colourArray[iy * m_width + ix];

	// colour of secondary colour (to be used as BG colour)
	short c01{ BG_CYAN };

	// delta value for fractional distance from center of TEXEL to next closest TEXEL
	// used to assign glyph character for 'shading'
	float delta{ 0 };

	// BOTTOM RIGHT quadrant of texel
	if (dx >= 0.5f && dy >= 0.5f) {
		if (dx > dy) { // RIGHT takes precedence
			c01 = m_colourArray[iy * m_width + (ix + 1) % m_width]; // Wrap around horizontally
			delta = dx - 0.5f;
		}
		else { // DOWN takes precedence
			c01 = m_colourArray[((iy + 1) % m_height) * m_width + ix]; // Wrap around vertically
			delta = dy - 0.5f;
		}
	}
	// TOP RIGHT quadrant of texel
	else if (dx >= 0.5f && dy <= 0.5f) {
		if (1.0f - dx < dy) { // RIGHT takes precedence over UP
			c01 = m_colourArray[iy * m_width + (ix + 1) % m_width]; // Wrap around horizontally
			delta = dx - 0.5f;
		}
		else { // UP takes precedence
			c01 = m_colourArray[((iy - 1 + m_height) % m_height) * m_width + ix]; // Wrap around vertically
			delta = 0.5f - dy;
		}
	}
	// BOTTOM LEFT quadrant of texel
	else if (dx <= 0.5f && dy >= 0.5f) {
		if (dx < 1.0f - dy) { // LEFT takes precedence
			c01 = m_colourArray[iy * m_width + ((ix - 1 + m_width) % m_width)]; // Wrap around horizontally
			delta = 0.5f - dx;
		}
		else { // DOWN takes precedence
			c01 = m_colourArray[((iy + 1) % m_height) * m_width + ix]; // Wrap around vertically
			delta = dy - 0.5f;
		}
	}
	// TOP LEFT quadrant of texel
	else if (dx <= 0.5f && dy <= 0.5f) {
		if (dx < dy) { // LEFT takes precedence over UP
			c01 = m_colourArray[iy * m_width + ((ix - 1 + m_width) % m_width)]; // Wrap around horizontally
			delta = 0.5f - dx;
		}
		else { // UP takes precedence
			c01 = m_colourArray[((iy - 1 + m_height) % m_height) * m_width + ix]; // Wrap around vertically
			delta = 0.5f - dy;
		}
	}

	// default glyph
	short glyph = PIXEL_SOLID;

	// closer to the center of TEXEL (-> 0) more solid glyph
	if (delta < 0.35)
		glyph = PIXEL_SOLID;
	else if (delta < 0.45f)
		glyph = PIXEL_THREEQUARTERS;
	else 
		glyph = PIXEL_HALF;


	// assign fg/bg colors and glyph to CHAR_INFO pixel
	pixel.Attributes = c00 | c01 << 4; // shift bg colour into WORD
	pixel.Char.UnicodeChar = glyph;
}

bool Texture::IsIlluminated() const
{
	return m_illuminated;
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

short Texture::GetWidth() const
{
	return m_width;
}

short Texture::GetHeight() const
{
	return m_height;
}

