#include "TexturePainter.h" // need to include full class declaration here (only forward dec. in ButtonContainer.h file)
#include "ButtonContainer.h"

ButtonContainer::ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, short colour, int space)
    : drawingClass{ drawer }, xPos{ x }, yPos{ y }, lastClicked{ 0 }, columns{ col }, rows{ row },
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

        // if 1st button
        if (columnPosition == 0 && rowPosition == 0)
        {
            nextXPos = xPos + spacing;
            nextYPos = yPos + spacing;
            tallestInRow = height;
        }
        // if starting new row
        else if (columnPosition == 0) 
        {
            nextXPos = xPos + spacing;
            nextYPos += tallestInRow + spacing;
            tallestInRow = height; // first height the tallest
        }
        // keep track of tallest button in row
        else 
            if (height > tallestInRow)
                tallestInRow = height;

        // add button
        Button* newButton = new Button(nextXPos, nextYPos, width, height, colour, onClickFunction);
        buttons.push_back(newButton);

        nextXPos += width + spacing; // Prepare nextXPos for the next button in the same row

        return true;
    }
}

// button made with a texture
bool ButtonContainer::AddButton(Texture* iconTexture, std::function<void()> onClickFunction)
{

    int buttonId = buttons.size();
    if (buttonId >= rows * columns)
    {
        drawingClass.AddToLog(L"Too many buttons added to ButtonContainer.");
        return false;
    }
    else
    {

        int height{ iconTexture->GetHeight() };
        int width{ iconTexture->GetWidth() };
        // Calculate the column and row position for this button based on its ID
        int columnPosition = buttonId % columns;
        int rowPosition = buttonId / columns;  // Integer division will naturally floor the result

        // if 1st button
        if (columnPosition == 0 && rowPosition == 0)
        {
            nextXPos = xPos + spacing;
            nextYPos = yPos + spacing;
            tallestInRow = height;
        }
        // if starting new row
        else if (columnPosition == 0)
        {
            nextXPos = xPos + spacing;
            nextYPos += tallestInRow + spacing;
            tallestInRow = height; // first height the tallest
        }
        // keep track of tallest button in row
        else
            if (height > tallestInRow)
                tallestInRow = height;

        // add button
        Button* newButton = new Button(nextXPos, nextYPos, iconTexture, onClickFunction);
        buttons.push_back(newButton);

        nextXPos += width + spacing; // Prepare nextXPos for the next button in the same row

        return true;
    }
}


void ButtonContainer::HandleMouseClick(int mouseX, int mouseY)
{
    int count{ 0 };
    for (Button* button : buttons)
    {
        if (button->IsMouseClickOnButton(mouseX, mouseY))
        {
            button->Clicked();
            lastClicked = count;
            break;
        }
        count++;
    }
}

void ButtonContainer::DrawButtons()
{
    for (const Button* button : buttons)
    {
        if (button->texture == nullptr)
            drawingClass.DrawRectangleEdgeLength(button->xPos, button->yPos, button->width, button->height, button->colour, true, PIXEL_SOLID);
        else
            drawingClass.DrawTextureToScreen(*button->texture, button->xPos, button->yPos, 1, true);

        drawingClass.DrawRectangleEdgeLength(button->xPos - 1, button->yPos - 1, button->width + 2, button->height + 2, background, false, PIXEL_HALF);
    }
    // highlight last clicked
    drawingClass.DrawRectangleEdgeLength(buttons.at(lastClicked)->xPos - 1, buttons.at(lastClicked)->yPos - 1, buttons.at(lastClicked)->width + 2, buttons.at(lastClicked)->height + 2, TexturePainter::HIGHLIGHT_COLOUR, false, PIXEL_QUARTER);

}
