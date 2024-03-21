#pragma once

#include "Texture.h"

class TexturePainter; // forwawrd decleration for DI

class Canvas {

public:
    enum class BrushType {
        BRUSH_BLOCK,
        BRUSH_RECT, // For drawing squares
        BRUSH_RECT_FILLED,
        BRUSH_LINE    // For drawing lines
    };

private:
    static const short STARTING_COLOUR = FG_BLACK;
    static const short STARTING_GLYPH = PIXEL_SOLID;
    static const int START_ZOOM_LEVEL = 1;
    static const int START_BRUSH_SIZE = 1;
    static const BrushType STARTING_BRUSH = BrushType::BRUSH_BLOCK;

    BrushType currentBrushType;
    int brushSize;
    bool initialClick;
    COORD initialClickCoords;
    CHAR_INFO currentPixel;
    CHAR_INFO drawPixel;
    CHAR_INFO deletePixel;

    std::wstring fileName;
    std::wstring filePath;
    int xPos;
    int yPos;
    int zoomLevel;
    Texture* texture;
    Texture* currentBrushStroke;

    TexturePainter& drawingClass;

public:

// constructors.destructors etc
    Canvas(TexturePainter& drawer, Texture* texture, std::wstring fileName, std::wstring filePath, int xPos, int yPos);

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

    BrushType GetBrushType();

    void SetBrushType(BrushType brushType);

    short GetBrushColour();

    void SetBrushColour(short colour);

    const Texture* GetTexture();

    COORD GetPositionCoords();

    bool IsMouseWithinCanvas(int x, int y);

    COORD ConvertScreenCoordsToTextureCoords(int x, int y);

    COORD ConvertTextureCoordsToScreenCoords(int x, int y);
   
    void ApplyBrushStroke(const Texture* brushStroke);

    void ApplyBrush(int x, int y, bool erase = false);

    void ChangeBrushType(BrushType newBrush);
    
    void ChangeBrushSize(int sizeChange);

    void PaintPoint(int x, int y);

    void PaintLine(int x0, int y0, int x1, int y1, int lineThickness = 1);

    void PaintBlock(int x, int y, int sideLength);

    void PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled = true, int lineWidth = 1);

    void DrawCanvas();

    void DisplayBrush();

    void DisplayRectangleOnCanvas(int x0, int y0, int x1, int y1, short colour, bool filled = false, short glyph = PIXEL_SOLID, int lineWidth = 1);

    void DisplayLineOnCanvas(int x0, int y0, int x1, int y1, short colour, short glyph = PIXEL_SOLID, int lineWidth = 1);

    void DisplayBlockOnCanvas(int x, int y, int size, short colour, short glyph);

    void DisplayPixelOnCanvas(int x, int y, short colour, short glyph);

    void IncreaseZoomLevel();
};

