#include <iostream>
#include "TexturePainter.h"


TexturePainter::TexturePainter(std::wstring title)
{
    sConsoleTitle = title;
}

bool TexturePainter::OnGameCreate()
{

    if (!GetUserStartInput())
    {
        std::wcerr << "Exiting program - Unable to find files/get user input...\n";
        return 1;
    }

    if (currentTexture == nullptr)
    {
        std::wcerr << "Exiting Program - Unable to load a texture...\n";
        return 1;
    }

    return true;
}


bool TexturePainter::OnGameUpdate(float fElapsedTime) {

    
    while (bGameThreadRunning) {

        // square around canvas position
        DrawRectangleEdgeLength(nCanvasXpos - 1, nCanvasYpos - 1, nCanvasSizeX, nCanvasSizeY, FG_RED);

        // add texture to screen buffer
        DrawTextureToScreen(currentTexture, nCanvasYpos, nCanvasXpos, 1);


        // update info display
        WriteStringToBuffer(1, 1, L"Current File Name: " + sCurrentFileName);
        WriteStringToBuffer(1, 3, L"Dimentions: " + std::to_wstring(nCanvasSizeX) + L" x " + std::to_wstring(nCanvasSizeY));
        WriteStringToBuffer(1, 5, L"                                                    "); // hack to clear mouse position info
        WriteStringToBuffer(1, 5, L"Canvas COORDS X: " + std::to_wstring(mouseCoords.X - nCanvasXpos) + L", Y: " + std::to_wstring(mouseCoords.Y - nCanvasYpos));

    }

    return true;
}


bool TexturePainter::GetUserStartInput()
{
    int userInputX{ 0 };
    int userInputY{ 0 };
    int userInputI{ 0 };
    std::wstring userFileName;
    std::vector <std::wstring>* fileList{ nullptr };

    // ask until existing or correct format file name given
    do {
        // attempt to get list of saved files
        fileList = GetFileList(sSaveFolderName, sExtensionName);
        std::wcout << std::endl;

        // Prompt the user for a string value
        if (fileList->size() > 0)
            std::wcout << L"Please select an existing File Name(1-" << fileList->size() << "), or ";

        if (!GetValidFileName(L"Type a new name to start new texture project ('q' to exit): ", userFileName))
            return false;

        // returns -1 if not an integer 
        int userSelection{ WStringToInteger(userFileName) };

        // if a valid number then load existing file and break loop
        if (userSelection > 0 && userSelection <= fileList->size())
        {
            // load texture from existing file
            InitialiseExistingSprite(fileList->at(userSelection - 1));
            break;
        }
        else // else test to see if file exists
        {
            if (FileExistInDir(fileList, userFileName + sExtensionName)) // if exists
            {
                // load texture from existing file
                InitialiseExistingSprite(userFileName + sExtensionName);
                break;
            }
            else
            {
                // get new dimentions
                GetDimensionInput(L"\nPlease enter an integer value for the new texture width: ", userInputX, nMinDim, nMaxDim);
                GetDimensionInput(L"\nPlease enter an integer value for the new texture height: ", userInputY, nMinDim, nMaxDim);
                GetDimensionInput(L"\nPlease enter illumination value 0 or 1 (off/on) for the new texture: ", userInputI, 0, 1);


                // create and save new .txr file in saves path
                InitialiseNewSprite(userInputX, userInputY, userInputI, userFileName + sExtensionName);
                break;
            }
        }
    } while (currentTexture == nullptr);


    // Output the entered values (optional, for verification)
    std::wcout << L"\nFile Name:      " << sCurrentFileName;
    std::wcout << L"\nSize (w * h):   " << nCanvasSizeX;
    std::wcout << L"\nIlluminated:    " << currentTexture->IsIlluminated() << std::endl << std::endl;

    system("pause"); // wait for key press

    // free memory from file list ptr
    delete fileList;

    return true;
}

bool TexturePainter::InitialiseNewSprite(int width, int height, bool illuminated, const std::wstring& fileName)
{
    // update file name / path
    sCurrentFilePath = sSaveFolderName + fileName;
    sCurrentFileName = fileName;

    // create new texture object with arguments
    currentTexture = new Texture(width, height, false);

    // get texture size
    nCanvasSizeX = currentTexture->GetWidth();
    nCanvasSizeY = currentTexture->GetHeight();

    // save new texture to save folder
    currentTexture->SaveAs(sCurrentFilePath);
    return true;
}

bool TexturePainter::InitialiseExistingSprite(const std::wstring& fileName)
{
    // update file name / path
    sCurrentFilePath = sSaveFolderName + fileName;
    sCurrentFileName = fileName;

    // create new texture object with arguments
    currentTexture = new Texture(sCurrentFilePath);

    // get texture size
    nCanvasSizeX = currentTexture->GetWidth();
    nCanvasSizeY = currentTexture->GetHeight();
    return true;
}

// check if within canvas
bool TexturePainter::WithinCanvas(int x, int y)
{
    if (x >= nCanvasXpos && x <= nCanvasXpos + nCanvasSizeX && y >= nCanvasYpos && y <= nCanvasYpos + nCanvasSizeY)
        return true;
    else
        return false;
}

void TexturePainter::DrawPointOnSprite(int mouse_x, int mouse_y, short colour, short glyph)
{
}
