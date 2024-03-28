#pragma once

#include "Windows.h"

class ICoordinateStrategy {
public:
    virtual COORD ConvertScreenToTexture(COORD) const = 0;
    virtual COORD ConvertTextureToScreen(COORD) const = 0;
    virtual COORD ConvertCoordsToCanvasPixel(COORD) const = 0;

    virtual ~ICoordinateStrategy() = default; // Virtual destructor for proper cleanup
};