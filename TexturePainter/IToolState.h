#pragma once

#include <Windows.h> // Ensure this is included for COORD

// Forward declaration of Canvas
class Canvas;

class IToolState {
protected:
    Canvas& canvas;
    COORD initialClickCoords{ 0,0 };
    bool initialClick{ false };

public:
    IToolState(Canvas& canvas) : canvas(canvas) {}
    virtual void HandleBrushStroke(COORD mouseCoords) = 0;
    virtual void HandleMouseRelease(COORD mouseCoords) = 0;
    virtual void DisplayPointer(COORD mouseCoords) = 0;
    virtual void ToggleOption() = 0;
    virtual void ResetTool() = 0;
    virtual void ResetClicks() = 0;

    virtual ~IToolState() {}
};

