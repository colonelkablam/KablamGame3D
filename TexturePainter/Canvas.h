#pragma once

#include <stack>

#include "Texture.h"

#include "UndoRedoManager.h"

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
    bool liftedClick;

    CHAR_INFO currentPixel;
    CHAR_INFO deletePixel;

    std::wstring fileName;
    std::wstring filePath;

    // position in screen window
    COORD topLeft;
    COORD bottomRight;

    // canvas view
    int zoomLevel;
    COORD canvasViewOffset;

    // main canvas texture
    Texture backgroundTexture;
    // texture of brushStroke
    Texture currentBrushStrokeTexture;

    // manage the application of brushStrokes
    UndoRedoManager brushMangager;
    
    // DI from parent
    TexturePainter& drawingClass;

public:

// constructors.destructors etc
    Canvas(TexturePainter& drawer, Texture& texture, std::wstring fileName, std::wstring filePath, short xPos, short yPos);

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

    void SetBrushType(BrushType brushType);

    short GetBrushColour();

    void SetBrushColour(short colour);

    void SetBrushColourAndGlyph(short colour, short glyph);

    const Texture* GetTexture();

    COORD GetPositionCoords();

    int ChangeCanvasOffset(COORD change);

    bool IsMouseWithinCanvas(int x, int y);

    void LeftButtonReleased();

    COORD ConvertScreenCoordsToTextureCoords(int x, int y);

    COORD ConvertTextureCoordsToScreenCoords(int x, int y);
   
    // apply painting block - able to hold down mouse button
    void ApplyPaint(int x, int y);

    void SetPaint();

    // apply a tool requiring initial click and them secondary click
    void ApplyTool(int x, int y);

    Texture* MergeTexture(Texture* other, bool treatSpacesAsValid);

    void SetBrushToDelete();

    void ChangeBrushType(BrushType newBrush);
    
    void ChangeBrushSize(int sizeChange);

    void PaintPoint(int x, int y);

    void PaintLine(int x0, int y0, int x1, int y1, int lineThickness = 1);

    void PaintBlock(int x, int y, int sideLength);

    void PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled = true, int lineWidth = 1);

    void DrawCanvas();

    void DisplayBrushStroke();

    void IncreaseZoomLevel();

    void UndoLastCommand();

    int GetSizeUndoStack();

    int GetSizeRedoStack();
};

