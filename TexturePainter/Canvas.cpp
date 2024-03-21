#include <Windows.h>

// undefine the macros from windows.h
#undef max
#undef min

// need to include full header here to use class methods from DI
#include "TexturePainter.h"
// need to #include Texture.h in Canvas.h before here
#include "Canvas.h"


Canvas::Canvas(TexturePainter& drawer, Texture* tex, std::wstring fn, std::wstring fp, int x, int y)
    : drawingClass{ drawer }, texture{ tex }, currentBrushStroke{ nullptr }, fileName { fn }, filePath{ fp }, xPos{ x }, yPos{ y },
    initialClick{ false }, initialClickCoords{ 0, 0 }
{
    currentBrushType = STARTING_BRUSH;
    brushSize = START_BRUSH_SIZE;
    zoomLevel = START_ZOOM_LEVEL;

    currentPixel = { STARTING_GLYPH, STARTING_COLOUR };
    drawPixel = { STARTING_GLYPH, STARTING_COLOUR };
    deletePixel = { L' ', 0 };

    //create first brush stroke
    currentBrushStroke = new Texture{ texture->GetWidth(), texture->GetHeight() };

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

COORD Canvas::ConvertScreenCoordsToTextureCoords(int x, int y)
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

COORD Canvas::ConvertTextureCoordsToScreenCoords(int x, int y)
{
    COORD screenCoords{};

    // reverse transformation used in ConvertMouseCoordsToTextureCoords
    // Consider the canvas position (xPos, yPos) and the zoom level
    screenCoords.X = xPos + (x * zoomLevel);
    screenCoords.Y = yPos + (y * zoomLevel);

    return screenCoords;
}

void Canvas::ApplyBrushStroke(const Texture* brushStroke)
{
    texture->MergeOther(brushStroke);
    currentBrushStroke->Clear();
}


void Canvas::ApplyBrush(int x, int y, bool erase)
{ 
    COORD coords = ConvertScreenCoordsToTextureCoords(x, y);
    COORD mouseCoords = drawingClass.GetMousePosition();


    if (erase)
        currentPixel = deletePixel;
    else
        currentPixel = drawPixel;

    switch (currentBrushType) {
    case BrushType::BRUSH_BLOCK:
        PaintBlock(coords.X, coords.Y, brushSize); // Draws a filled block
        ApplyBrushStroke(currentBrushStroke);
        break;
    case BrushType::BRUSH_RECT:
        // store the initial click position
        if (initialClick == false) {
            initialClickCoords.X = coords.X;
            initialClickCoords.Y = coords.Y;
            initialClick = true; // Reset after the initial click is recorded
        }
        else {
            initialClick = false; // Reset after the initial click is recorded
            initialClickCoords.X = coords.X;
            initialClickCoords.Y = coords.Y;
            ApplyBrushStroke(currentBrushStroke);
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
    int newSize = (brushSize - 1 + sizeChange + 5) % 5 + 1;
    brushSize = newSize;
}


void Canvas::PaintPoint(int x, int y)
{
    currentBrushStroke->SetPixel(x, y, currentPixel);
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
    drawingClass.WriteStringToBuffer(xPos - 1, yPos - 3, L"ZOOM: " + std::to_wstring(zoomLevel) + L'. ');
    drawingClass.WriteStringToBuffer(xPos + 8, yPos - 3, L"X: -   Y: -");

    COORD mouseCoords = drawingClass.GetMousePosition();

    // if coords withing canvas diplay (index starting from 1 for user)
    if (IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        COORD textureCoords = ConvertScreenCoordsToTextureCoords(mouseCoords.X, mouseCoords.Y);
        drawingClass.WriteStringToBuffer(xPos + 11, yPos - 3, std::to_wstring(textureCoords.X + 1));
        drawingClass.WriteStringToBuffer(xPos + 18, yPos - 3, std::to_wstring(textureCoords.Y + 1));
    }

    drawingClass.DrawRectangleEdgeLength(xPos - 1, yPos - 1, (texture->GetWidth() * zoomLevel) + 2, (texture->GetHeight() * zoomLevel) + 2, FG_RED);

    // add texture to screen buffer
    drawingClass.DrawTextureToScreen(texture, xPos, yPos, zoomLevel, true);
    // draw current brush (block, line, or square being drawn)

    // if brushStroke object exists, draw it to screen
    if (currentBrushStroke != nullptr)
        drawingClass.DrawPartialTextureToScreen(currentBrushStroke, xPos, yPos, zoomLevel);

    DisplayBrush();
}

void Canvas::DisplayBrush()
{
    // get appropriate coords for drawing to screen - will dynamically calc these
    COORD mouseCoords = ConvertScreenCoordsToTextureCoords(drawingClass.GetMousePosition().X, drawingClass.GetMousePosition().Y);
   // COORD mouseCoords = ConvertTextureCoordsToScreenCoords(mouseTexPosition.X, mouseTexPosition.Y);

    //COORD mouseCoords = { drawingClass.GetMousePosition().X - xPos, drawingClass.GetMousePosition().Y - yPos };


    COORD ScreenInitClickCoords = ConvertTextureCoordsToScreenCoords(initialClickCoords.X, initialClickCoords.Y);

    if (currentBrushStroke != nullptr)
        currentBrushStroke->Clear();

    // draw appropriate brush
    switch (currentBrushType) {
    case BrushType::BRUSH_BLOCK:
        //DisplayBlockOnCanvas(mouseCoords.X, mouseCoords.Y, brushSize, drawPixel.Attributes, PIXEL_THREEQUARTERS);
        break;
    case BrushType::BRUSH_RECT:
        if (initialClick)
        {
            PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, false, brushSize);
        }

        //if (initialClick)
        //{
        //    DisplayRectangleOnCanvas(ScreenInitClickCoords.X, ScreenInitClickCoords.Y, drawingClass.GetMousePosition().X, drawingClass.GetMousePosition().Y, drawPixel.Attributes, false, PIXEL_THREEQUARTERS, brushSize);
        //    DisplayPixelOnCanvas(ScreenInitClickCoords.X, ScreenInitClickCoords.Y, TexturePainter::HIGHLIGHT_COLOUR, PIXEL_THREEQUARTERS);
        //}
        //DisplayPixelOnCanvas(mouseCoords.X, mouseCoords.Y, TexturePainter::HIGHLIGHT_COLOUR, PIXEL_THREEQUARTERS);
        break;
    case BrushType::BRUSH_RECT_FILLED:
        if (initialClick)
        {
            DisplayRectangleOnCanvas(ScreenInitClickCoords.X, ScreenInitClickCoords.Y, drawingClass.GetMousePosition().X, drawingClass.GetMousePosition().Y, drawPixel.Attributes, true, PIXEL_THREEQUARTERS);
            DisplayPixelOnCanvas(ScreenInitClickCoords.X, ScreenInitClickCoords.Y, TexturePainter::HIGHLIGHT_COLOUR, PIXEL_THREEQUARTERS);
        }
        DisplayPixelOnCanvas(mouseCoords.X, mouseCoords.Y, TexturePainter::HIGHLIGHT_COLOUR, PIXEL_THREEQUARTERS);

        break;
    case BrushType::BRUSH_LINE:
        if (initialClick)
        {
            DisplayLineOnCanvas(ScreenInitClickCoords.X, ScreenInitClickCoords.Y, mouseCoords.X, mouseCoords.Y, currentPixel.Attributes, PIXEL_THREEQUARTERS, brushSize);
            DisplayBlockOnCanvas(ScreenInitClickCoords.X, ScreenInitClickCoords.Y, brushSize, TexturePainter::HIGHLIGHT_COLOUR, PIXEL_THREEQUARTERS);
        }
        DisplayBlockOnCanvas(mouseCoords.X, mouseCoords.Y, brushSize, TexturePainter::HIGHLIGHT_COLOUR, PIXEL_THREEQUARTERS);
        break;
    }

}

void Canvas::DisplayRectangleOnCanvas(int x0, int y0, int x1, int y1, short colour, bool filled, short glyph, int lineWidth)
{   // Normalize coordinates
    int left = std::min(x0, x1);
    int top = std::min(y0, y1);
    int right = std::max(x0, x1);
    int bottom = std::max(y0, y1);

    if (filled) {
        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                DisplayPixelOnCanvas(x, y, colour, glyph);
            }
        }
    } else {
        for (int i = 0; i < lineWidth * zoomLevel; i+=zoomLevel) {
            // Draw top and bottom sides of the current concentric rectangle
            for (int x = left + i; x <= right - i; ++x) {
                DisplayPixelOnCanvas(x, top + i, colour, glyph);
                DisplayPixelOnCanvas(x, bottom - i, colour, glyph);
            }
            // Draw left and right sides of the current concentric rectangle
            for (int y = top + i + 1; y <= bottom - i - 1; y++) { // Avoid redrawing corners
                DisplayPixelOnCanvas(left + i, y, colour, glyph);
                DisplayPixelOnCanvas(right - i, y, colour, glyph);
            }
        }
    }
}

