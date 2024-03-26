#include <Windows.h>

// undefine the macros from windows.h
#undef max
#undef min

// need to include full header here to use class methods from DI
#include "TexturePainter.h"
#include "Canvas.h"
#include "BrushstrokeCommand.h"


// Constructor with dimensions and illumination
Canvas::Canvas(TexturePainter& drawer, int width, int height, int illumination, const std::wstring& saveFolder, const std::wstring& fileName, short xPos, short yPos)
    : drawingClass{ drawer },
    // Directly use the appropriate Texture constructor
    backgroundTexture{ width, height, illumination },
    topLeft{ xPos, yPos }
{
    Initialise(saveFolder, fileName);

    currentBrushStrokeTexture = Texture{ backgroundTexture.GetWidth(), backgroundTexture.GetHeight(),  0};
}

// Constructor with texture loading from file
Canvas::Canvas(TexturePainter& drawer, const std::wstring& saveFolder, const std::wstring& fileName, short xPos, short yPos)
    : drawingClass{ drawer },
    topLeft{ xPos, yPos },
    // Use an empty initializer to satisfy the compiler
    backgroundTexture{} 
{
    std::wstring fullPath = saveFolder + fileName;
    // Attempt to load the texture;
    if (!backgroundTexture.LoadFrom(fullPath)) {
        // Fallback initialization
        backgroundTexture = Texture{};
    }
    Initialise(saveFolder, fileName);

    currentBrushStrokeTexture = Texture{ backgroundTexture.GetWidth(), backgroundTexture.GetHeight(), 0 };
}

// common initialisation handled here
void Canvas::Initialise(const std::wstring& saveFolder, const std::wstring& fileName) {
    this->fileName = fileName;
    filePath = saveFolder + this->fileName;
    currentBrushType = STARTING_BRUSH;
    brushSize = START_BRUSH_SIZE;
    zoomLevel = START_ZOOM_LEVEL;
    currentPixel = { STARTING_GLYPH, STARTING_COLOUR };
    deletePixel = { L'X', FG_WHITE };
    bottomRight = { static_cast<short>(topLeft.X + TexturePainter::CANVAS_DISPLAY_WIDTH),
                    static_cast<short>(topLeft.Y + TexturePainter::CANVAS_DISPLAY_HEIGHT) };
}

bool Canvas::SaveTexture(const std::wstring& filePath)
{
    if (backgroundTexture.SaveAs(filePath))
        return true;
    else
        return false;
}

bool Canvas::LoadTexture(const std::wstring& filePath)
{
    if (backgroundTexture.LoadFrom(filePath))
        return true;
    else
        return false;
}

const std::wstring& Canvas::GetFileName()
{
    return fileName;
}

const std::wstring& Canvas::GetFilePath()
{
    return filePath;
}

int Canvas::GetIllumination()
{
    return backgroundTexture.GetIllumination();
}

int Canvas::GetZoomLevel()
{
    return zoomLevel;
}

int Canvas::GetTextureWidth()
{
    return backgroundTexture.GetWidth();
}

int Canvas::GetTextureHeight()
{
    return backgroundTexture.GetHeight();
}

int Canvas::GetBrushSize()
{
    return brushSize;
}

void Canvas::SetBrushType(BrushType newBrushType)
{
    initialClick = false;
    currentBrushType = newBrushType;
}

Canvas::BrushType Canvas::GetBrushType()
{
    return currentBrushType;
}


short Canvas::GetBrushColour()
{
    return currentPixel.Attributes;
}

void Canvas::SetBrushColour(short newColour)
{   
    currentPixel.Attributes = newColour;

    // also reset glyph to solid_pixel
    currentPixel.Char.UnicodeChar = PIXEL_SOLID;
}

void Canvas::SetBrushColourAndGlyph(short colour, short glyph)
{
    currentPixel.Attributes = colour;
    currentPixel.Char.UnicodeChar = glyph;
}

void Canvas::SetBrushToDelete()
{
        currentPixel = deletePixel;
}

// constant texture pointer for texture painter to draw to screen
const Texture& Canvas::GetTexture()
{
    return backgroundTexture;
}

COORD Canvas::GetPositionCoords()
{
    return topLeft;
}

int Canvas::ChangeCanvasOffset(COORD change)
{
    // Update the canvasOffset with the change
    canvasViewOffset.X += change.X;
    canvasViewOffset.Y += change.Y;

    int canvasWidth = backgroundTexture.GetWidth();
    int canvasHeight = backgroundTexture.GetHeight();

    // Clamp canvasOffset.X to be within 0 and canvasWidth
    if (canvasViewOffset.X < 0)
        canvasViewOffset.X = 0;
    else if (canvasViewOffset.X > canvasWidth  - 1)
        canvasViewOffset.X = canvasWidth - 1;

    // Clamp canvasOffset.Y to be within 0 and canvasHeight
    if (canvasViewOffset.Y < 0)
        canvasViewOffset.Y = 0;
    else if (canvasViewOffset.Y > canvasHeight - 1)
        canvasViewOffset.Y = canvasHeight - 1;

    // Return 0 for success  - might add feedback if hitting edge
    return 0;
}

