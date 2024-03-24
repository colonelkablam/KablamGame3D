#include "BrushStroke.h"


BrushStroke::BrushStroke(int xPos, int yPos, Texture* texture)
    : x{ xPos }, y{ yPos }, strokeTexture{ texture }, undoTexture{ nullptr }
{
    // take ownership of texture pointer
    delete strokeTexture;
    delete undoTexture;
    strokeTexture = texture;
}

BrushStroke::~BrushStroke()
{
    delete strokeTexture;
    delete undoTexture;
}

Texture* BrushStroke::GetStrokeTexture()
{
    return strokeTexture;
}

Texture* BrushStroke::GetUndoTexture()
{
    return undoTexture;
}

void BrushStroke::SetUndoTexture(Texture* undo)
{
    delete undoTexture;
    undoTexture = undo;
}