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

    size_t highlighted;
    COORD mouseClickPosition;
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
        // will be sourced from a shared static pointer to textures in the Canvas class
        Texture* offTexture;
        Texture* onTexture;
        Texture* texture;
        std::function<void()> OnClick;

        // primary constructor
        Button(int x, int y, bool highlight, int w, int h, bool toggle, Texture* offTex, Texture* onTex, short c, std::function<void()> onClickFunction, bool initState)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, toggleable{ toggle }, width{ w }, height{ h }, colour{ c },
            onTexture{ onTex }, offTexture{ offTex }, texture{ initState ? onTex : offTex }, OnClick(onClickFunction), toggleState{ initState } {}

        // For two different textures, able to toggle
        Button(int x, int y, bool highlight, bool toggle, Texture* offTex, Texture* onTex, std::function<void()> onClickFunction = nullptr, bool initState = false)
            : Button(x, y, highlight, onTex->GetWidth(), onTex->GetHeight(), toggle, offTex, onTex, 0, onClickFunction, initState) {}

        // For a single texture, unable to toggle
        Button(int x, int y, bool highlight, Texture* tex, std::function<void()> onClickFunction = nullptr, bool initState = false)
            : Button(x, y, highlight, tex->GetWidth(), tex->GetHeight(), false, tex, tex, 0, onClickFunction, initState) {}

        // For dimensions and a color, unable to toggle
        Button(int x, int y, bool highlight, int w, int h, short c, std::function<void()> onClickFunction = nullptr, bool initState = false)
            : Button(x, y, highlight, w, h, false, nullptr, nullptr, c, onClickFunction, initState) {}

        ~Button() {
            // Do not delete pointers here as they are sourced from a static pointer 
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

        // change the look and toggle when clicked on
        void ToggleButton() {
            // Check if two distinct textures are available AND not a colour button
            if (toggleable && onTexture != offTexture && texture != nullptr) { 
                toggleState = !toggleState;
                texture = toggleState ? onTexture : offTexture;
            }
        }

        // change the look of a button without clicking on it (regardless if toggleable)
        void UpdateButtonTexture(bool state) {
            // Check if toggle effect is intended
            if (onTexture != offTexture) {
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

    bool AddButton(bool highlightable, int width, int height, short colour, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, Texture* texture, std::function<void()> onClickFunction);

    bool AddButton(bool highlightable, bool toggleable, Texture* offTexture, Texture* onTexture, std::function<void()> onClickFunction);

    void UpdatePosition(int buttonId, int width, int height);

    void HandleMouseClick(COORD);

    void UpdateButtonAppearance(size_t buttonNumber, bool state);

    void DrawButtons(COORD mousePosition);

};

