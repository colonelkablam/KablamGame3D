#pragma once

#include "ICommand.h"
#include "Canvas.h"

class BrushstrokeCommand : public ICommand {

    Canvas& canvas;  // Reference to the canvas
    Canvas::Brushstroke brushStroke;  // The brush stroke to apply


public:
    BrushstrokeCommand(Canvas& canvas, Canvas::Brushstroke stroke) 
        : canvas{ canvas }, brushStroke{ stroke } {}


    // Method to apply the stroke to the canvas
    void execute() override
    {
        // applying the texture to the background canvas 
        canvas.ApplyBrushstrokeTextureToBackground(brushStroke);
    }

    // Method to remove the stroke from the canvas
    void undo() override
    {
        canvas.ApplyUndoBrushstroke(brushStroke);

    }

};