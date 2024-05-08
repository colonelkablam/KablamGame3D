#pragma once

#include <functional>
#include <vector>

#include "Texture.h"
#include "Button.h"
#include "ToolTypeEnums.h"

class TexturePainter; // forward declaration rather than header file (creates circular dependency)
// holds the buttons in related groups

class ButtonContainer
{
private:
    // poition of container
    int xPos;
    int yPos;

    size_t highlighted;
    COORD mouseClickPosition;
    short bgColour;

    // handling button placements
    int nextXPos;
    int nextYPos;
    int tallestInRow;
    int columns;
    int rows;
    int spacing;

    // button storage
    std::vector<Button*> buttons;

    // Dependency Injection Pattern chosen...
    TexturePainter& drawingClass;

public:
    ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, short bgColour = FG_DARK_GREY, int space = 1 );

    // Deleted copy constructor and copy assignment operator as not needed
    ButtonContainer(const ButtonContainer&) = delete;
    ButtonContainer& operator=(const ButtonContainer&) = delete;

    ~ButtonContainer();

    bool AddButton(bool highlightable, int width, int height, short colour, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, Texture* texture, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, bool toggleable, Texture* offTexture, Texture* onTexture, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2 = nullptr);

    bool AddExternalBoolPtr(bool* externalBoolean);

    bool AddNewToolToButtonActivateList(ToolType newTool);

    void UpdatePosition(int buttonId, int width, int height);

    void HandleMouseClick(COORD mouseCoord);

    bool MouseOverButton(COORD mouseCoord);

    void UpdateButtonActive(ToolType currentToolType);

    void UpdateButtonAppearance();

    void DrawButtons(COORD mousePosition);

};

