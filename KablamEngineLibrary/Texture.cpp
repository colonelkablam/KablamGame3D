
#include "pch.h"
#include "Texture.h"


// undefine the macros from windows.h
#undef max
#undef min

Texture::Texture(int w, int h, bool illum)
	: m_colourArray{ nullptr }, m_glyphArray{ nullptr }, topMipmap{ nullptr }
{
	Initialise(w, h, illum);

	GenerateMipmaps();
}

Texture::Texture(std::wstring filePath)
	: m_colourArray{ nullptr }, m_glyphArray{ nullptr }, topMipmap{ nullptr }
{
	if (!LoadFrom(filePath))
	{
		// if failed to load make a 32*32 dark red canvas
		Initialise(32, 32, false, FG_DARK_MAGENTA);
	}
	GenerateMipmaps();
}

// copy constructor TBC
//Texture::Texture(Texture& source)
//{
//}

Texture::~Texture()
{
	delete[] m_colourArray;
	delete[] m_glyphArray;

	MipmapLevel* current = topMipmap;
	while (current != nullptr) {
		MipmapLevel* next = current->next;
		delete current;
		current = next;
	}
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

void Texture::GenerateMipmaps() {

	topMipmap = new MipmapLevel(m_width, m_height);

	topMipmap->colourArray = m_colourArray;
	topMipmap->glyphArray = m_glyphArray;
	topMipmap->height = m_height;
	topMipmap->width = m_width;
	topMipmap->next = nullptr;

	if (!topMipmap) return;  // Ensure the top level exists

	MipmapLevel* currentLevel = topMipmap;
	while (currentLevel->width > 1 && currentLevel->height > 1) {
		int nextWidth = currentLevel->width / 2;
		int nextHeight = currentLevel->height / 2;

		MipmapLevel* nextLevel = new MipmapLevel(nextWidth, nextHeight);
		// Populate nextLevel->colourArray and nextLevel->glyphArray by downsampling from currentLevel


		Downsample(currentLevel, nextLevel);

		currentLevel->next = nextLevel;  // Link to the new level
		currentLevel = nextLevel;  // Move down to the new level
	}
}

void Texture::Downsample(MipmapLevel* currentLevel, MipmapLevel* nextLevel) {
	int currentWidth = currentLevel->width;
	int nextWidth = nextLevel->width;
	int nextHeight = nextLevel->height;

	for (int y = 0; y < nextHeight; ++y) {
		for (int x = 0; x < nextWidth; ++x) {
			// Corresponding top-left pixel in the current level
			int topLeftX = x * 2;
			int topLeftY = y * 2;

			// Sample 2x2 block from current level
			short colours[4];
			for (int dy = 0; dy < 2; ++dy) {
				for (int dx = 0; dx < 2; ++dx) {
					int index = (topLeftY + dy) * currentWidth + (topLeftX + dx);
					colours[dy * 2 + dx] = currentLevel->colourArray[index];
				}
			}

			// Compute averages for colour and glyph. Note: averaging glyphs may not always give desired visual results.
			short avgColour = AverageColour(colours);

			// Set averaged colour and glyph in the next level
			nextLevel->colourArray[y * nextWidth + x] = avgColour;
		}
	}
}

short Texture::AverageColour(short colours[4]) {
	// Example averaging function for colours
	int sum = 0;
	for (int i = 0; i < 4; ++i) {
		sum += colours[i];
	}
	return static_cast<short>(sum / 4);
}

Texture::MipmapLevel* Texture::GetMipmapLevel(float detail) const {
	MipmapLevel* currentLevel = topMipmap;
	int level = 0;

	// Simple heuristic to determine mipmap level based on 'detail'
	// 'detail' might represent distance from viewer or some other metric
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
	topMipmap = nullptr;

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

// MIPMAPPING
short Texture::SampleColourWithMipmap(float x, float y, float detail) const {
	MipmapLevel* level = GetMipmapLevel(detail);  // Determine appropriate mipmap level based on detail

	// Scale x, y to level dimensions and clamp
	x = std::max(0.0f, std::min(x, 1.0f)) * level->width;
	y = std::max(0.0f, std::min(y, 1.0f)) * level->height;

	int ix = std::min(static_cast<int>(x), level->width - 1);
	int iy = std::min(static_cast<int>(y), level->height - 1);

	// Return colour from the selected mipmap level
	return level->colourArray[iy * level->width + ix];
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

void Texture::LinearInterpolationWithGlyphShading(float x, float y, CHAR_INFO& pixel) {
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// get decimal value of the relative position in texture grid
	float fx = x * m_width, fy = y * m_height;

	// get grid index (integer part) of TEXEL to be sampled
	int ix = static_cast<int>(fx) % m_width, iy = static_cast<int>(fy) % m_height;

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
	pixel.Attributes = c00 | (c01 << 4);
	pixel.Char.UnicodeChar = glyph;
}


bool Texture::IsIlluminated() const
{
	return m_illuminated;
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

