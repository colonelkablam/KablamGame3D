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

    BrushType GetBrushType();

    void SetBrushType(BrushType brushType);

    short GetBrushColour();

    void SetBrushColour(short colour);

    void SetBrushColourAndGlyph(short colour, short glyph);

    const Texture* GetTexture();

    COORD GetPositionCoords();

    bool IsMouseWithinCanvas(int x, int y);

    COORD ConvertScreenCoordsToTextureCoords(int x, int y);

    COORD ConvertTextureCoordsToScreenCoords(int x, int y);
   
    void MergeBrushStroke(const Texture* brushStroke);
    
    // apply painting block - able to hold down mouse button
    void ApplyBrushPaint(int x, int y);

    // apply a tool requiring initial click and them secondary click
    void ApplyBrushTool(int x, int y);

    void SetBrushToDelete();

    void ChangeBrushType(BrushType newBrush);
    
    void ChangeBrushSize(int sizeChange);

    void PaintPoint(int x, int y);

    void PaintLine(int x0, int y0, int x1, int y1, int lineThickness = 1);

    void PaintBlock(int x, int y, int sideLength);

    void PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled = true, int lineWidth = 1);

    void DrawCanvas();

    void CreateBrushStroke();

    void IncreaseZoomLevel();
};

