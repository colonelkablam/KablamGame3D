#include "TexturePainter.h" // need to include full class declaration here (only forward dec. in ButtonContainer.h file)
#include "ButtonContainer.h"

ButtonContainer::ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, short colour, int space)
    : drawingClass{ drawer }, xPos{ x }, yPos{ y }, highlighted{ 0 }, columns{ col }, rows{ row },
    background{ colour }, spacing{ space }, nextXPos { 0 }, nextYPos{ 0 }, tallestInRow{ 0 } {}

ButtonContainer::~ButtonContainer()
{
    for (Button* button : buttons)
    {
        delete button;
        button = nullptr;
    }
}

// button made with dimentions and colour
bool ButtonContainer::AddButton(bool highlightable, int width, int height, short colour, std::function<void()> onClickFunction) {
    int buttonId = buttons.size();
    if (buttonId >= rows * columns) {
        drawingClass.AddToLog(L"Too many buttons added to ButtonContainer.");
        return false;
    }

    UpdatePosition(buttonId, width, height);
    Button* newButton = new Button(nextXPos - width, nextYPos, highlightable, width, height, colour, onClickFunction);
    buttons.push_back(newButton);
    return true;
}

// button made with one texture
bool ButtonContainer::AddButton(bool highlightable, Texture* iconTexture, std::function<void()> onClickFunction) {
    int buttonId = buttons.size();
    if (buttonId >= rows * columns) {
        drawingClass.AddToLog(L"Too many buttons added to ButtonContainer.");
        return false;
    }

    int height = iconTexture->GetHeight();
    int width = iconTexture->GetWidth();
    UpdatePosition(buttonId, width, height);
    Button* newButton = new Button(nextXPos - width, nextYPos, highlightable, iconTexture, onClickFunction);
    buttons.push_back(newButton);
    return true;
}

// button made with two textures
bool ButtonContainer::AddButton(bool highlightable, bool toggleable, Texture* offTexture, Texture* onTexture, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2) {
    int buttonId = buttons.size();
    if (buttonId >= rows * columns) {
        drawingClass.AddToLog(L"Too many buttons added to ButtonContainer.");
        return false;
    }

    int height = offTexture->GetHeight();
    int width = onTexture->GetWidth();
    UpdatePosition(buttonId, width, height);
    Button* newButton = new Button(nextXPos - width, nextYPos, highlightable, toggleable, offTexture, onTexture, onClickFunction1, onClickFunction2);
    buttons.push_back(newButton);
    return true;
}

// helper for AddButton methods
void ButtonContainer::UpdatePosition(int buttonId, int width, int height) {
    int columnPosition = buttonId % columns;
    int rowPosition = buttonId / columns;

    if (columnPosition == 0 && rowPosition == 0) {
        nextXPos = xPos + spacing;
        nextYPos = yPos + spacing;
        tallestInRow = height;
    }
    else if (columnPosition == 0) {
        nextXPos = xPos + spacing;
        nextYPos += tallestInRow + spacing;
        tallestInRow = height;
    }
    else if (height > tallestInRow) {
        tallestInRow = height;
    }

    nextXPos += width + spacing; // Update position for next button
}

bool ButtonContainer::AddExternalBoolPtr(bool* externalBoolean)
{
    if (externalBoolean == nullptr)
    {
        drawingClass.AddToLog(L"Unable to add external boolean to button (index " + std::to_wstring(buttons.size() - 1) + L"); nullptr recieved.");
        return false;
    }
    else if (buttons.empty()) {
        drawingClass.AddToLog(L"Unable to add external boolean to button; buttons container is empty.");
        return false;
    }
    else {
        // Set the external boolean pointer for the most recently added button
        buttons.back()->SetExternalAppearanceBool(externalBoolean);
        return true;
    }
}

// Add list of toolTypes that activate button to last button added
bool ButtonContainer::AddNewToolToButtonActivateList(ToolType newTool)
{
    buttons.back()->AddNewToolToActivateList(newTool);
    return true;
}

void ButtonContainer::HandleMouseClick(COORD mouseCoord)
{
    int count{ 0 };
    for (Button* button : buttons)
    {
        if (button->IsMouseClickOnButton(mouseCoord))
        {
            mouseClickPosition = mouseCoord;
            button->Clicked();
            if (button->highlightable)
                highlighted = count;

            drawingClass.DrawRectangleEdgeLength(buttons.at(count)->xPos - 1, buttons.at(count)->yPos - 1, buttons.at(count)->width + 2, buttons.at(count)->height + 2, FG_DARK_GREEN, true, PIXEL_QUARTER);
            break;
        }
        count++;
    }
}

void ButtonContainer::UpdateButtonActive(ToolType currentToolType)
{
    for (Button* button : buttons)
    {
        button->UpdateIfActive(currentToolType);
    }
}

// update buttons
void ButtonContainer::UpdateButtonAppearance()
{
    for (Button* button : buttons)
    {
        button->UpdateTextureAppearanceFromExternalBool();
    }
}

void ButtonContainer::DrawButtons(COORD mousePosition)
{
    for (const Button* button : buttons) {
        if (button->currentTexture == nullptr)
            drawingClass.DrawRectangleEdgeLength(button->xPos, button->yPos, button->width, button->height, button->colour, true, button->IsButtonActive() ? PIXEL_SOLID : PIXEL_HALF);
        else
            drawingClass.DrawTextureToScreen(*button->currentTexture, button->xPos, button->yPos, 1, true, !button->IsButtonActive());

        // draw default border
        drawingClass.DrawRectangleEdgeLength(button->xPos - 1, button->yPos - 1, button->width + 2, button->height + 2, background, false, PIXEL_HALF);

        // highlight if button clicked
        if (button->IsMouseClickOnButton(mousePosition) && drawingClass.GetLeftMouseHeld() && button->IsButtonActive())
            drawingClass.DrawRectangleEdgeLength(button->xPos, button->yPos, button->width, button->height, FG_GREEN, true, PIXEL_HALF);

    }

    // highlight last clicked highlightable button in the container (size_t)
    if (highlighted < buttons.size())
        drawingClass.DrawRectangleEdgeLength(buttons.at(highlighted)->xPos - 1, buttons.at(highlighted)->yPos - 1, buttons.at(highlighted)->width + 2, buttons.at(highlighted)->height + 2, TexturePainter::HIGHLIGHT_COLOUR, false, PIXEL_QUARTER);
    else
        // log an error if `highlighted` is out of bounds
        drawingClass.AddToLog(L"Highlighted button index out of range");



}