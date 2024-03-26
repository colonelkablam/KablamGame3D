#pragma once

#include "Canvas.h"
#include "Command.h"
#include "BrushStroke.h"


class BrushStrokeCommand : public Command {
    Canvas& canvas;  // Reference to the canvas
    BrushStroke* brushStroke;   // The brush stroke to apply

public:
    BrushStrokeCommand(Canvas& canvas, BrushStroke* stroke) 
        : canvas{ canvas }, brushStroke{ stroke } {}

    ~BrushStrokeCommand()
    {
        delete brushStroke;
        brushStroke = nullptr;
    }

    // Method to apply the stroke to the canvas
    void execute() override
    {
        // applying the texture to the background canvas populates the undo texture
        brushStroke->SetUndoTexture(canvas.MergeTexture(brushStroke->GetBrushStrokeTexture(), false));
    }

    // Method to remove the stroke from the canvas
    void undo() override
    {
        if (brushStroke->GetUndoTexture() != nullptr)
            canvas.MergeTexture(brushStroke->GetUndoTexture(), true);
    }

};