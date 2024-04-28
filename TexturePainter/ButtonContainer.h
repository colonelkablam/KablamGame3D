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
        bool toggleable;
        bool toggleState;
        short colour;
        Texture* onTexture;
        Texture* offTexture;
        Texture* texture;
        std::function<void()> OnClick;

        // Primary constructor for two different textures
        Button(int x, int y, bool highlight, bool toggle, int w, int h, short c, Texture* onTex, Texture* offTex, std::function<void()> onClickFunction = nullptr)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, toggleable{ toggle }, width{ w }, height{ h }, colour{ c },
            onTexture{ onTex }, offTexture{ offTex }, texture{ onTex }, OnClick(onClickFunction), toggleState{ false } {}

        // Constructor for a single texture (no toggle effect)
        Button(int x, int y, bool highlight, bool toggle, int w, int h, short c, Texture* tex, std::function<void()> onClickFunction = nullptr)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, toggleable{ toggle }, width{ w }, height{ h }, colour{ c },
            onTexture{ tex }, offTexture{ tex }, texture{ tex }, OnClick(onClickFunction), toggleState{ false } {}

        ~Button() {
            // Delete only if onTexture and offTexture are different
            if (onTexture != offTexture) {
                delete onTexture;
            }
            delete offTexture;
        }

        void Clicked() {
            if (OnClick) {
                OnClick(); // Invoke the onClick handler if set
                ToggleButton();
            }
        }

        bool IsMouseClickOnButton(COORD mouseClick) const {
            return mouseClick.X >= xPos && mouseClick.X < (xPos + width) &&
                mouseClick.Y >= yPos && mouseClick.Y < (yPos + height);
        }

        void ToggleButton() {
            if (toggleable && onTexture != offTexture) { // Check if two distinct textures are available
                toggleState = !toggleState;
                texture = toggleState ? onTexture : offTexture;
            }
        }

        void SetButtonTexture(bool state) {
            if (onTexture != offTexture) { // Check if toggle effect is intended
                texture = state ? onTexture : offTexture;
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

    bool AddButton(bool highlightable, bool toggleable, int width, int height, short colour, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, bool toggleable, Texture* texture, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, bool toggleable, Texture* texture, Texture* texture2, std::function<void()> onClickFunction);

    void HandleMouseClick(COORD);

    void SetButtonAppearance(int buttonNumber, bool state);

    void DrawButtons();

};

