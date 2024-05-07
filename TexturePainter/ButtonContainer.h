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
        Texture* textureFunction1;
        Texture* textureFunction2;
        Texture* currentTexture;
        std::vector<std::function<void()>> clickFunctions;

        // Pointer to the external boolean value - will effect appearance
        bool* externalBoolPtrAppearance;

        // list of tools to activate button - if empty active at all times
        std::vector <ToolType> activationToolList;
        bool active; // can button be clicked

        // primary constructor
        Button(int x, int y, bool highlight, int w, int h, bool toggle, Texture* tex1, Texture* tex2, short c, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2, bool initState)
            : xPos{ x }, yPos{ y }, highlightable{ highlight }, toggleable{ toggle }, width{ w }, height{ h }, colour{ c }, active {true},
            textureFunction1{ tex1 }, textureFunction2{ tex2 }, currentTexture{ initState ? tex1 : tex2 }, toggleState{ initState }, externalBoolPtrAppearance{ nullptr }
        {
            if (onClickFunction1)
                clickFunctions.push_back(onClickFunction1);
            if (onClickFunction2)
                clickFunctions.push_back(onClickFunction2);
        }

        // For two different textures, able to toggle
        Button(int x, int y, bool highlightable, bool toggleable, Texture* tex1, Texture* tex2, std::function<void()> onClickFunction1 = nullptr, std::function<void()> onClickFunction2 = nullptr, bool initState = false)
            : Button(x, y, highlightable, tex1->GetWidth(), tex1->GetHeight(), toggleable, tex1, tex2, 0, onClickFunction1, onClickFunction2, initState) {}

        // For a single texture, unable to toggle
        Button(int x, int y, bool highlightable, Texture* tex, std::function<void()> onClickFunction)
            : Button(x, y, highlightable, tex->GetWidth(), tex->GetHeight(), false, tex, tex, 0, onClickFunction, nullptr, false) {}

        // For dimensions and a color, unable to toggle
        Button(int x, int y, bool highlightable, int w, int h, short c, std::function<void()> onClickFunction)
            : Button(x, y, highlightable, w, h, false, nullptr, nullptr, c, onClickFunction, nullptr, false) {}

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
            size_t numFunctions = clickFunctions.size();

            if (!active || numFunctions == 0) // If the button is not active or no functions, exit
                return;

            if (numFunctions == 1)
            {
                auto& functionToCall = clickFunctions.at(0);
                if (functionToCall) // Check if the function is not nullptr
                    functionToCall(); // Call the function
            }
            else if (numFunctions == 2)
            {
                auto& functionToCall = toggleState ? clickFunctions.at(1) : clickFunctions.at(0);
                if (functionToCall) // Check if the function is not nullptr
                    functionToCall(); // Call the function
            }

            ToggleButton();
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
            if (toggleable && textureFunction2 != textureFunction1 && currentTexture != nullptr) { 
                toggleState = !toggleState;
                currentTexture = toggleState ? textureFunction1 : textureFunction2;
            }
        }

        // change the look of a button without clicking on it (regardless if toggleable)
        void UpdateButtonTexture(bool state)
        {
            // Check if toggle effect is intended
            if (textureFunction2 != textureFunction1) {
                currentTexture = state ? textureFunction2 : textureFunction1;
            }
        }

        // change the texture and toggleState from external bool
        bool UpdateTextureAppearanceFromExternalBool()
        {
            if (externalBoolPtrAppearance == nullptr) {
                // failure if the pointer is nullptr
                return false;
            }

            // Dereference the pointer and check its value
            if (*externalBoolPtrAppearance) {
                // Assign texture and toggle state accordingly
                currentTexture = textureFunction2;
                toggleState = true;
            }
            else {
                currentTexture = textureFunction1;
                toggleState = false;
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

    bool AddButton(bool highlightable, bool toggleable, Texture* offTexture, Texture* onTexture, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2 = nullptr);

    bool AddExternalBoolPtr(bool* externalBoolean);

    bool AddNewToolToButtonActivateList(ToolType newTool);

    void UpdatePosition(int buttonId, int width, int height);

    void HandleMouseClick(COORD mouseCoord);

    bool MouseOverButton(COORD mouseCoord);

    void UpdateButtonActive(ToolType currentToolType);

    void UpdateButtonAppearance();

    void DrawButtons(COORD mousePosition);

};

