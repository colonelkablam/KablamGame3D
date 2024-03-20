#include <Windows.h>

// undefine the macros from windows.h
#undef max
#undef min

// need to include full header here to use class methods from DI
#include "TexturePainter.h"
// need to #include Texture.h in Canvas.h before here
#include "Canvas.h"


Canvas::Canvas(TexturePainter& drawer, Texture* tex, std::wstring fn, std::wstring fp, int x, int y)
    : drawingClass{ drawer }, texture{ tex }, fileName{ fn }, filePath{ fp }, xPos{ x }, yPos{ y },
    initialClick{ false }, initialClickCoords{ 0, 0 }
{
    currentBrushType = STARTING_BRUSH;
    brushSize = START_BRUSH_SIZE;
    zoomLevel = START_ZOOM_LEVEL;

    currentPixel = { STARTING_GLYPH, STARTING_COLOUR };
    drawPixel = { STARTING_GLYPH, STARTING_COLOUR };
    deletePixel = { L' ', 0 };
}

Canvas::~Canvas()
{
    delete texture;
}


bool Canvas::SaveTexture(const std::wstring& filePath)
{
    if (texture->SaveAs(filePath))
        return true;
    else
        return false;
}

bool Canvas::LoadTexture(const std::wstring& filePath)
{
    if (texture->LoadFrom(filePath))
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
    return texture->GetIllumination();
}

int Canvas::GetZoomLevel()
{
    return zoomLevel;
}
int Canvas::GetTextureWidth()
{
    return texture->GetWidth();
}

int Canvas::GetTextureHeight()
{
    return texture->GetHeight();
}

int Canvas::GetBrushSize()
{
    return brushSize;
}

int Canvas::GetBrushTypeInt()
{
    return static_cast<int>(currentBrushType);
}

void Canvas::SetBrushType(BrushType newBrushType)
{
    currentBrushType = newBrushType;
}


short Canvas::GetBrushColour()
{
    return drawPixel.Attributes;
}

void Canvas::SetBrushColour(short newColour)
{
    drawPixel.Attributes = newColour;
}

// constant texture pointer for texture painer to draw to screen
const Texture* Canvas::GetTexture()
{
    return texture;
}

COORD Canvas::GetPositionCoords()
{
    COORD coords{ (short)xPos, (short)yPos };
    return coords;
}

bool Canvas::IsMouseWithinCanvas(int x, int y)
{
    // Check if the mouse coordinates are within the zoom-adjusted canvas boundaries
    if (x >= xPos && x < xPos + texture->GetWidth() * zoomLevel && y >= yPos && y < yPos + texture->GetHeight() * zoomLevel)
        return true;
    else
        return false;
}

COORD Canvas::ConvertMouseCoordsToTextureCoords(int x, int y)
{
    COORD coords{};

    if (IsMouseWithinCanvas(x, y))
    {
        // Correct calculation for both X and Y coordinates
        coords.X = (x - xPos) / zoomLevel;
        coords.Y = (y - yPos) / zoomLevel;
    }
    else
    {
        coords.X = -1; // Indicate invalid coordinates
        coords.Y = -1;
    }
    return coords;
}

void Canvas::ApplyBrush(int x, int y, bool erase)
{ 
    COORD coords = ConvertMouseCoordsToTextureCoords(x, y);

    if (erase)
        currentPixel = deletePixel;
    else
        currentPixel = drawPixel;

    switch (currentBrushType) {
    case BrushType::BRUSH_POINT:
        PaintPoint(coords.X, coords.Y);
        break;
    case BrushType::BRUSH_BLOCK:
        PaintBlock(coords.X, coords.Y, brushSize); // Draws a filled block
        break;
    case BrushType::BRUSH_RECT:
        // store the initial click position
        if (!initialClick) {
            initialClickCoords.X = coords.X;
            initialClickCoords.Y = coords.Y;
            initialClick = true; // Reset after the initial click is recorded
        }
        else {
            PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, coords.X, coords.Y, false, brushSize);
            initialClick = false; // Reset after the initial click is recorded
            initialClickCoords.X = coords.X;
            initialClickCoords.Y = coords.Y;
        }
        break;
    case BrushType::BRUSH_RECT_FILLED:
        // store the initial click position
        if (!initialClick) {
            initialClickCoords.X = coords.X;
            initialClickCoords.Y = coords.Y;
            initialClick = true; // Reset after the initial click is recorded
        }
        else {
            PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, coords.X, coords.Y);
            initialClick = false; // Reset after the initial click is recorded
            initialClickCoords.X = coords.X;
            initialClickCoords.Y = coords.Y;
        }
        break;    
    case BrushType::BRUSH_LINE:
        // store the initial click position
       if (!initialClick) {
           initialClickCoords.X = coords.X;
           initialClickCoords.Y = coords.Y;
           initialClick = true; // Reset after the initial click is recorded
       }
       else {
           PaintLine(initialClickCoords.X, initialClickCoords.Y, coords.X, coords.Y, brushSize); // Draws a line
           initialClick = false; // Reset after the initial click is recorded
           initialClickCoords.X = coords.X;
           initialClickCoords.Y = coords.Y;
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
    brushSize = (brushSize + sizeChange) % 5;
}


