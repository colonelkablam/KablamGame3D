#include "TexturePainter.h" // need to include full class declaration here (only forward dec. in ButtonContainer.h file)
#include "ButtonContainer.h"

ButtonContainer::ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, short colour, int space)
    : drawingClass{ drawer }, xPos{ x }, yPos{ y }, highlighted{ 0 }, columns{ col }, rows{ row },
    bgColour{ colour }, spacing{ space }, nextXPos{ 0 }, nextYPos{ 0 }, tallestInRow{ 0 }, mouseClickPosition{ 0,0 } {}

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
            if (button->GetHighlightable() && button->GetActive())
                highlighted = count;

            break;
        }
        count++;
    }
}

bool ButtonContainer::MouseOverButton(COORD mouseCoord)
{
    for (Button* button : buttons)
    {
        if (button->IsMouseClickOnButton(mouseCoord))
            return true;
    }
    return false;
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
        button->UpdateStateFromExternalBool();
    }
}

void ButtonContainer::DrawButtons(COORD mousePosition)
{
    for (const Button* button : buttons) {
        if (button->GetCurrentTexture() == nullptr)
            drawingClass.DrawRectangleEdgeLength(button->GetXPos(), button->GetYPos(), button->GetWidth(), button->GetHeight(), button->GetColour(), true, button->GetActive() ? PIXEL_SOLID : PIXEL_QUARTER);
        else
            drawingClass.DrawTextureToScreen(*button->GetCurrentTexture(), button->GetXPos(), button->GetYPos(), 1, true, !button->GetActive());

        // draw default border
        drawingClass.DrawRectangleEdgeLength(button->GetXPos() - 1, button->GetYPos() - 1, button->GetWidth() + 2, button->GetHeight() + 2, bgColour, false, PIXEL_HALF);

        // highlight if button clicked
        if (button->IsMouseClickOnButton(mousePosition) && drawingClass.GetLeftMouseHeld() && button->GetActive())
        {
            drawingClass.DrawRectangleEdgeLength(button->GetXPos(), button->GetYPos(), button->GetWidth(), button->GetHeight(), FG_GREEN, true, PIXEL_HALF);
        }

    }

    // highlight last clicked highlightable button in the container (size_t)
    if (highlighted < buttons.size())
        drawingClass.DrawRectangleEdgeLength(   buttons.at(highlighted)->GetXPos() - 1,
                                                buttons.at(highlighted)->GetYPos() - 1,
                                                buttons.at(highlighted)->GetWidth() + 2, 
                                                buttons.at(highlighted)->GetHeight() + 2, 
                                                TexturePainter::HIGHLIGHT_COLOUR,
                                                false, 
                                                PIXEL_QUARTER   );
    else
        // log an error if `highlighted` is out of bounds
        drawingClass.AddToLog(L"Highlighted button index out of range");



}