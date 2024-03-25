#pragma once

class Texture;


class BrushStroke
{
private:

    Texture* brushStrokeTexture;
    Texture* undoTexture;

public:
    BrushStroke(Texture* texture);

    ~BrushStroke();

    Texture* GetBrushStrokeTexture();

    Texture* GetUndoTexture();

    void SetBrushStrokeTexture(Texture* texture);

    void SetUndoTexture(Texture* texture);
};

