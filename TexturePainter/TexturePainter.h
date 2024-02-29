#pragma once

#include "Utility.h"
#include "Texture.h"
#include "KablamEngine.h"

class TexturePainter : public KablamEngine
{
    // member attributes for KablamEngine
private:
    const int nMinDim = 4;
    const int nMaxDim = 64;

    const std::wstring sSaveFolderName = L"Textures\\"; // relative path to subDir
    const std::wstring sExtensionName = L".txr";
    std::wstring sCurrentFileName = L"untitled";
    std::wstring sCurrentFilePath;
    Texture* currentTexture = nullptr;

    short currentColour = FG_WHITE;
    short currentGlyph = PIXEL_SOLID;

    int nCanvasXpos = 16;  // xy of drawing space
    int nCanvasYpos = 16;
    int nCanvasSizeX = 32; // texture sizes
    int nCanvasSizeY = 32;



public:
    // constructor
    TexturePainter(std::wstring title = L"New Unnamed Project");

    // destructor
    ~TexturePainter();

    // member methods for TexturePainter

protected:
    // virtual methods from KablamEngine to be defined
    virtual bool OnGameCreate();
    virtual bool OnGameUpdate(float fElapsedTime);

    // Optional for clean up 
    //virtual bool OnGameDestroy() { return true; }

private:

    bool GetUserStartInput();

    bool InitialiseExistingSprite(const std::wstring& fileName);

    bool InitialiseNewSprite(int width, int height, bool illuminated, const std::wstring& fileName);

    // check if within canvas
    bool WithinCanvas(int x, int y);

    // only draw if in canvas
    void DrawPointOnSprite(int mouse_x, int mouse_y, short colour = FG_WHITE, short glyph = 0x2588);


}; // end of TexturePainter class definition

