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
bool ButtonContainer::AddButton(bool highlightable, bool toggleable, Texture* offTexture, Texture* onTexture, std::function<void()> onClickFunction) {
    int buttonId = buttons.size();
    if (buttonId >= rows * columns) {
        drawingClass.AddToLog(L"Too many buttons added to ButtonContainer.");
        return false;
    }

    int height = offTexture->GetHeight();
    int width = onTexture->GetWidth();
    UpdatePosition(buttonId, width, height);
    Button* newButton = new Button(nextXPos - width, nextYPos, highlightable, toggleable, offTexture, onTexture, onClickFunction);
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

// update a particular button
void ButtonContainer::UpdateButtonAppearance(size_t buttonIndex, bool state)
{
    if (buttonIndex < buttons.size())
        buttons.at(buttonIndex)->UpdateButtonTexture(state);
    else
        drawingClass.AddToLog(L"Attempted to set appearance of a button outside button container vector index");
}

void ButtonContainer::DrawButtons(COORD mousePosition)
{
    for (const Button* button : buttons) {
        if (button->texture == nullptr)
            drawingClass.DrawRectangleEdgeLength(button->xPos, button->yPos, button->width, button->height, button->colour, true, PIXEL_SOLID);
        else
            drawingClass.DrawTextureToScreen(*button->texture, button->xPos, button->yPos, 1, true);

        // draw default border
        drawingClass.DrawRectangleEdgeLength(button->xPos - 1, button->yPos - 1, button->width + 2, button->height + 2, background, false, PIXEL_HALF);

        // highlight if button clicked
        if (button->IsMouseClickOnButton(mousePosition) && drawingClass.GetLeftMouseHeld())
            drawingClass.DrawRectangleEdgeLength(button->xPos, button->yPos, button->width, button->height, FG_GREEN, true, PIXEL_HALF);

    }

    // highlight last clicked highlightable button in the container (size_t)
    if (highlighted < buttons.size())
        drawingClass.DrawRectangleEdgeLength(buttons.at(highlighted)->xPos - 1, buttons.at(highlighted)->yPos - 1, buttons.at(highlighted)->width + 2, buttons.at(highlighted)->height + 2, TexturePainter::HIGHLIGHT_COLOUR, false, PIXEL_QUARTER);
    else
        // log an error if `highlighted` is out of bounds
        drawingClass.AddToLog(L"Highlighted button index out of range");



}