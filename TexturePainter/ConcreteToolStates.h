#pragma once

#include "IToolState.h";
#include "Canvas.h"

class BrushToolState : public IToolState
{
public:

    BrushToolState(Canvas& canvas)
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

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

class LineToolState : public IToolState {
public:
    LineToolState(Canvas& canvas)
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

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

class RectToolState : public IToolState {
public:
    RectToolState(Canvas& canvas)
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

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

class FilledRectToolState : public IToolState {
public:
    FilledRectToolState(Canvas& canvas)
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

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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


class CircleToolState : public IToolState {
public:
    CircleToolState(Canvas& canvas)
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

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

class FilledCircleToolState : public IToolState {
public:
    FilledCircleToolState(Canvas& canvas)
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
            canvas.PaintCircleCoords(initialClickCoords.X, initialClickCoords.Y, mouseCoords.X + size - 1, mouseCoords.Y + size - 1, true);
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

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

class FillToolState : public IToolState
{
public:

    FillToolState(Canvas& canvas)
        : IToolState{ canvas } {}

    // Correct placement of handleBrushStroke
    void HandleBrushStroke(COORD mouseCoords) override {
        short colourToFill = canvas.GetBackgroundTextureSample(mouseCoords);

        // Perform flood fill starting from mouseCoords
        FloodFill(mouseCoords.X, mouseCoords.Y, colourToFill, canvas.GetTextureWidth(), canvas.GetTextureHeight());
    }

    void HandleMouseRelease(COORD mouseCoords) override {
        canvas.SetBrushTextureToBackground();
        ResetClicks();
    }

    void DisplayPointer(COORD mouseCoords)
    {
        canvas.DisplayBrushPointer(mouseCoords, true);
    }

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

    // additional concrete method to handle a flood fill
    void FloodFill(short x, short y, short targetColor, int textureWidth, int textureHeight) {
        
        // 1D array to track visited pixels
        std::vector<bool> visited(textureWidth * textureHeight, false); // fill with false

        // flood fill using a stack-based approach
        std::stack<int> stack;
        stack.push(x + y * textureWidth); // Convert 2D to 1D index

        while (!stack.empty()) {
            int currentIndex = stack.top();
            stack.pop();

            short currentX = currentIndex % textureWidth;
            short currentY = currentIndex / textureWidth;

            // Check if the current pixel is within bounds and has not been visited
            if (currentX < 0 || currentX >= textureWidth || currentY < 0 || currentY >= textureHeight || visited[currentIndex])
                continue;

            // Update stack
            visited[currentIndex] = true;

            // Check if the current pixel has the target color
            if (canvas.GetBackgroundTextureSample({ currentX, currentY }) != targetColor)
                continue;

            // Fill the current pixel
            canvas.PaintPoint(currentX, currentY);

            // Add adjacent pixels to the stack
            stack.push((currentX + 1) + currentY * textureWidth); // Right
            stack.push((currentX - 1) + currentY * textureWidth); // Left
            stack.push(currentX + (currentY + 1) * textureWidth); // Down
            stack.push(currentX + (currentY - 1) * textureWidth); // Up
        }
    }

};


class CopyToolState : public IToolState {

public:
    CopyToolState(Canvas& canvas)
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
            canvas.PaintClipboardTextureSample(placement, toggleState); // toggleState is partial/full clipboard texture drawn
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
                    // When toggleState is true, draw all pixels except delete pixels
                    if (toggleState)
                    {
                        if (pixel.glyph != canvas.deletePixel.Char.UnicodeChar)
                        {
                            canvas.drawingClass.DrawBlock(x, y, zoom, pixel.colour, pixel.glyph);
                        }
                    }
                    // When toggleState is false, draw all pixels
                    else
                        canvas.drawingClass.DrawBlock(x, y, zoom, pixel.colour, pixel.glyph);
                }
            }
        }
    }

    void ToggleToolState()
    {
        toggleState = !toggleState;
    }

    bool GetToggleState()
    {
        return toggleState;
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

