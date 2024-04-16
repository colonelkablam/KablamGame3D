#pragma once

#include <stack>
#include <unordered_map>
#include "Texture.h"
#include "UndoRedoManager.h"
#include "ICoordinateStrategy.h"


// Forward declarations to resolve circular dependencies
class BrushstrokeCommand;
class TexturePainter;
class IToolState;

enum class ToolType {
    BRUSH_BLOCK,
    BRUSH_RECT,
    BRUSH_RECT_FILLED,
    BRUSH_LINE,
    BRUSH_COPY
};

class Canvas {
    
    //friend class as will be using this parent classes methods
    friend class BrushstrokeCommand;
    friend class CopyBrushState;

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
    CHAR_INFO cutPixel;

    bool textureSaved;

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

    // data structures for holding brushstroke changes to canvas
    struct TextureChangePixel {
        int x, y; // Position of the change
        short oldGlyph, newGlyph; // Character values before and after the change
        short oldColour, newColour; // Color values before and after the change
    };
    // container of TextureChanges
    struct Brushstroke {
        std::vector<TextureChangePixel> changes;
    };

    // data structures for holding pixel sampled from texture
    struct PixelSample {
        int x, y; // Position of the change
        short glyph; // Character values before and after the change
        short colour; // Color values before and after the change
    };
    // container of TextureChanges
    struct TextureSample {
        int width, height;
        std::vector<PixelSample> pixels;
        void Reset()
        {
            pixels.clear();
        }
    };

    // container for the concrete classes
    std::unordered_map<ToolType, IToolState*> toolStates;
    // current toolState pointer
    IToolState* currentToolState;

    // TRYING OUT SMART POINTERS - 'bout time
    // pointer to the coordinate converter strategy
    std::unique_ptr<ICoordinateStrategy> coordinateStrategy;

    // manage the application of brushStrokes
    UndoRedoManager brushMangager;
    
    // Dependancy Injection from parent - able to draw to screen using the KABLAM engine
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

    bool GetSavedState();

    int GetIllumination();

    int GetZoomLevel();

    void SetZoomLevel(int newZoomLevel);

    void IncreaseZoomLevel();

    void DecreaseZoomLevel();

    int GetTextureWidth();

    int GetTextureHeight();

    int GetBrushSize();

    short GetBrushColour();

    void SetBrushColour(short colour);

    void SetBrushColourAndGlyph(short colour, short glyph);

    void SwitchTool(ToolType type);

    void ToggleCurrentToolOption();

    int ChangeCanvasOffset(COORD change);

    bool AreCoordsWithinCanvas(COORD coords);

    void SetBrushTextureToBackground();

    void ClearCurrentBrushstrokeTexture();

    void HandleLeftMouseClick(COORD mouseCoords);

    void HandleLeftMouseRelease(COORD mouseCoords);

    void ApplyBrushstrokeTextureToBackground(const Brushstroke& stroke);
    
    void ApplyUndoBrushstroke(const Brushstroke& stroke);

    Brushstroke CaptureDifferential();

    TextureSample GrabTextureSample(COORD topLeft, COORD bottomRight);

    void SetBrushToDelete();
    
    void ChangeBrushSize(int sizeChange);

    void PaintPoint(int x, int y);

    void PaintGlyph(int x, int y);

    void PaintLine(int x0, int y0, int x1, int y1, int lineThickness = 1);

    void PaintBlock(int x, int y, int sideLength);

    void PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled = true, int lineWidth = 1);

    void PaintRectangleGlyphs(int x0, int y0, int x1, int y1, bool filled = true, int lineWidth = 1);

    void PaintTextureSample(const TextureSample& sample, COORD topLeft, bool partialSample = false);

    void DrawCanvas();

    void DisplayBrushPointer(COORD);

    void UndoLastCommand();

    void RedoLastCommand();

    int GetSizeUndoStack();

    int GetSizeRedoStack();
};