bool Canvas::IsMouseWithinCanvas(int x, int y)
{
    // Check if the mouse coordinates are within the zoom-adjusted canvas boundaries
    if (x >= topLeft.X && x < bottomRight.X * zoomLevel && y >= topLeft.Y && y < bottomRight.Y * zoomLevel)
        return true;
    else
        return false;
}

void Canvas::LeftButtonReleased()
{
    liftedClick = true;
}

COORD Canvas::ConvertScreenCoordsToTextureCoords(int x, int y)
{
    COORD coords{ (x - topLeft.X) / zoomLevel + canvasViewOffset.X, (y - topLeft.Y) / zoomLevel + canvasViewOffset.Y };
    return coords;
}

void Canvas::ApplyPaint(int x, int y)
{
    COORD coords = ConvertScreenCoordsToTextureCoords(x, y);

    switch (currentBrushType) {
    case BrushType::BRUSH_BLOCK:
        // apply block to currentBrushStrokeTexture - will set to backgroundTexture when release button
        PaintBlock(coords.X, coords.Y, brushSize);
        break;
    }
}

// when left button lifted
void Canvas::SetPaint()
{
    // create a new brushstroke to add to the brushManager
    BrushstrokeCommand* newStroke = new BrushstrokeCommand(*this, CaptureDifferential());
    brushMangager.PerformAction(newStroke);

    // clear to start new brushstroke
    currentBrushStrokeTexture.Clear();
}

void Canvas::ApplyTool(int x, int y)
{
    // clear brushstroke texture as will be updated later
    currentBrushStrokeTexture.Clear();

    COORD mouseClick = ConvertScreenCoordsToTextureCoords(x, y);

    // currently all have same logic
    switch (currentBrushType) {
    case BrushType::BRUSH_RECT:
    case BrushType::BRUSH_RECT_FILLED:
    case BrushType::BRUSH_LINE:
        if (!initialClick) {
            // Store the initial click position
            initialClickCoords = mouseClick;
            initialClick = true;
            PaintLine(initialClickCoords.X, initialClickCoords.Y, mouseClick.X, mouseClick.Y, brushSize);

        }
        else {
            initialClick = false;
            // apply line to currentBrushStrokeTexture - will set to backgroundTexture when release button
            PaintLine(initialClickCoords.X, initialClickCoords.Y, mouseClick.X, mouseClick.Y, brushSize);

           //(currentBrushStrokeTexture);
        }
        break;
    }
}

void Canvas::ApplyBrushstroke(const Brushstroke& stroke) 
{
    for (const auto& change : stroke.changes) {
        // Set the glyph and color at the specified position to their new values
        backgroundTexture.SetGlyph(change.x, change.y, change.newGlyph);
        backgroundTexture.SetColour(change.x, change.y, change.newColor);
    }
}

void Canvas::UndoBrushstroke(const Brushstroke& stroke)
{
    for (const auto& change : stroke.changes) {
        // Set the glyph and color at the specified position to their new values
        backgroundTexture.SetGlyph(change.x, change.y, change.oldGlyph);
        backgroundTexture.SetColour(change.x, change.y, change.oldColor);
    }
}

Canvas::Brushstroke Canvas::CaptureDifferential() {
    Canvas::Brushstroke stroke;

    int width = backgroundTexture.GetWidth();
    int height = backgroundTexture.GetHeight();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Capture the glyphs and colours from both textures
            short bgGlyph = backgroundTexture.GetGlyph(x, y);
            short cbstGlyph = currentBrushStrokeTexture.GetGlyph(x, y);
            short bgColour = backgroundTexture.GetColour(x, y);
            short cbstColour = currentBrushStrokeTexture.GetColour(x, y);

            // Check if there is a difference and the glyph in the brush stroke is not a space
            if ((bgGlyph != cbstGlyph || bgColour != cbstColour) && cbstGlyph != L' ') {
                stroke.changes.push_back(TextureChange{
                    x, y,
                    bgGlyph,
                    cbstGlyph,
                    bgColour,
                    cbstColour
                    });
            }
        }
    }
    return stroke;
}



void Canvas::ChangeBrushType(BrushType newBrush)
{
    currentBrushType = newBrush;
}

void Canvas::ChangeBrushSize(int sizeChange)
{
    int newSize = (brushSize - 1 + sizeChange + 5) % 5 + 1;
    brushSize = newSize;
}

void Canvas::PaintPoint(int x, int y)
{
    currentBrushStrokeTexture.SetPixel(x, y, currentPixel);
}

