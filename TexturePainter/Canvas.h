#pragma once

#include "Texture.h"


class Canvas {

    enum class BrushType {
        BRUSH_POINT,  // For single pixel drawing
        BRUSH_SQUARE, // For drawing squares
        BRUSH_LINE    // For drawing lines
    };

    static const short startingColour = FG_WHITE;
    static const short startingGlyph = PIXEL_SOLID;
    static const int startingZoomLevel = 1;
    static const int startingBrushSize = 1;
    static const BrushType startingBrush = BrushType::BRUSH_POINT;


    BrushType currentBrushType;
    int brushSize;
    short currentColour;
    short currentGlyph;
    CHAR_INFO currentPixel;
    CHAR_INFO deletePixel;


    std::wstring fileName;
    std::wstring filePath;
    int illumination;
    int xPos;
    int yPos;
    int width;
    int height;
    int zoomLevel;
    Texture* texture;

    Canvas(Texture* text, std::wstring fileName, std::wstring filePath, int xPos, int yPos);

    ~Canvas();

    // Method to cycle to the next zoom level
    void IncreaseZoomLevel();
};

