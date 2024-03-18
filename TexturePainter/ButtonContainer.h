#pragma once

#include <vector>
#include <functional>

#include "Texture.h"
#include "TexturePainter.h"

// holds the buttons in related groups
class ButtonContainer
{
private:
    // poition of container
    int xPos{ 0 };
    int yPos{ 0 };
    int columns{ 0 };
    int rows{ 0 };
    int spacing{ 0 };

public:
    std::function<void(int, int, int, int, short)> DrawRectangle;
    std::function<void(int, int, Texture*)> DrawTexture;

private:
    struct Button
    {
        int xPos;
        int yPos;
        int width;
        int height;
        short colour;
        Texture* texture;
        // Function pointer type that takes no arguments and returns void
        std::function<void()> OnClick;

        
        // Constructor with width and height
        Button(int x, int y, int w, int h, short c, std::function<void()> onClickFunction)
            : xPos{ x }, yPos{ y }, texture{ nullptr }, width{ w }, height{ h }, colour{ c }, OnClick(onClickFunction) { }

        // Constructor with texture
        Button(int x, int y, Texture* tex, short c, std::function<void()> onClickFunction)
            : xPos{ x }, yPos{ y }, texture{ tex }, width{ tex->GetWidth() }, height{ tex->GetHeight() }, colour{ c }, OnClick(onClickFunction) { }

        // Deleted copy constructor and copy assignment operator as not needed
        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;

        ~Button()
        {
            delete texture;
        }

        // Simulate the button being clicked
        void Clicked() {
            if (OnClick) { // Check if the function pointer is not null
                OnClick(); // Call the function
            }
        }

        // Determine if a mouse click is on the button
        bool IsMouseClickOnButton(int x, int y) {
            return x >= xPos && x < (xPos + width) && y >= yPos && y < (yPos + height);
        }
    };
    std::vector<Button*> buttons{};
    TexturePainter& drawingClass;

public:
    ButtonContainer(TexturePainter& drawer, int x, int y, int col, int row, int space = 1);

    // Deleted copy constructor and copy assignment operator as not needed
    ButtonContainer(const ButtonContainer&) = delete;
    ButtonContainer& operator=(const ButtonContainer&) = delete;

    bool AddButton(int width, int height, short colour, std::function<void()> onClickFunction);

    void HandleMouseClick(int mouseX, int mouseY);

    void DrawButtons();

};

