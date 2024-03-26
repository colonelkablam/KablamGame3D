#include "BrushStroke.h"

BrushStroke::BrushStroke(Texture* texture)
    : brushStrokeTexture { texture }, undoTexture {nullptr} { }

BrushStroke::~BrushStroke()
{
    delete brushStrokeTexture;
    brushStrokeTexture = nullptr;

    if (undoTexture != nullptr)
    {
        delete undoTexture;
        undoTexture = nullptr;
    }
}

Texture* BrushStroke::GetBrushStrokeTexture()
{
    return brushStrokeTexture;
}

Texture* BrushStroke::GetUndoTexture()
{
    return undoTexture;
}

void BrushStroke::SetBrushStrokeTexture(Texture* texture)
{
    delete brushStrokeTexture;
    brushStrokeTexture = texture;
}


void BrushStroke::SetUndoTexture(Texture* texture)
{
    delete undoTexture;
    undoTexture = texture;
}