void Canvas::DisplayLineOnCanvas(int x0, int y0, int x1, int y1, short colour, short glyph, int lineWidth)
{
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? zoomLevel : -zoomLevel;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? zoomLevel : -zoomLevel;
    int err = dx + dy, e2; // error value e_xy

    while (true) {
        DisplayBlockOnCanvas(x0, y0, lineWidth, colour, glyph); // Use DisplayBlockOnCanvas to draw a block at each point

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

void Canvas::DisplayBlockOnCanvas(int x0, int y0, int size, short colour, short glyph)
{
    for (int x{ 0 }; x < size * zoomLevel; x++)
        for (int y{ 0 }; y < size * zoomLevel; y++)
            DisplayPixelOnCanvas(x0 + x, y0 + y, colour, glyph);
}

void Canvas::DisplayPixelOnCanvas(int x, int y, short colour, short glyph)
{
    // convert given screen coordinates to texture position
    COORD texPosition = ConvertScreenCoordsToTextureCoords(x, y);
    // then convert back to screen - this is needed to 'stick' to the texture pixels when zooming
    COORD pixelCoords = ConvertTextureCoordsToScreenCoords(texPosition.X, texPosition.Y);
    // display block on screen corresponding to pixel placed
    drawingClass.DrawBlock(pixelCoords.X, pixelCoords.Y, zoomLevel, colour, glyph);
}

void Canvas::IncreaseZoomLevel()
{
    zoomLevel = (zoomLevel % 3) + 1;
}



