#pragma once

#include "Texture.h"


class Canvas {

public:
    enum class BrushType {
        BRUSH_POINT,  // For single pixel drawing
        BRUSH_SQUARE, // For drawing squares
        BRUSH_LINE    // For drawing lines
    };

private:
    static const short startingColour = FG_WHITE;
    static const short startingGlyph = PIXEL_SOLID;
    static const int startingZoomLevel = 1;
    static const int startingBrushSize = 1;

    static const BrushType startingBrush = BrushType::BRUSH_POINT;

    BrushType currentBrushType;
    int brushSize;
    short currentColour;
    short currentGlyph;
    CHAR_INFO currentPixel;
    CHAR_INFO drawPixel;
    CHAR_INFO deletePixel;

    std::wstring fileName;
    std::wstring filePath;
    int xPos;
    int yPos;
    int zoomLevel;
    Texture* texture;

public:

// constructors.destructors etc
    Canvas(Texture* texture, std::wstring fileName, std::wstring filePath, int xPos, int yPos);

    ~Canvas();

// instance methods

    bool SaveTexture(const std::wstring& filePath);

    bool LoadTexture(const std::wstring& filePath);

    const std::wstring& GetFileName();

    const std::wstring& GetFilePath();

    int GetIllumination();

    int GetZoomLevel();

    int GetTextureWidth();

    int GetTextureHeight();

    int GetBrushSize();

    int GetBrushTypeInt();

    short GetBrushColour();

    void SetBrushColour(short colour);

    const Texture* GetTexture();

    COORD GetPositionCoords();

    bool IsMouseWithinCanvas(int x, int y);

    COORD ConvertMouseCoordsToTextureCoords(int x, int y);

    void ApplyBrush(int x, int y, bool erase = false);

    void ChangeBrushType(BrushType newBrush);
    
    void ChangeBrushSize(int sizeChange);

    void PaintPoint(int x, int y);

    void PaintLine(int x0, int y0, int x1, int y1);

    void PaintSquare(int x, int y, int sideLength, bool filled = true, int lineWidth = 1);

    void IncreaseZoomLevel();
};