// Bresenham's line algorithm
void Canvas::PaintLine(int x0, int y0, int x1, int y1, int lineThickness)
{
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; // error value e_xy

    while (true) {
        PaintBlock(x0, y0, brushSize); // Use DisplayBlockOnCanvas to draw a block at each point

        if (x0 == x1 && y0 == y1) break; // Check if the end point is reached
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Canvas::PaintBlock(int x, int y, int sideLength)
{
    for (size_t i{ 0 }; i < sideLength; i++)
        for (size_t j{ 0 }; j < sideLength; j++)
            PaintPoint(x + i, y + j);

}

void Canvas::PaintRectangleCoords(int x0, int y0, int x1, int y1, bool filled, int lineWidth) {
    // Normalize coordinates
    int left = std::min(x0, x1);
    int top = std::min(y0, y1);
    int right = std::max(x0, x1);
    int bottom = std::max(y0, y1);

    if (filled) {
        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                PaintPoint(x, y);
            }
        }
    }
    else {
        // Top and bottom sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int x = left; x <= right; ++x) {
                PaintPoint(x, top + i); // Top side
                PaintPoint(x, bottom - i); // Bottom side
            }
        }
        // Left and right sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int y = top; y <= bottom; ++y) {
                PaintPoint(left + i, y); // Left side
                PaintPoint(right - i, y); // Right side
            }
        }
    }
}

void Canvas::DrawCanvas()
{
    drawingClass.WriteStringToBuffer(topLeft.X, topLeft.Y - 3, L"ZOOM: " + std::to_wstring(zoomLevel) + L'. ');
    drawingClass.WriteStringToBuffer(topLeft.X + 8, topLeft.Y - 3, L"X: -   Y: -");

    COORD mouseCoords = drawingClass.GetMousePosition();

    // if coords withing canvas diplay (index starting from 1 for user)
    if (IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        COORD textureCoords = ConvertScreenCoordsToTextureCoords(mouseCoords.X, mouseCoords.Y);
        drawingClass.WriteStringToBuffer(topLeft.X + 11, topLeft.Y - 3, std::to_wstring(textureCoords.X + 1));
        drawingClass.WriteStringToBuffer(topLeft.X + 18, topLeft.Y - 3, std::to_wstring(textureCoords.Y + 1));
    }

    // currently drawing offset for debugging
    drawingClass.WriteStringToBuffer(topLeft.X, topLeft.Y - 2, std::to_wstring(canvasViewOffset.X) + L' ' + std::to_wstring(canvasViewOffset.Y));

    // draw non texture background
    drawingClass.DrawRectangleEdgeLength(topLeft.X, topLeft.Y, TexturePainter::CANVAS_DISPLAY_WIDTH, TexturePainter::CANVAS_DISPLAY_HEIGHT, FG_BLUE, true, L'.');

    // add texture to screen buffer
    drawingClass.DrawSectionOfTextureToScreen(backgroundTexture, topLeft.X, topLeft.Y, 
                                                canvasViewOffset.X, 
                                                canvasViewOffset.Y, 
                                                TexturePainter::CANVAS_DISPLAY_WIDTH/zoomLevel + canvasViewOffset.X - 1, 
                                                TexturePainter::CANVAS_DISPLAY_HEIGHT/zoomLevel + canvasViewOffset.Y - 1,
                                                zoomLevel,
                                                true);

    // draw brush stroke as it is made
    drawingClass.DrawPartialTextureToScreen(currentBrushStrokeTexture, 
                                                topLeft.X - canvasViewOffset.X * zoomLevel, 
                                                topLeft.Y - canvasViewOffset.Y * zoomLevel,
                                                zoomLevel);

    // draw border around maximum display panel
    drawingClass.DrawRectangleEdgeLength(topLeft.X - 1, topLeft.Y - 1, TexturePainter::CANVAS_DISPLAY_WIDTH + 2, TexturePainter::CANVAS_DISPLAY_HEIGHT + 2, FG_RED, false, PIXEL_HALF);

    DisplayBrushStroke(drawingClass.GetMousePosition().X, drawingClass.GetMousePosition().Y);
}

void Canvas::DisplayBrushStroke(int x, int y)
{
    // get appropriate coords for drawing to screen - will dynamically calc these
    COORD mouseCoords = ConvertScreenCoordsToTextureCoords(x, y);

    //// if mouse not initially pressed down keep texture clear
    //if (initialClick == false)
       currentBrushStrokeTexture.Clear();



    // draw appropriate brush
    switch (currentBrushType) {
    case BrushType::BRUSH_BLOCK:
        PaintBlock(mouseCoords.X, mouseCoords.Y, brushSize);
        break;
    case BrushType::BRUSH_RECT:
        if (initialClick)
        {
            PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, false, brushSize);
        }
        break;
    case BrushType::BRUSH_RECT_FILLED:
        if (initialClick)
        {
            PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, true);
        }
        break;
    case BrushType::BRUSH_LINE:
        if (initialClick)
        {
            PaintLine(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, brushSize);
        }
        break;
    }
}

void Canvas::IncreaseZoomLevel()
{
    zoomLevel = (zoomLevel % 3) + 1;
}

void Canvas::UndoLastCommand()
{
    brushMangager.Undo();
}

int Canvas::GetSizeUndoStack()
{
    return brushMangager.GetUndoStackSize();
}

int Canvas::GetSizeRedoStack()
{
    return brushMangager.GetRedoStackSize();

}



