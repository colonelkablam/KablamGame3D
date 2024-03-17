#pragma once

#include<functional>
#include "Utility.h"
#include "KablamEngine.h"
#include "Canvas.h"

class TexturePainter : public KablamEngine
{
    // member attributes TexturePainter
private:
    static const int MIN_TEXTURE_SIZE = 4;
    static const int MAX_TEXTURE_SIZE = 64;

    const std::wstring SAVE_FOLDER = L"Textures\\"; // relative path to subDir
    const std::wstring TEXTURE_EXTENSION = L".txr";
    const size_t CANVAS_XPOS = 16;
    const size_t CANVAS_YPOS = 22;

    const size_t COLOUR_BUTTON_XPOS = 2;
    const size_t COLOUR_BUTTON_YPOS = 20;
    const size_t COLOUR_BUTTON_SIZE = 5;

    const size_t BRUSH_BUTTON_XPOS = 2;
    const size_t BRUSH_BUTTON_YPOS = 10;
    const size_t BRUSH_BUTTON_SIZE = 5;



    // vector for storing canvases and textures together for editing
    std::vector<Canvas*> canvases;
    int nCurrentCanvas = -1;
    // ptrs to manage current selected canvas and texture
    Canvas* currentCanvas;

    struct Button {

        int xPos;
        int yPos;
        int size;
        short colour;
        // Function pointer type that takes no arguments and returns void
        std::function<void()> OnClick;

        // Constructor that allows setting the onClick function during button creation
        Button(int x, int y, int s, short c, std::function<void()> onClickFunction)
            : xPos{ x }, yPos{ y }, size{ s }, colour {c}, OnClick(onClickFunction)
        {
        
        }

        // Simulate the button being clicked
        void Clicked() {
            if (OnClick) { // Check if the function pointer is not null
                OnClick(); // Call the function
            }
        }

        // Determine if a mouse click is on the button
        bool IsMouseClickOnButton(int x, int y) {
            return x >= xPos && x <= (xPos + size) && y >= yPos && y <= (yPos + size);
        }
    };

    std::vector<Button> buttons;



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

    void ChangeCanvas(size_t index);

    void DrawCanvas();

    void DrawHeadingInfo(size_t x, size_t y);

    void DrawToolInfo(size_t x, size_t y);

    void DrawButtons();

    bool IsMouseWithinCanvas(size_t x, size_t y);

    void ApplyBrush(size_t x, size_t y);

    bool HandleKeyPress();

    bool CheckFolderExist(const std::wstring& folderPath);

    bool CreateFolder(const std::wstring& folderPath);


}; // end of TexturePainter class definition

