#pragma once

#include <Windows.h> // Ensure this is included for COORD

// Forward declaration of Canvas
class Canvas;

class ToolState {
protected:
    Canvas& canvas;
    COORD initialClickCoords{ 0,0 };
    bool initialClick{ false };

public:
    ToolState(Canvas& canvas) : canvas(canvas) {}
    virtual void HandleBrushStroke(COORD mouseCoords) = 0;
    virtual void ResetTool() = 0;

    virtual ~ToolState() {}
};

