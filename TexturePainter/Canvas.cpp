#pragma once

#include <Windows.h>
#include "Canvas.h"


Canvas::Canvas(Texture* tex, std::wstring fn, std::wstring fp, int x, int y)
    : texture {tex}, fileName {fn}, filePath { fp}, xPos{ x }, yPos{ y }
{

    currentBrushType = startingBrush;
    brushSize = startingBrushSize;
    zoomLevel = startingZoomLevel;

    currentColour = startingColour;
    currentGlyph = startingGlyph;
    currentPixel = { startingGlyph, startingColour };
    drawPixel = { startingGlyph, startingColour };
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

short Canvas::GetBrushColour()
{
    return currentColour;
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
    case BrushType::BRUSH_SQUARE:
        PaintSquare(coords.X, coords.Y, brushSize); // Draws a filled square
        break;
    case BrushType::BRUSH_LINE:
        //For a line, you'll likely need to store the initial click position
        //and then draw the line to the current mouse position
       //if (initialClick) {
       //    initialX = x;
       //    initialY = y;
       //    initialClick = false; // Reset after the initial click is recorded
       //}
       //else {
       //    DrawLine(initialX, initialY, x, y, currentPixel); // Draws a line
       //}
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
void Canvas::PaintLine(int x0, int y0, int x1, int y1)
{
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    while (true) {
        PaintPoint(x0, y0); // Set the character at the current position
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void Canvas::PaintSquare(int x, int y, int sideLength, bool filled, int lineWidth)
{
    if (filled)
    {
        for (size_t i{ 0 }; i < sideLength; i++)
            for (size_t j{ 0 }; j < sideLength; j++)
                PaintPoint(x + i, y + j);
    }
    else
    {
        sideLength -= 1;

        // Top side
        PaintLine(x, y, x + sideLength, y);
        // Right side
        PaintLine(x + sideLength, y, x + sideLength, y + sideLength);
        // Bottom side
        PaintLine(x, y + sideLength, x + sideLength, y + sideLength);
        // Left side
        PaintLine(x, y, x, y + sideLength);
    }
}

void Canvas::IncreaseZoomLevel()
{
    zoomLevel = (zoomLevel % 3) + 1;
}



