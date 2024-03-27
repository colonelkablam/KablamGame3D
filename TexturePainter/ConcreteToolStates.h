#pragma once

#include "ToolState.h";
#include "Canvas.h"

class BlockBrushState : public ToolState
{
public:

    BlockBrushState(Canvas& canvas)
        : ToolState{ canvas } {}

    // Correct placement of handleBrushStroke
    void HandleBrushStroke(COORD mouseCoords) override {
       canvas.PaintBlock(mouseCoords.X, mouseCoords.Y, canvas.GetBrushSize());
    }

    void ResetTool() override
    {
        initialClick = false;
    }
};

class RectBrushState : public ToolState {
public:
    RectBrushState(Canvas& canvas)
        : ToolState{ canvas } {}


    void HandleBrushStroke(COORD mouseCoords) override {
        if (!initialClick)
        {
            initialClickCoords = mouseCoords;
            initialClick = true;
        }
        else
        {
            int size = canvas.GetBrushSize();
            canvas.ClearCurrentBrushStrokeTexture();
            canvas.PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X + size - 1, mouseCoords.Y + size - 1, false, size);
        }
    }

    void ResetTool() override
    {
        initialClick = false;
    }
};

class FilledRectBrushState : public ToolState {
public:
    FilledRectBrushState(Canvas& canvas)
        : ToolState{ canvas } {}

    void HandleBrushStroke(COORD mouseCoords) override {
        if (!initialClick)
        {
            initialClickCoords = mouseCoords;
            initialClick = true;
        }
        else
        {
            int size = canvas.GetBrushSize();
            canvas.ClearCurrentBrushStrokeTexture();
            canvas.PaintRectangleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X + size - 1, mouseCoords.Y + size - 1, true);
        }
    }

    void ResetTool() override
    {
        initialClick = false;
    }
};

class LineBrushState : public ToolState {
public:
    LineBrushState(Canvas& canvas)
        : ToolState{ canvas } {}

    void HandleBrushStroke(COORD mouseCoords) override {
        if (!initialClick)
        {
            initialClickCoords = mouseCoords;
            initialClick = true;
        }
        else
        {
            canvas.ClearCurrentBrushStrokeTexture();
            canvas.PaintLine(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X, mouseCoords.Y, canvas.GetBrushSize());
        }
    }

    void ResetTool() override
    {
        initialClick = false;
    }
};