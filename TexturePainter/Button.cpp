#include "Button.h"


// primary constructor
Button::Button(int x, int y, bool highlight, int w, int h, bool toggle, Texture* tex1, Texture* tex2, short c, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2, bool initState)
    : xPos{ x }, yPos{ y }, highlightable{ highlight }, toggleable{ toggle }, width{ w }, height{ h }, colour{ c }, active{ true },
    textureFunction1{ tex1 }, textureFunction2{ tex2 }, currentTexture{ initState ? tex1 : tex2 }, toggleState{ initState }, externalBoolPtrAppearance{ nullptr }
{
    if (onClickFunction1)
        clickFunctions.push_back(onClickFunction1);
    if (onClickFunction2)
        clickFunctions.push_back(onClickFunction2);
}

// For two different textures, able to toggle
Button::Button(int x, int y, bool highlightable, bool toggleable, Texture* tex1, Texture* tex2, std::function<void()> onClickFunction1, std::function<void()> onClickFunction2, bool initState)
    : Button(x, y, highlightable, tex1->GetWidth(), tex1->GetHeight(), toggleable, tex1, tex2, 0, onClickFunction1, onClickFunction2, initState) {}

// For a single texture, unable to toggle
Button::Button(int x, int y, bool highlightable, Texture* tex, std::function<void()> onClickFunction)
    : Button(x, y, highlightable, tex->GetWidth(), tex->GetHeight(), false, tex, tex, 0, onClickFunction, nullptr, false) {}

// For dimensions and a color, unable to toggle
Button::Button(int x, int y, bool highlightable, int w, int h, short c, std::function<void()> onClickFunction)
    : Button(x, y, highlightable, w, h, false, nullptr, nullptr, c, onClickFunction, nullptr, false) {}

Button::~Button() {
    // Do not delete pointers here as they are sourced from a static pointer 
}

void Button::SetExternalAppearanceBool(bool* ptr)
{
    externalBoolPtrAppearance = ptr;
}

void Button::AddNewToolToActivateList(const ToolType& newTool)
{
    // deactivate button by defalult
    active = false;
    activationToolList.push_back(newTool);
}

void Button::UpdateIfActive(const ToolType& currentTool)
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

int Button::GetXPos() const
{
    return xPos;
}

int Button::GetYPos() const
{
    return yPos;
}

int Button::GetWidth() const
{
    return width;
}

int Button::GetHeight() const
{
    return height;
}

Texture* Button::GetCurrentTexture() const
{
    return currentTexture;
}

short Button::GetColour() const
{
    return colour;
}

bool Button::GetHighlightable() const
{
    return highlightable;
}

bool Button::GetActive() const
{
    return active;
}

void Button::Clicked()
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

bool Button::IsMouseClickOnButton(const COORD& mouseClick) const
{
    return mouseClick.X >= xPos && mouseClick.X < (xPos + width) &&
        mouseClick.Y >= yPos && mouseClick.Y < (yPos + height);
}

// change the look and toggle when clicked on
void Button::ToggleButton()
{
    // Check if two distinct textures are available AND not a colour button
    if (toggleable && textureFunction2 != textureFunction1 && currentTexture != nullptr)
    {
        toggleState = !toggleState;
        currentTexture = toggleState ? textureFunction1 : textureFunction2;
    }
}

// change the look of a button without clicking on it (regardless if toggleable)
void Button::SetTextureWithBool(bool textureState)
{
    // Check if toggle effect is intended
    if (textureFunction2 != textureFunction1) {
        currentTexture = textureState ? textureFunction2 : textureFunction1;
        if (toggleable)
            toggleState = !toggleState;
    }
}

// change the texture and toggleState from external bool
bool Button::UpdateStateFromExternalBool()
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
