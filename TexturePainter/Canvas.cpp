#pragma once
#include "Canvas.h"

#include <Windows.h>


Canvas::Canvas(Texture* tex, std::wstring fn, std::wstring fp, int x, int y)
    : texture {tex}, fileName {fn}, filePath { fp}, xPos{ x }, yPos{ y }
{

    currentBrushType = startingBrush;
    brushSize = startingBrushSize;
    zoomLevel = startingZoomLevel;

    currentColour = startingColour;
    currentGlyph = startingGlyph;
    currentPixel = { startingGlyph, startingColour };
    deletePixel = { L' ', 0 };

    illumination = texture->GetIllumination();
    width = texture->GetWidth();
    height = texture->GetHeight();

}

Canvas::~Canvas()
{
    delete texture;
}

// Method to cycle to the next zoom level
void Canvas::IncreaseZoomLevel() {
    // Increment zoom level, wrapping around back to ZOOM_X1 after ZOOM_X3
    zoomLevel = (zoomLevel % 3) + 1;
}