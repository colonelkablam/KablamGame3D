#include <Windows.h>

// undefine the macros from windows.h
#undef max
#undef min

// need to include full header here to use class methods from DI
#include "TexturePainter.h"
// need to #include Texture.h in Canvas.h before here

#include "Canvas.h"
#include "BrushStrokeCommand.h"

Canvas::Canvas(TexturePainter& drawer, Texture* tex, std::wstring fn, std::wstring fp, short x, short y)
    : drawingClass{ drawer }, backgroundTexture{ tex }, currentBrushStrokeTexture{ nullptr }, fileName{ fn }, filePath{ fp }, topLeft{ x, y },
        bottomRight{ 0, 0 }, initialClick{false}, initialClickCoords{0, 0}, canvasViewOffset{0,0}
{
    currentBrushType = STARTING_BRUSH;
    brushSize = START_BRUSH_SIZE;
    zoomLevel = START_ZOOM_LEVEL;

    currentPixel = { STARTING_GLYPH, STARTING_COLOUR };
    deletePixel = { L'X', 0 };

    //create first brush stroke
    currentBrushStrokeTexture = new Texture{ backgroundTexture->GetWidth(), backgroundTexture->GetHeight() };


    bottomRight = { static_cast<short>(x + TexturePainter::CANVAS_DISPLAY_WIDTH), static_cast<short>(y + TexturePainter::CANVAS_DISPLAY_HEIGHT) };

}

Canvas::~Canvas()
{
    delete backgroundTexture;
}

bool Canvas::SaveTexture(const std::wstring& filePath)
{
    if (backgroundTexture->SaveAs(filePath))
        return true;
    else
        return false;
}

bool Canvas::LoadTexture(const std::wstring& filePath)
{
    if (backgroundTexture->LoadFrom(filePath))
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
    return backgroundTexture->GetIllumination();
}

int Canvas::GetZoomLevel()
{
    return zoomLevel;
}

int Canvas::GetTextureWidth()
{
    return backgroundTexture->GetWidth();
}

int Canvas::GetTextureHeight()
{
    return backgroundTexture->GetHeight();
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

// constant texture pointer for texture painer to draw to screen
const Texture* Canvas::GetTexture()
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

    int canvasWidth = backgroundTexture->GetWidth();
    int canvasHeight = backgroundTexture->GetHeight();

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

COORD Canvas::ConvertScreenCoordsToTextureCoords(int x, int y)
{
    COORD coords{ (x - topLeft.X) / zoomLevel + canvasViewOffset.X, (y - topLeft.Y) / zoomLevel + canvasViewOffset.Y };
    return coords;
}

COORD Canvas::ConvertTextureCoordsToScreenCoords(int x, int y)
{
    COORD screenCoords{};

    // reverse transformation used in ConvertMouseCoordsToTextureCoords
    // Consider the canvas position (xPos, yPos) and the zoom level
    screenCoords.X = topLeft.X + (x * zoomLevel);
    screenCoords.Y = topLeft.Y + (y * zoomLevel);

    return screenCoords;
}

Texture* Canvas::MergeBrushStroke(const Texture* brushStroke)
{
    Texture* undoTexture{ nullptr };
    undoTexture = backgroundTexture->MergeOther(brushStroke);
    return undoTexture;
}

void Canvas::ApplyBrushPaint(int x, int y)
{
    COORD coords = ConvertScreenCoordsToTextureCoords(x, y);


    switch (currentBrushType) {
    case BrushType::BRUSH_BLOCK:
        /*BrushStrokeCommand* newStroke = new BrushStrokeCommand(*this, new BrushStroke(0, 0, currentBrushStrokeTexture));
        brushMangager.performAction(newStroke);
        currentBrushStrokeTexture = new Texture(backgroundTexture->GetWidth(), backgroundTexture->GetHeight());
        break;*/

        if (!initialClick) {
            currentBrushStrokeTexture(currentBrushStrokeTexture);
        }
        else
        {

        }
        break;
    }
}

void Canvas::ApplyBrushTool(int x, int y)
{
    COORD coords = ConvertScreenCoordsToTextureCoords(x, y);

    // currently all have same logic
    switch (currentBrushType) {
    case BrushType::BRUSH_RECT:
    case BrushType::BRUSH_RECT_FILLED:
    case BrushType::BRUSH_LINE:
        if (!initialClick) {
            // Store the initial click position
            initialClickCoords = coords;
            initialClick = true;
        }
        else {
            initialClick = false;
            // Apply the brush stroke from the initial click position to the current position
            // brushStroke being prepared in CreateBrushStroke()
            MergeBrushStroke(currentBrushStrokeTexture);
        }
        break;
    }
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
    currentBrushStrokeTexture->SetPixel(x, y, currentPixel);
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
    drawingClass.WriteStringToBuffer(topLeft.X - 1, topLeft.Y - 3, L"ZOOM: " + std::to_wstring(zoomLevel) + L'. ');
    drawingClass.WriteStringToBuffer(topLeft.X + 8, topLeft.Y - 3, L"X: -   Y: -");

    COORD mouseCoords = drawingClass.GetMousePosition();

    // if coords withing canvas diplay (index starting from 1 for user)
    if (IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        COORD textureCoords = ConvertScreenCoordsToTextureCoords(mouseCoords.X, mouseCoords.Y);
        drawingClass.WriteStringToBuffer(topLeft.X + 11, topLeft.Y - 3, std::to_wstring(textureCoords.X + 1));
        drawingClass.WriteStringToBuffer(topLeft.X + 18, topLeft.Y - 3, std::to_wstring(textureCoords.Y + 1));
    }

    drawingClass.WriteStringToBuffer(topLeft.X + 11, topLeft.Y - 4, std::to_wstring(canvasViewOffset.X) + L' ' + std::to_wstring(canvasViewOffset.Y));

    // add texture to screen buffer
    drawingClass.DrawSectionOfTextureToScreen(backgroundTexture, topLeft.X, topLeft.Y, 
                                                canvasViewOffset.X, 
                                                canvasViewOffset.Y, 
                                                TexturePainter::CANVAS_DISPLAY_WIDTH/zoomLevel + canvasViewOffset.X - 1, 
                                                TexturePainter::CANVAS_DISPLAY_HEIGHT/zoomLevel + canvasViewOffset.Y - 1,
                                                zoomLevel);

    // if brushStroke object exists, draw it to screen
    if (currentBrushStrokeTexture != nullptr)
        drawingClass.DrawPartialTextureToScreen(currentBrushStrokeTexture, 
                                                topLeft.X - canvasViewOffset.X * zoomLevel, 
                                                topLeft.Y - canvasViewOffset.Y * zoomLevel,
                                                zoomLevel);

    // this is the current brush stroke texture that will be laid ontop of base
    CreateBrushStroke();

    // draw border around maximum display panel
    drawingClass.DrawRectangleEdgeLength(topLeft.X - 2, topLeft.Y - 2, TexturePainter::CANVAS_DISPLAY_WIDTH + 4, TexturePainter::CANVAS_DISPLAY_HEIGHT + 4, FG_RED, false, PIXEL_HALF);

}

void Canvas::CreateBrushStroke()
{
    // get appropriate coords for drawing to screen - will dynamically calc these
    COORD mouseCoords = ConvertScreenCoordsToTextureCoords(drawingClass.GetMousePosition().X, drawingClass.GetMousePosition().Y);

    if (currentBrushStrokeTexture != nullptr)
        currentBrushStrokeTexture->Clear();

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



