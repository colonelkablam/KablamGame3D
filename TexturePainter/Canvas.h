#pragma once

#include <stack>
#include <unordered_map>
#include "Texture.h"
#include "UndoRedoManager.h"

// Forward declarations to resolve circular dependencies
class BrushstrokeCommand;
class TexturePainter;
class ToolState;

enum class ToolType {
    BRUSH_BLOCK,
    BRUSH_RECT,
    BRUSH_RECT_FILLED,
    BRUSH_LINE
};

class Canvas {

    friend class BrushstrokeCommand;
    //friend class Concrete

private:
    static const short STARTING_COLOUR = FG_BLACK;
    static const short STARTING_GLYPH = PIXEL_SOLID;
    static const int START_ZOOM_LEVEL = 1;
    static const int START_BRUSH_SIZE = 1;
    static const ToolType STARTING_TOOL = ToolType::BRUSH_BLOCK;

    ToolType currentBrushType;
    int brushSize;
    bool initialClick;
    COORD initialClickCoords;

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

    struct TextureChange {
        int x, y; // Position of the change
        short oldGlyph, newGlyph; // Character values before and after the change
        short oldColor, newColor; // Color values before and after the change
    };

    struct Brushstroke {
        std::vector<TextureChange> changes;
    };

    // container for the concrete classes
    std::unordered_map<ToolType, ToolState*> toolStates;
    // current pointer
    ToolState* currentToolState;

    // manage the application of brushStrokes
    UndoRedoManager brushMangager;
    
    // DI from parent
    TexturePainter& drawingClass;

    // private initialiser methods
    void Initialise(const std::wstring& saveFolder, const std::wstring& fileName);
    void PostInitialise();

public:
// constructors.destructors etc
    Canvas(TexturePainter& drawer, int width, int height, int illumination, const std::wstring& fileName, const std::wstring& filePath, short xPos, short yPos);

    Canvas(TexturePainter& drawer, const std::wstring& saveFolder, const std::wstring& fileName, short xPos, short yPos);

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

    short GetBrushColour();

    void SetBrushColour(short colour);

    void SetBrushColourAndGlyph(short colour, short glyph);

    void SwitchTool(ToolType type);

    int ChangeCanvasOffset(COORD change);

    bool AreCoordsWithinCanvas(int x, int y);

    COORD ConvertScreenCoordsToTextureCoords(int x, int y);
    
    COORD ConvertTextureCoordsToScreenCoords(int x, int y);

    void SetBrushTextureToBackground();

    void ClearCurrentBrushStrokeTexture();

    void ApplyToolToBrushTexture(int x, int y);

    void ApplyBrushstroke(const Brushstroke& stroke);

    void ApplyUndoBrushstroke(const Brushstroke& stroke);

    Brushstroke CaptureDifferential();

    void SetBrushToDelete();
    
    void ChangeBrushSize(int sizeChange);

    void PaintPoint(int x, int y);

    void PaintLine(int x0, int y0, int x1, int y1, int lineThickness = 1);

    void PaintBlock(int x, int y, int sideLength);

    void PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled = true, int lineWidth = 1);

    void DrawCanvas();

    void DisplayBrushPointer(int x, int y);

    void IncreaseZoomLevel();

    void UndoLastCommand();

    void RedoLastCommand();

    int GetSizeUndoStack();

    int GetSizeRedoStack();
};

