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
        // avoiding duplication for every canvas
        Texture* offTexture;
        Texture* onTexture;
        Texture* texture;
        std::function<void()> OnClick;

        // Pointer to the external boolean value - will effect appearance
        bool* externalBoolPtrAppearance;

        // list of tools to activate button - if empty active at all times
        std::vector <ToolType> activationToolList;
        bool active; // can button be clicked

        // primary constructor
        Button(int x, int y, bool highlight, int w, int h, bool toggle, Texture* offTex, Texture* onTex, short c, std::function<void()> onClickFunction, bool initState, bool* extBool = nullptr)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, toggleable{ toggle }, width{ w }, height{ h }, colour{ c }, active {false},
            onTexture{ onTex }, offTexture{ offTex }, texture{ initState ? onTex : offTex }, OnClick(onClickFunction), toggleState{ initState }, externalBoolPtrAppearance{ extBool } {}

        // For two different textures, able to toggle
        Button(int x, int y, bool highlightable, bool toggleable, Texture* offTex, Texture* onTex, std::function<void()> onClickFunction = nullptr, bool initState = false)
            : Button(x, y, highlightable, onTex->GetWidth(), onTex->GetHeight(), toggleable, offTex, onTex, 0, onClickFunction, initState) {}

        // For a single texture, unable to toggle
        Button(int x, int y, bool highlightable, Texture* tex, std::function<void()> onClickFunction = nullptr, bool initState = false)
            : Button(x, y, highlightable, tex->GetWidth(), tex->GetHeight(), false, tex, tex, 0, onClickFunction, initState) {}

        // For dimensions and a color, unable to toggle
        Button(int x, int y, bool highlightable, int w, int h, short c, std::function<void()> onClickFunction = nullptr, bool initState = false)
            : Button(x, y, highlightable, w, h, false, nullptr, nullptr, c, onClickFunction, initState) {}

        ~Button() {
            // Do not delete pointers here as they are sourced from a static pointer 
        }

        void SetExternalAppearanceBool(bool* ptr)
        {
            externalBoolPtrAppearance = ptr;
        }

        void AddNewToolToActivateList(const ToolType& newTool)
        {
            // deactivate button by defalult
            active = false;
            activationToolList.push_back(newTool);
        }

        void UpdateIfActive(const ToolType& currentTool)
        {
            // no toolTypes explicitly added so button always active
            if (activationToolList.empty())
            {
                active = true;
            }
            else // search through list to see if should be active or not
            {
                for (const ToolType& tool : activationToolList) {
                    if (tool == currentTool) {
                        active = true;
                        return;
                    }
                }
                active = false;
            }
        }

        bool IsButtonActive() const
        {
            return active;
        }

        void Clicked()
        {
            if (OnClick) {
                OnClick(); // Invoke the onClick handler if set
                ToggleButton();
            }
        }

        bool IsMouseClickOnButton(const COORD& mouseClick) const
        {
            return mouseClick.X >= xPos && mouseClick.X < (xPos + width) &&
                mouseClick.Y >= yPos && mouseClick.Y < (yPos + height);
        }

        // change the look and toggle when clicked on
        void ToggleButton()
        {
            // Check if two distinct textures are available AND not a colour button
            if (toggleable && onTexture != offTexture && texture != nullptr) { 
                toggleState = !toggleState;
                texture = toggleState ? onTexture : offTexture;
            }
        }

        // change the look of a button without clicking on it (regardless if toggleable)
        void UpdateButtonTexture(bool state)
        {
            // Check if toggle effect is intended
            if (onTexture != offTexture) {
                texture = state ? onTexture : offTexture;
            }
        }

        bool UpdateTextureApperanceFromExternalBool()
        {
            if (externalBoolPtrAppearance == nullptr) {
                // return false as no value assigned
                return false;
            }

            // safe to dereference the pointer
            if (*externalBoolPtrAppearance) {
                // Set to one texture if true
                texture = onTexture;
            }
            else {
                // Set to the off texture if false
                texture = offTexture;
            }
            return true;
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

    bool AddExternalBoolPtr(bool* externalBoolean);

    bool AddNewToolToButtonActivateList(ToolType newTool);

    void UpdatePosition(int buttonId, int width, int height);

    void HandleMouseClick(COORD);

    void UpdateButtonActive(ToolType currentToolType);

    void UpdateButtonAppearance();

    void DrawButtons(COORD mousePosition);

};

