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

    void ToggleOption()
    {
        // do nothing
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

    void ToggleOption()
    {
        // do nothing
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

    void ToggleOption()
    {
        // do nothing
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

    void ToggleOption()
    {
        // do nothing
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
    bool drawPartialSample{ true };
    Canvas::TextureSample textureSample{};

public:
    CopyBrushState(Canvas& canvas)
        : IToolState{ canvas } {}

    Canvas::TextureSample GetTextureSample()
    {
        return textureSample;
    }

    void SetTextureSample(Canvas::TextureSample newSample)
    {
        textureSample = newSample;
    }

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
            canvas.PaintTextureSample(textureSample, placement, drawPartialSample);
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

    void DisplayPointer(COORD mouseCoords) {
        int width = textureSample.width;
        int height = textureSample.height;
        int zoom = canvas.GetZoomLevel();

        // Calculate base coordinates for drawing
        COORD pixelCoord = canvas.coordinateStrategy->ConvertCoordsToCanvasPixel(mouseCoords);
        int baseX = pixelCoord.X - (width - 1) * zoom;
        int baseY = pixelCoord.Y - (height - 1) * zoom;

        // Iterate through sample pixels
        for (const Canvas::PixelSample& pixel : textureSample.pixels) {
            int x = baseX + pixel.x * zoom;
            int y = baseY + pixel.y * zoom;

            // Check for valid drawing position
            if (canvas.AreCoordsWithinCanvas(COORD{(short)x, (short)y}))
            {
                short glyph = pixel.glyph;
                short colour = pixel.colour;

                // When drawPartialSample is true, draw all pixels except delete pixels
                if (drawPartialSample && glyph != canvas.deletePixel.Char.UnicodeChar) {
                    canvas.drawingClass.DrawBlock(x, y, zoom, colour, glyph);
                }
                // When drawPartialSample is false, draw all pixels
                else if (!drawPartialSample) {
                    canvas.drawingClass.DrawBlock(x, y, zoom, colour, glyph);
                }
            }
        }
    }

    void ToggleOption()
    {
        drawPartialSample = !drawPartialSample;
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

