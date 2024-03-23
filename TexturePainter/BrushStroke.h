#pragma once


class Texture;

class BrushStroke
{
private:
    int x;
    int y;
    Texture* strokeTexture;
    Texture* undoTexture;

public:
    BrushStroke(int xPos, int yPos, Texture* texture);

    ~BrushStroke();

    Texture* GetStrokeTexture();

    Texture* GetUndoTexture();

    void SetUndoTexture(Texture* undo);

};

//#include "BrushStroke.h"


BrushStroke::BrushStroke(int xPos, int yPos, Texture* texture)
    : x{ xPos }, y{ yPos }, strokeTexture{ texture }, undoTexture{ nullptr }
{
    // deep copy of texture
    strokeTexture = new Texture(*texture);
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
    undoTexture = undo;
}

