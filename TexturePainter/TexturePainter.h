#pragma once

#include "Utility.h"
#include "KablamEngine.h"
#include "Canvas.h"

class TexturePainter : public KablamEngine
{
    // member attributes for KablamEngine
private:
    static const int MIN_TEXTURE_SIZE = 4;
    static const int MAX_TEXTURE_SIZE = 64;

    const std::wstring SAVE_FOLDER = L"Textures\\"; // relative path to subDir
    const std::wstring TEXTURE_EXTENSION = L".txr";
    const size_t CANVAS_XPOS = 16;
    const size_t CANVAS_YPOS = 16;

    // vector for storing canvases and textures together for editing
    std::vector<Canvas*> canvases;

    int nCurrentCanvas = -1;

    // ptrs to manage current selected canvas and texture
    Canvas* currentCanvas;


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

    bool IsMouseWithinCanvas(int x, int y);

    COORD ConvertMouseCoordsToTextureCoords();

    void ApplyBrush(int x, int y);

    bool HandleKeyPress();

    bool CheckFolderExist(const std::wstring& folderPath);

    bool CreateFolder(const std::wstring& folderPath);


}; // end of TexturePainter class definition

