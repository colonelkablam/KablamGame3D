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
        // need to display pointer
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
                canvas.PaintRectangleCoords(initialClickCoords.X - 1, initialClickCoords.Y - 1, mouseCoords.X + size, mouseCoords.Y + size, false, 1);
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
        int w{ textureSample.width };
        int h{ textureSample.height };

        for (const auto& pixel : textureSample.pixels)
        {
            int x{ mouseCoords.X + pixel.x - w + 1 };
            int y{ mouseCoords.Y + pixel.y - h + 1 };

            canvas.drawingClass.DrawPoint(x, y, pixel.colour, pixel.glyph);
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
    }
};

