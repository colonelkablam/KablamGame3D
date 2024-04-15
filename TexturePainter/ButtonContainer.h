#pragma once

#include <functional>
#include <vector>

#include "Texture.h"

class TexturePainter; // forward declaration rather than header file (creates circular dependency)

// holds the buttons in related groups
class ButtonContainer
{
private:
    // poition of container
    int xPos;
    int yPos;

    int lastClicked;
    short background;

    // handling button placements
    int nextXPos;
    int nextYPos;
    int tallestInRow;
    int columns;
    int rows;
    int spacing;

    struct Button
    {
        int xPos;
        int yPos;
        int width;
        int height;
        bool highlightable;
        short colour;
        Texture* texture;
        // Function pointer type that takes no arguments and returns void
        std::function<void()> OnClick;

        
        // Constructor with width and height
        Button(int x, int y, bool highlight, int w, int h, short c, std::function<void()> onClickFunction)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, texture{ nullptr }, width{ w }, height{ h }, colour{ c }, OnClick(onClickFunction) { }

        // Constructor with texture
        Button(int x, int y, bool highlight, Texture* iconTexture, std::function<void()> onClickFunction)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, texture{ iconTexture }, width{ iconTexture->GetWidth() },
                height{ iconTexture->GetHeight() }, colour{ FG_DARK_BLUE }, OnClick(onClickFunction) { }

        // Deleted copy constructor and copy assignment operator as not needed
        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;

        ~Button()
        {
            delete texture;
        }

        // when button clicked
        void Clicked() {
            if (OnClick) { // Check if the function pointer is not null
                OnClick(); // Call the function
            }
        }

        // Determine if a mouse click is on the button
        bool IsMouseClickOnButton(COORD mouseClick) {
            return mouseClick.X >= xPos && mouseClick.X < (xPos + width) && mouseClick.Y >= yPos && mouseClick.Y < (yPos + height);
        }
    };

    std::vector<Button*> buttons;

    // Dependency Injection Pattern chosen...
    TexturePainter& drawingClass;

public:
    ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, short background = FG_DARK_GREY, int space = 1 );

    // Deleted copy constructor and copy assignment operator as not needed
    ButtonContainer(const ButtonContainer&) = delete;
    ButtonContainer& operator=(const ButtonContainer&) = delete;

    ~ButtonContainer();

    bool AddButton(bool highlightable, int width, int height, short colour, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, Texture* texture, std::function<void()> onClickFunction);

    void HandleMouseClick(COORD);

    void ActivateLastClicked();

    void DrawButtons();

};

