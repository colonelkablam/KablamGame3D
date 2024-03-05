
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
	int ix = static_cast<int>(x * (m_width));
	ix = std::min(ix, m_width - 1);          // Clamp to the maximum valid index

	int iy = static_cast<int>(y * (m_height));
	iy = std::min(iy, m_height - 1);          // Clamp to the maximum valid index

	// Calculate the index in the glyph array
	int index = iy * m_width + ix;

	// Return the glyph at the index
	return m_glyphArray[index];
}


// experiment to see if can blend console colours with glyphs

std::map<short, int> Texture::countColorRatios(short c00, short c10, short c01, short c11) {
	std::map<short, int> colorCounts;

	// Increment the count for each color
	colorCounts[c00]++;
	colorCounts[c10]++;
	colorCounts[c01]++;
	colorCounts[c11]++;

	return colorCounts;
}

std::map<short, float> Texture::calculateColorRatios(const std::map<short, int>& colorCounts) {
	std::map<short, float> colorRatios;
	for (const auto& pair : colorCounts) {
		colorRatios[pair.first] = pair.second / 4.0f;  // Dividing by 4, as there are 4 texels
	}
	return colorRatios;
}

void Texture::SampleColourBilinearGlyph(float x, float y, CHAR_INFO &pixel) {
	// Ensure x and y are within the expected range [0.0, 1.0]
	x = std::max(0.0f, std::min(x, 1.0f));
	y = std::max(0.0f, std::min(y, 1.0f));

	// Scale the normalized coordinates to array indices - handle edge cases
	int ix = static_cast<int>(x * (m_width));
	ix = std::min(ix, m_width - 1);          // Clamp to the maximum valid index

	int iy = static_cast<int>(y * (m_height));
	iy = std::min(iy, m_height - 1);          // Clamp to the maximum valid index

	// Fetch the four texels nearest to (x, y)
	short c00 = m_colourArray[iy * m_width + ix];
	short c10 = m_colourArray[iy * m_width + std::min(ix + 1, m_width - 1)];
	short c01 = m_colourArray[std::min(iy + 1, m_height - 1) * m_width + ix];
	short c11 = m_colourArray[std::min(iy + 1, m_height - 1) * m_width + std::min(ix + 1, m_width - 1)];

	auto colourCounts = countColorRatios(c00, c10, c01, c11);
	auto colourRatios = calculateColorRatios(colourCounts);

	// Sort colors by their ratio to find the most and second most dominant colors
	std::vector<std::pair<short, float>> sortedRatios(colourRatios.begin(), colourRatios.end());
	std::sort(sortedRatios.begin(), sortedRatios.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Descending order
		});

	// Select the two most dominant colors for foreground and background
	short fgColour = sortedRatios.size() > 0 ? sortedRatios[0].first : FG_WHITE; // Most dominant
	short bgColour = sortedRatios.size() > 1 ? sortedRatios[1].first : BG_BLACK; // Second most dominant

	// Choose a glyph based on the ratio of the most dominant color
	short glyph;
	float ratio = sortedRatios[0].second; // Ratio of the most dominant color

	std::array<short, 4> glyphs = { PIXEL_QUARTER, PIXEL_HALF, PIXEL_THREEQUARTERS, PIXEL_SOLID };

	int ratioIndex = std::min(static_cast<int>(ratio * 4), 3); // Assuming ratio is normalized to [0, 1]
	glyph = glyphs[ratioIndex];

	pixel.Attributes = fgColour | (bgColour << 4);
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

