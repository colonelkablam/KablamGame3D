#pragma once

#include <Windows.h>
#include <vector>
#include <functional>

#include "Texture.h"
#include "ToolTypeEnums.h"

// button currently a struct rather than own class - might change but currently accessing members directly
class Button
{
private:
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

public:
    // primary constructor
    Button(int x, int y, bool highlight, int w, int h, bool toggle, Texture* tex1, Texture* tex2, short c, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2, bool initState);

    // For two different textures, able to toggle, can take two different function pointers
    Button(int x, int y, bool highlightable, bool toggleable, Texture* tex1, Texture* tex2, std::function<void()> onClickFunction1 = nullptr, std::function<void()> onClickFunction2 = nullptr, bool initState = false);

    // For a single texture, unable to toggle
    Button(int x, int y, bool highlightable, Texture* tex, std::function<void()> onClickFunction);

    // For dimensions and a color, unable to toggle
    Button(int x, int y, bool highlightable, int w, int h, short c, std::function<void()> onClickFunction);
    
    ~Button();

    void SetExternalAppearanceBool(bool* ptr);

    void AddNewToolToActivateList(const ToolType& newTool);

    void UpdateIfActive(const ToolType& currentTool);

    int GetXPos() const;

    int GetYPos() const;

    int GetWidth() const;

    int GetHeight() const;

    Texture* GetCurrentTexture() const;

    short GetColour() const;

    bool GetHighlightable() const;

    bool GetActive() const;

    void Clicked();

    bool IsMouseClickOnButton(const COORD& mouseClick) const;

    // change the look and toggle when clicked on
    void ToggleButton();

    // change the state of a button without clicking on it (regardless if toggleable)
    void SetTextureWithBool(bool textureState);

    // change the state and toggleState from external bool
    bool UpdateStateFromExternalBool();
};

