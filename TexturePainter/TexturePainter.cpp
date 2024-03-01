#include <iostream>
#include "TexturePainter.h"

// constructor stuff...
TexturePainter::TexturePainter(std::wstring title)
{
    sConsoleTitle = title;
    eventLog.push_back(GetFormattedDateTime() + L" - Output of Error Log of last " + sConsoleTitle + L" session" + L":\n");

}

TexturePainter::~TexturePainter()
{
    for (Canvas canvas: canvases)
    {
        delete[] canvas.texture;
        canvas.texture = nullptr;
    }
}

bool TexturePainter::OnGameCreate() 
{

    if (canvases.empty())
    {
        return Error(L"No textures loaded; unable to continue.");
    }

    nCurrentCanvas = 0;
    return true;
}


bool TexturePainter::OnGameUpdate(float fElapsedTime) {

    
    while (bGameThreadRunning) {

        int canvasXPos = canvases.at(nCurrentCanvas).xPos;
        int canvasYPos = canvases.at(nCurrentCanvas).yPos;
        int canvasWidth = canvases.at(nCurrentCanvas).width;
        int canvasHeight = canvases.at(nCurrentCanvas).height;
        Texture* canvasTexture = canvases.at(nCurrentCanvas).texture;


        // square around canvas position
        DrawRectangleEdgeLength(canvasXPos - 1, canvasYPos - 1, canvasWidth, canvasHeight, FG_RED);

        // add texture to screen buffer
        DrawTextureToScreen(canvasTexture, canvasXPos, canvasYPos, 1);


        // update info display
        WriteStringToBuffer(1, 1, L"Current File Name: " + sCurrentFileName);
        WriteStringToBuffer(1, 3, L"Dimentions: " + std::to_wstring(canvasWidth) + L" x " + std::to_wstring(canvasHeight));
        WriteStringToBuffer(1, 5, L"                                                    "); // hack to clear mouse position info
        WriteStringToBuffer(1, 5, L"Canvas COORDS X: " + std::to_wstring(mouseCoords.X - canvasXPos) + L", Y: " + std::to_wstring(mouseCoords.Y - canvasYPos));

    }

    return true;
}

bool TexturePainter::GetUserStartInput()
{
    bool finishedInput{ false };
    int inputWidth{ 0 };
    int inputHeight{ 0 };
    int inputIllumination{ 0 };
    std::wstring userFileName;
    std::vector <std::wstring>* fileList{ nullptr };

    // check if folder exists; create a new one if not.
    if (!CheckFolderExist(sSaveFolderName))
    {
        AddToLog(L"'" + sSaveFolderName + L"' does not exist.");
        if (!CreateFolder(sSaveFolderName))
        {
            AddToLog(L"'" + sSaveFolderName + L"Unable to create save folder. Exiting program.");
            return false;
        }
        else
            AddToLog(L"'" + sSaveFolderName + L"Created '" + sSaveFolderName + L"' save folder.");
    }
    else
        AddToLog(L"'" + sSaveFolderName + L"' exists. No need to create save folder.");


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
            InitCanvasExistingTexture(fileList->at(userSelection - 1));
            break;
        }
        else // else test to see if file exists
        {
            if (FileExistInDir(fileList, userFileName + sExtensionName)) // if exists
            {
                // load texture from existing file
                InitCanvasExistingTexture(userFileName + sExtensionName);
            }
            else
            {
                // get new dimentions
                GetDimensionInput(L"\nPlease enter an integer value for the new texture width: ", inputWidth, MIN_TEXTURE_SIZE, MAX_TEXTURE_SIZE);
                GetDimensionInput(L"\nPlease enter an integer value for the new texture height: ", inputHeight, MIN_TEXTURE_SIZE, MAX_TEXTURE_SIZE);
                GetDimensionInput(L"\nPlease enter illumination value 0 or 1 (off/on) for the new texture: ", inputIllumination, 0, 1);


                // create and save new .txr file in saves path
                InitCanvasNewTexture(inputWidth, inputHeight, inputIllumination, userFileName + sExtensionName);
            }
        }
        if (!GetYesNoInput(L"\nDo you want to add more Textures to edit? (y/n): "))
            finishedInput = true;

    } while (finishedInput == false);


    // Output the entered textures (optional, for verification)
    for (Canvas canvas : canvases)
    {
        std::wcout << L"\nFile Name:      " << canvas.fileName;
        std::wcout << L"\nSize (w * h):   " << canvas.width << L" x " << canvas.height;
        std::wcout << L"\nIlluminated:    " << canvas.illumination << std::endl << std::endl;
    }


    system("pause"); // wait for key press

    // free memory from file list ptr
    delete fileList;

    return true;
}

bool TexturePainter::InitCanvasNewTexture(int width, int height, bool illuminated, const std::wstring& fileName)
{
    // create new canvas
    Canvas canvas;
    canvas.fileName = fileName;
    canvas.filePath = sSaveFolderName + fileName;
    canvas.width = width;
    canvas.height = height;
    canvas.illumination = illuminated;
    canvas.texture = new Texture(width, height, false);

    // save the new texture to save folder (create an empty file)
    canvas.texture->SaveAs(sCurrentFilePath);

    canvases.push_back(canvas);

    return true;
}

bool TexturePainter::InitCanvasExistingTexture(const std::wstring& fileName)
{
    // create new canvas
    Canvas canvas;
    canvas.texture = new Texture(sSaveFolderName + fileName);
    canvas.fileName = fileName;
    canvas.filePath = sSaveFolderName + fileName;
    canvas.width = canvas.texture->GetWidth();
    canvas.height = canvas.texture->GetWidth();
    canvas.illumination = canvas.texture->IsIlluminated();


    // no need to save a file as texture already exists

    canvases.push_back(canvas);

    return true;
}


// check if within canvas
bool TexturePainter::WithinCanvas(int x, int y)
{
    if (x >= canvases.at(nCurrentCanvas).xPos && x <= canvases.at(nCurrentCanvas).xPos + canvases.at(nCurrentCanvas).width
     && y >= canvases.at(nCurrentCanvas).yPos && y <= canvases.at(nCurrentCanvas).yPos + canvases.at(nCurrentCanvas).height )
        return true;
    else
        return false;
}

void TexturePainter::DrawPointOnTexture(int mouse_x, int mouse_y, short colour, short glyph)
{
}

bool TexturePainter::CheckFolderExist(const std::wstring& folderPath) {
    DWORD fileAttributes = GetFileAttributes(folderPath.c_str());

    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        // The path does not exist or there is an error
        return false;
    }

    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        // The path exists and is a directory
        return true;
    }

    // The path exists but is not a directory (it's a file)
    return false;
}

bool TexturePainter::CreateFolder(const std::wstring& folderPath) {
    if (!CreateDirectory(folderPath.c_str(), NULL)) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            return true; // The folder already exists
        }
        std::wcerr << L"Failed to create directory: " << folderPath << std::endl;
        return false; // Failed to create the directory
    }

    return true; // Successfully created the directory
}
