#pragma once

#include "BrushStroke.h"
#include "Command.h"
#include "Canvas.h"

class BrushStrokeCommand : public Command {
    Canvas& canvas;  // Reference to the canvas
    BrushStroke* brushStroke;   // The brush stroke to apply

public:
    BrushStrokeCommand(Canvas& canvas, BrushStroke* stroke) 
        : canvas{ canvas }, brushStroke{ stroke } {}

    ~BrushStrokeCommand()
    {
        delete brushStroke;
    }

    // Method to apply the stroke to the canvas
    void execute() override {
        Texture* undoTexture = canvas.MergeBrushStroke(brushStroke->GetStrokeTexture());
        brushStroke->SetUndoTexture(undoTexture);
    }

    // Method to remove the stroke from the canvas
    void undo() override {
        canvas.MergeBrushStroke(brushStroke->GetUndoTexture()); 
    }

};