#pragma once

#include "KablamEngine.h"

#include "Canvas.h"
#include "ButtonContainer.h"

class TexturePainter : public KablamEngine
{
    // member attributes TexturePainter

public:
    static const short HIGHLIGHT_COLOUR = FG_RED;
private:
    static const int MIN_TEXTURE_SIZE = 4;
    static const int MAX_TEXTURE_SIZE = 64;

    const std::wstring SAVE_FOLDER = L"Textures\\"; // relative path to subDir
    const std::wstring TEXTURE_EXTENSION = L".txr";
    const int CANVAS_XPOS = 16;
    const int CANVAS_YPOS = 23;

    const int COLOUR_BUTTON_XPOS = 2;
    const int COLOUR_BUTTON_YPOS = 22;
    const int COLOUR_BUTTON_SIZE = 5;

    const int BRUSH_BUTTON_XPOS = 15;
    const int BRUSH_BUTTON_YPOS = 13;
    const int BRUSH_BUTTON_SIZE = 5;



    // vector for storing canvases and textures together for editing
    std::vector<Canvas*> canvases;
    int nCurrentCanvas = -1;
    // ptrs to manage current selected canvas and texture
    Canvas* currentCanvas;

    // button containers
    ButtonContainer* colourButtonsContainer;
    ButtonContainer* brushButtonsContainer;

    // containers for icons
    Texture* deleteToolIcon;
    Texture* blockToolIcon;
    Texture* increaseToolIcon;
    Texture* decreaseToolIcon;
    Texture* rectToolIcon;
    Texture* rectFillToolIcon;
    Texture* lineToolIcon;

public:
    // constructor
    TexturePainter(std::wstring title = L"New Unnamed Project");

    // destructor
    ~TexturePainter();

    // member methods for TexturePainter

    bool GetUserStartInput(); // needed to load up textures


protected:
    // virtual methods from KablamEngine to be defined
    virtual bool OnGameCreate();
    virtual bool OnGameUpdate(float fElapsedTime);

    // Optional for clean up 
    //virtual bool OnGameDestroy() { return true; }

private:

    bool InitCanvasExistingTexture(const std::wstring& fileName);

    bool InitCanvasNewTexture(int width, int height, int illuminated, const std::wstring& fileName);

    bool ChangeCanvas(size_t index);

    void DrawHeadingInfo(int x, int y);

    void DrawToolInfo(int x, int y);

    void DrawButtons();

    bool HandleKeyPress();

}; // end of TexturePainter class definition

