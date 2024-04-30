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

    void SetClicks(bool value)
    {
        initialClick = value;
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

    void SetClicks(bool value)
    {
        initialClick = value;
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

    void SetClicks(bool value)
    {
        initialClick = value;
    }

    void ResetTool() override
    {
        initialClick = false;

    }
};


class CircleBrushState : public IToolState {
public:
    CircleBrushState(Canvas& canvas)
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
            canvas.PaintCircleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X + size - 1, mouseCoords.Y + size - 1, false, size);
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

    void SetClicks(bool value)
    {
        initialClick = value;
    }

    void ResetTool() override
    {
        initialClick = false;

    }
};

class FilledCircleBrushState : public IToolState {
public:
    FilledCircleBrushState(Canvas& canvas)
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
            canvas.PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, true);
        }
    }

    void HandleMouseRelease(COORD mouseCoords) override {
        canvas.SetBrushTextureToBackground();
        ResetClicks();
    }

    void DisplayPointer(COORD mouseCoords)
    {
        canvas.DisplayBrushPointer(mouseCoords, true);
    }

    void ToggleOption()
    {
        // do nothing
    }

    void ResetClicks()
    {
        initialClick = false;
    }

    void SetClicks(bool value)
    {
        initialClick = value;
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

    void SetClicks(bool value)
    {
        initialClick = value;
    }

    void ResetTool() override
    {
        initialClick = false;

    }
};

class CopyBrushState : public IToolState {

    bool drawPartialSample{ true };

public:
    CopyBrushState(Canvas& canvas)
        : IToolState{ canvas } {}

    void HandleBrushStroke(COORD mouseCoords) override {
        
        if (canvas.clipboardTextureSample == nullptr)
        {
            if (initialClick == false)
            {
                initialClickCoords = mouseCoords;
                initialClick = true;
            }
            else
            {
                canvas.ClearCurrentBrushstrokeTexture();
                canvas.PaintRectangleGlyphs(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, false, 1);
            }
        }
        else // only drawing if not nullptr
        {
            COORD placement{ mouseCoords.X - (short)canvas.clipboardTextureSample->width + 1, mouseCoords.Y - (short)canvas.clipboardTextureSample->height + 1};
            canvas.PaintClipboardTextureSample(placement, drawPartialSample);
        }

    }

    void HandleMouseRelease(COORD mouseCoords) override {
        if (canvas.clipboardTextureSample == nullptr)
        {
            canvas.ClearCurrentBrushstrokeTexture();
            canvas.AddTextureSampleToClipboard(initialClickCoords, mouseCoords);
        }
        else
        {
            canvas.SetBrushTextureToBackground();
        }
    }

    void DisplayPointer(COORD mouseCoords) {

        if (canvas.clipboardTextureSample != nullptr)
        {
            // Access the sample and canvas only once
            const auto& sample = *canvas.clipboardTextureSample;

            int width = sample.width;
            int height = sample.height;
            int zoom = canvas.GetZoomLevel();

            // Calculate base coordinates for drawing
            COORD pixelCoord = canvas.coordinateStrategy->ConvertCoordsToCanvasPixel(mouseCoords);
            int baseX = pixelCoord.X - (width - 1) * zoom;
            int baseY = pixelCoord.Y - (height - 1) * zoom;

            // Iterate through sample pixels
            for (const Canvas::PixelSample& pixel : sample.pixels) {
                int x = baseX + pixel.x * zoom;
                int y = baseY + pixel.y * zoom;

                // Check for valid drawing position
                if (canvas.AreCoordsWithinCanvas(COORD{ (short)x, (short)y }))
                {
                    // When drawPartialSample is true, draw all pixels except delete pixels
                    if (drawPartialSample)
                    {
                        if (pixel.glyph != canvas.deletePixel.Char.UnicodeChar)
                        {
                            canvas.drawingClass.DrawBlock(x, y, zoom, pixel.colour, pixel.glyph);
                        }
                    }
                    // When drawPartialSample is false, draw all pixels
                    else
                        canvas.drawingClass.DrawBlock(x, y, zoom, pixel.colour, pixel.glyph);
                }
            }

        }


    }

    void ToggleOption()
    {
        drawPartialSample = !drawPartialSample;
    }
    void ResetClicks() // default false
    {
        initialClick = false;
    }

    void SetClicks(bool value)
    {
        initialClick = value;
    }

    void ResetTool() override
    {
        initialClick = false;
        delete canvas.clipboardTextureSample;
        canvas.clipboardTextureSample = nullptr;
    }
};

