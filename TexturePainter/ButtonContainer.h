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
        bool toggleState;
        short colour;
        Texture* texture;
        Texture* texture2;
        // Function pointer type that takes no arguments and returns void
        std::function<void()> OnClick;

        // Primary constructor
        Button(int x, int y, bool highlight, int w, int h, short c, Texture* iconTexture = nullptr, Texture* iconTexture2 = nullptr, std::function<void()> onClickFunction = nullptr)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, width{ w }, height{ h }, colour{ c },
            texture{ iconTexture }, texture2{ iconTexture2 }, OnClick(onClickFunction), toggleState{ false } {}

        // Constructor with width and height only
        Button(int x, int y, bool highlight, int w, int h, short c, std::function<void()> onClickFunction)
            : Button(x, y, highlight, w, h, c, nullptr, nullptr, onClickFunction) { }

        // Constructor with a single texture
        Button(int x, int y, bool highlight, Texture* iconTexture, std::function<void()> onClickFunction)
            : Button(x, y, highlight, iconTexture->GetWidth(), iconTexture->GetHeight(), FG_DARK_BLUE, iconTexture, nullptr, onClickFunction) { }

        // Constructor with two textures
        Button(int x, int y, bool highlight, Texture* iconTexture, Texture* iconTexture2, std::function<void()> onClickFunction)
            : Button(x, y, highlight, iconTexture->GetWidth(), iconTexture->GetHeight(), FG_DARK_BLUE, iconTexture, iconTexture2, onClickFunction) { }


        // Deleted copy constructor and copy assignment operator as not needed
        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;

        ~Button()
        {
            delete texture;
            delete texture2;
        }

        // when button clicked
        void Clicked() {
            if (OnClick) { // Check if the function pointer is not null
                OnClick(); // Call the function
                ToggleButton();
            }
        }

        // Determine if a mouse click is on the button
        bool IsMouseClickOnButton(COORD mouseClick) {
            return mouseClick.X >= xPos && mouseClick.X < (xPos + width) && mouseClick.Y >= yPos && mouseClick.Y < (yPos + height);
        }

        // toggle
        void ToggleButton() {
            if (texture2 != nullptr) // only toggles if two textures
            {
                std::swap(texture, texture2);
                toggleState = !toggleState;
            }
        }

        void SetButtonState(bool state) {
            if (texture2 != nullptr) // only toggles if two textures
            {
                if (state)
                std::swap(texture, texture2);
            }
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

    bool AddButton(bool highlightable, Texture* texture, Texture* texture2, std::function<void()> onClickFunction);

    void HandleMouseClick(COORD);

    void SetButtonAppearance(int buttonNumber, bool state);

    void DrawButtons();

};