void Canvas::PaintPoint(int x, int y)
{
    texture->SetPixel(x, y, currentPixel);
}

// Bresenham's line algorithm
void Canvas::PaintLine(int x0, int y0, int x1, int y1, int lineThickness)
{
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    // Adjust the start point for even thickness to ensure symmetric distribution
    int offset = lineThickness % 2 == 0 ? 1 : 0;

    while (true) {
        // Draw a square of thickness around the current point
        for (int i = -lineThickness / 2; i <= lineThickness / 2 - offset; ++i) {
            for (int j = -lineThickness / 2; j <= lineThickness / 2 - offset; ++j) {
                PaintPoint(x0 + i, y0 + j);
            }
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
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
    drawingClass.WriteStringToBuffer(xPos - 1, yPos - 3, L"ZOOM: " + std::to_wstring(zoomLevel) + L'. ');
    drawingClass.WriteStringToBuffer(xPos + 8, yPos - 3, L"X: -   Y: -");

    COORD mouseCoords = drawingClass.GetMousePosition();

    // if coords withing canvas diplay (index starting from 1 for user)
    if (IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        COORD textureCoords = ConvertMouseCoordsToTextureCoords(mouseCoords.X, mouseCoords.Y);
        drawingClass.WriteStringToBuffer(xPos + 11, yPos - 3, std::to_wstring(textureCoords.X + 1));
        drawingClass.WriteStringToBuffer(xPos + 18, yPos - 3, std::to_wstring(textureCoords.Y + 1));
    }

    drawingClass.DrawRectangleEdgeLength(xPos - 1, yPos - 1, (texture->GetWidth() * zoomLevel) + 2, (texture->GetHeight() * zoomLevel) + 2, FG_RED);

    // add texture to screen buffer
    drawingClass.DrawTextureToScreen(texture, xPos, yPos, zoomLevel, true);
    // draw current brush (block, line, or square being drawn)
    DisplayBrush();
}

void Canvas::DisplayBrush()
{
    short highlightColour = FG_RED;
    // get appropriate coords for drawing to screen - will dynamically calc these
    COORD mouseCoords = drawingClass.GetMousePosition();
    COORD screenClickCoords = { initialClickCoords.X + xPos, initialClickCoords.Y + yPos };

    // draw appropriate brush
    switch (currentBrushType) {
    case BrushType::BRUSH_POINT:
        drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, drawPixel.Attributes, PIXEL_THREEQUARTERS);
        break;
    case BrushType::BRUSH_BLOCK:
        drawingClass.DrawSquare(mouseCoords.X, mouseCoords.Y, brushSize, drawPixel.Attributes, PIXEL_THREEQUARTERS, true);
        break;
    case BrushType::BRUSH_RECT:
        drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, drawPixel.Attributes, PIXEL_THREEQUARTERS);
        if (initialClick)
        {
            drawingClass.DrawRectangleCoords(screenClickCoords.X, screenClickCoords.Y, mouseCoords.X, mouseCoords.Y, drawPixel.Attributes, false, PIXEL_THREEQUARTERS, brushSize);
            drawingClass.DrawPoint(screenClickCoords.X, screenClickCoords.Y, highlightColour, PIXEL_THREEQUARTERS);
            drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, highlightColour, PIXEL_THREEQUARTERS);

        }
        break;
    case BrushType::BRUSH_RECT_FILLED:
        drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, drawPixel.Attributes, PIXEL_THREEQUARTERS);
        if (initialClick)
        {
            drawingClass.DrawRectangleCoords(screenClickCoords.X, screenClickCoords.Y, mouseCoords.X, mouseCoords.Y, drawPixel.Attributes, true, PIXEL_THREEQUARTERS);
            drawingClass.DrawPoint(screenClickCoords.X, screenClickCoords.Y, highlightColour, PIXEL_THREEQUARTERS);
            drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, highlightColour, PIXEL_THREEQUARTERS);
        }
        break;        break;
    case BrushType::BRUSH_LINE:
        drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, drawPixel.Attributes, PIXEL_THREEQUARTERS);
        if (initialClick)
        {
            drawingClass.DrawLine(screenClickCoords.X, screenClickCoords.Y, mouseCoords.X, mouseCoords.Y, currentPixel.Attributes, PIXEL_THREEQUARTERS, brushSize);
            drawingClass.DrawPoint(screenClickCoords.X, screenClickCoords.Y, highlightColour, PIXEL_THREEQUARTERS);
            drawingClass.DrawPoint(mouseCoords.X, mouseCoords.Y, highlightColour, PIXEL_THREEQUARTERS);
        }
        break;
    }

}

void Canvas::IncreaseZoomLevel()
{
    zoomLevel = (zoomLevel % 3) + 1;
}



