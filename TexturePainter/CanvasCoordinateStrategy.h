#pragma once

#include "ICoordinateStrategy.h"

class CanvasCoordinateStrategy : public ICoordinateStrategy {
private:
    const COORD& topLeft;
    const COORD& canvasViewOffset;
    const int& zoom;

public:
    CanvasCoordinateStrategy(const COORD& topLeft, const COORD& canvasViewOffset, const int& zoomLev)
        : topLeft(topLeft), canvasViewOffset(canvasViewOffset), zoom(zoomLev) {}

    COORD ConvertScreenToTexture(COORD screen) const override
    {
        COORD textureCoords{    static_cast<SHORT>((screen.X - topLeft.X) / zoom + canvasViewOffset.X),
                                static_cast<SHORT>((screen.Y - topLeft.Y) / zoom + canvasViewOffset.Y) };
        return textureCoords;
    }

    COORD ConvertTextureToScreen(COORD texture) const override
    {
        COORD screenCoords{ static_cast<SHORT>((texture.X - canvasViewOffset.X) * zoom + topLeft.X),
                            static_cast<SHORT>((texture.Y - canvasViewOffset.Y) * zoom + topLeft.Y) };
        return screenCoords;
    }

    // this is needed to 'snap' the coordinate to the nearest pixel on the texture rather than screen
    COORD ConvertCoordsToCanvasPixel(COORD coords) const override
    {
        COORD textureCoords{ ConvertScreenToTexture(coords) };
        COORD pixelCoords{ ConvertTextureToScreen(textureCoords) };

        return pixelCoords;
    }

    // for displaying the texture brush to screen accounting for zoom and offset
    COORD AdjustForOffset(COORD coords) const override
    {
        COORD screenCoords{ static_cast<SHORT>((coords.X - canvasViewOffset.X * zoom)),
                            static_cast<SHORT>((coords.Y - canvasViewOffset.Y * zoom)) };
        return screenCoords;

    }

};