#pragma once

// forward declaration is sufficiant as only using pointers to Texture objects and not accessing members
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

