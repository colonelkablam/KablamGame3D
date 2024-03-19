#include "TexturePainter.h" // need to include full class declaration here (only forward dec. in ButtonContainer.h file)
#include "ButtonContainer.h"

ButtonContainer::ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, int space)
    : drawingClass{ drawer }, xPos{ x }, yPos{ y }, columns{ col }, rows{ row }, spacing{ space } {}

ButtonContainer::~ButtonContainer()
{
    for (Button* button : buttons)
    {
        delete button;
        button = nullptr;
    }
}

bool ButtonContainer::AddButton(int width, int height, short colour, std::function<void()> onClickFunction)
{
    int buttonId = buttons.size();
    if (buttonId >= rows * columns)
    {
        drawingClass.AddToLog(L"Too many buttons added to ButtonContainer.");
        return false;
    }
    else
    {
        // Calculate the column and row position for this button based on its ID
        int columnPosition = buttonId % columns;
        int rowPosition = buttonId / columns;  // Integer division will naturally floor the result

        // Calculate the actual x and y positions based on the column and row positions
        int x = xPos + (columnPosition * (width + spacing));
        int y = yPos + (rowPosition * (height + spacing));

        Button* newButton = new Button(x, y, width, height, colour, onClickFunction);
        buttons.push_back(newButton);

        return true;
    }
}

void ButtonContainer::HandleMouseClick(int mouseX, int mouseY)
{
    for (Button* button : buttons)
        if (button->IsMouseClickOnButton(mouseX, mouseY))
        {
            button->Clicked();
            break;
        }
}

void ButtonContainer::DrawButtons()
{
    for (const Button* button : buttons)
    {
        if (button->texture == nullptr)
            drawingClass.DrawRectangleEdgeLength(button->xPos, button->yPos, button->width, button->height, button->colour, true, PIXEL_SOLID);
        else
            drawingClass.DrawTextureToScreen(button->texture, button->xPos, button->yPos, 1);
    }
}