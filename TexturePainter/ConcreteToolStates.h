#pragma once

#include "IToolState.h";
#include "Canvas.h"

class BlockBrushState : public IToolState
{
public:

    BlockBrushState(Canvas& canvas)
        : IToolState{ canvas } {}

    // Correct placement of handleBrushStroke
    void HandleBrushStroke(COORD mouseCoords) override {
       canvas.PaintBlock(mouseCoords.X, mouseCoords.Y, canvas.GetBrushSize());
    }

    void HandleMouseRelease(COORD mouseCoords) override {
        canvas.SetBrushTextureToBackground();
        ResetClicks();
    }

    void DisplayPointer(COORD mouseCoords)
    {
        canvas.DisplayBrushPointer(mouseCoords);
    }
    
    void ResetClicks()
    {
        initialClick = false;
    }

    void ResetTool() override
    {
        initialClick = false;

    }

};

class RectBrushState : public IToolState {
public:
    RectBrushState(Canvas& canvas)
        : IToolState{ canvas } {}


    void HandleBrushStroke(COORD mouseCoords) override {
        if (!initialClick)
        {
            initialClickCoords = mouseCoords;
            initialClick = true;
        }
        else
        {
            int size = canvas.GetBrushSize();
            canvas.ClearCurrentBrushstrokeTexture();
            canvas.PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X + size - 1, mouseCoords.Y + size - 1, false, size);
        }
    }

    void HandleMouseRelease(COORD mouseCoords) override {
        canvas.SetBrushTextureToBackground();
        ResetClicks();
    }

    void DisplayPointer(COORD mouseCoords)
    {
        canvas.DisplayBrushPointer(mouseCoords);
    }

    void ResetClicks()
    {
        initialClick = false;
    }

    void ResetTool() override
    {
        initialClick = false;

    }
};

class FilledRectBrushState : public IToolState {
public:
    FilledRectBrushState(Canvas& canvas)
        : IToolState{ canvas } {}

    void HandleBrushStroke(COORD mouseCoords) override {
        if (!initialClick)
        {
            initialClickCoords = mouseCoords;
            initialClick = true;
        }
        else
        {
            int size = canvas.GetBrushSize();
            canvas.ClearCurrentBrushstrokeTexture();
            canvas.PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X + size - 1, mouseCoords.Y + size - 1, true);
        }
    }

    void HandleMouseRelease(COORD mouseCoords) override {
        canvas.SetBrushTextureToBackground();
        ResetClicks();
    }

    void DisplayPointer(COORD mouseCoords)
    {
        canvas.DisplayBrushPointer(mouseCoords);
    }

    void ResetClicks()
    {
        initialClick = false;
    }

    void ResetTool() override
    {
        initialClick = false;

    }
};

class LineBrushState : public IToolState {
public:
    LineBrushState(Canvas& canvas)
        : IToolState{ canvas } {}

    void HandleBrushStroke(COORD mouseCoords) override {
        if (!initialClick)
        {
            initialClickCoords = mouseCoords;
            initialClick = true;
        }
        else
        {
            int size = canvas.GetBrushSize();
            canvas.ClearCurrentBrushstrokeTexture();
            canvas.PaintLine(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, size);
        }
    }
    
    void HandleMouseRelease(COORD mouseCoords) override {
        canvas.SetBrushTextureToBackground();
        ResetClicks();
    }

    void DisplayPointer(COORD mouseCoords)
    {
        canvas.DisplayBrushPointer(mouseCoords);
    }

    void ResetClicks()
    {
        initialClick = false;
    }

    void ResetTool() override
    {
        initialClick = false;

    }
};

class CopyBrushState : public IToolState {

    bool textureBeenSampled{ false };
    Canvas::TextureSample textureSample{};

public:
    CopyBrushState(Canvas& canvas)
        : IToolState{ canvas } {}

    void HandleBrushStroke(COORD mouseCoords) override {
        
        if (!textureBeenSampled)
        {
            if (!initialClick)
            {
                initialClickCoords = mouseCoords;
                initialClick = true;
            }
            else
            {
                int size = canvas.GetBrushSize();
                canvas.ClearCurrentBrushstrokeTexture();
                canvas.PaintRectangleGlyphs(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, false, 1);
            }
        }
        else
        {
            COORD placement{ mouseCoords.X - (short)textureSample.width + 1, mouseCoords.Y - (short)textureSample.height + 1};
            canvas.PaintTextureSample(textureSample, placement);
        }

    }

    void HandleMouseRelease(COORD mouseCoords) override {
        if (!textureBeenSampled)
        {
            canvas.ClearCurrentBrushstrokeTexture();
            textureSample = canvas.GrabTextureSample(initialClickCoords, mouseCoords);
            textureBeenSampled = true;
        }
        else
        {
            canvas.SetBrushTextureToBackground();
        }
    }

    void DisplayPointer(COORD mouseCoords)
    {
        int width{ textureSample.width };
        int height{ textureSample.height };
        int zoom{ canvas.GetZoomLevel() };

        // get correct coordinates for sample and display
        COORD pixelCoord{ canvas.coordinateStrategy->ConvertCoordsToCanvasPixel(mouseCoords) };
        COORD textureCoord{ canvas.coordinateStrategy->ConvertScreenToTexture(mouseCoords) };

        // Pre-calculate parts of the position that don't change per pixel
        int baseX = pixelCoord.X + zoom - width * zoom;
        int baseY = pixelCoord.Y + zoom - height * zoom;

        // iterate though sample - this could be done in engine but currently only needed when drawing textures
        for (const Canvas::PixelSample& pixel : textureSample.pixels)
        {
            // Effectively scale up the drawing of each pixel in the texture
            // Calculate the top-left corner of the block representing this pixel
            int x = baseX + pixel.x * zoom;
            int y = baseY + pixel.y * zoom;

            // add 'shade' so cut cna easily be seen
            canvas.drawingClass.DrawBlock(x, y, zoom, canvas.cutPixel.Attributes | pixel.colour << 4, canvas.cutPixel.Char.UnicodeChar);
        }
    }

    void ResetClicks()
    {
        initialClick = false;
    }

    void ResetTool() override
    {
        initialClick = false;
        textureBeenSampled = false;
        textureSample.Reset();
    }
};

