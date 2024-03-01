#pragma once

#include "Utility.h"
#include "Texture.h"
#include "KablamEngine.h"

class TexturePainter : public KablamEngine
{
    // member attributes for KablamEngine
private:
    static const int MIN_TEXTURE_SIZE = 4;
    static const int MAX_TEXTURE_SIZE = 64;

    const std::wstring sSaveFolderName = L"Textures\\"; // relative path to subDir
    const std::wstring sExtensionName = L".txr";

    short currentColour = FG_WHITE;
    short currentGlyph = PIXEL_SOLID;

    // display
    struct Canvas {
        std::wstring fileName;
        std::wstring filePath;
        int illumination;
        int xPos;
        int yPos;
        int width; 
        int height;
        Texture* texture;
    };

    // vector for storing canvases and textures together for editing
    std::vector<Canvas> canvases;

    int nCurrentCanvas = -1;


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

    bool InitCanvasNewTexture(int width, int height, bool illuminated, const std::wstring& fileName);

    // check if within canvas
    bool WithinCanvas(int x, int y);

    // only draw if in canvas
    void DrawPointOnTexture(int mouse_x, int mouse_y, short colour = FG_WHITE, short glyph = 0x2588);

    bool CheckFolderExist(const std::wstring& folderPath);

    bool CreateFolder(const std::wstring& folderPath);


}; // end of TexturePainter class definition

