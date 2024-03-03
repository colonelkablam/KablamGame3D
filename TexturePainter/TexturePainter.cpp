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
        delete canvas.texture;
        canvas.texture = nullptr;
    }

    delete currentCanvas;
    currentCanvas = nullptr;
}

bool TexturePainter::OnGameCreate() 
{

    if (canvases.empty())
    {
        return Error(L"No textures loaded; unable to continue.");
    }


    // start at 1st canvas
    nCurrentCanvas = 0;

    // set current canvas pointer 
    currentCanvas = &canvases.at(nCurrentCanvas);

    return true;
}


bool TexturePainter::OnGameUpdate(float fElapsedTime) {

    HandleKeyPress(fElapsedTime);

    currentCanvas = &canvases.at(nCurrentCanvas);

    // square around canvas position
    DrawRectangleEdgeLength(currentCanvas->xPos - 1, currentCanvas->xPos - 1, currentCanvas->width + 2, currentCanvas->height + 2, FG_RED);

    // add texture to screen buffer
    DrawTextureToScreen(currentCanvas->texture, currentCanvas->xPos, currentCanvas->yPos, 1);


    // update info display
    WriteStringToBuffer(1, 1, L"Current File Name: " + currentCanvas->fileName, FG_WHITE);
    WriteStringToBuffer(1, 3, L"Dimentions: " + std::to_wstring(currentCanvas->width) + L" x " + std::to_wstring(currentCanvas->height));
    WriteStringToBuffer(1, 5, L"                                                    "); // hack to clear mouse position info
    WriteStringToBuffer(1, 5, L"Canvas COORDS X: " + std::to_wstring(mouseCoords.X - currentCanvas->xPos) +
        L", Y: " + std::to_wstring(mouseCoords.Y - currentCanvas->yPos));


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
    if (!CheckFolderExist(SAVE_FOLDER))
    {
        AddToLog(L"'" + SAVE_FOLDER + L"' does not exist.");
        if (!CreateFolder(SAVE_FOLDER))
        {
            AddToLog(L"'" + SAVE_FOLDER + L"Unable to create save folder. Exiting program.");
            return false;
        }
        else
            AddToLog(L"'" + SAVE_FOLDER + L"Created '" + SAVE_FOLDER + L"' save folder.");
    }
    else
        AddToLog(L"'" + SAVE_FOLDER + L"' exists. No need to create save folder.");


    // ask until existing or correct format file name given
    do {
        // attempt to get list of saved files
        fileList = GetFileList(SAVE_FOLDER, TEXTURE_EXTENSION);
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
            if (FileExistInDir(fileList, userFileName + TEXTURE_EXTENSION)) // if exists
            {
                // load texture from existing file
                InitCanvasExistingTexture(userFileName + TEXTURE_EXTENSION);
            }
            else
            {
                // get new dimentions
                GetDimensionInput(L"\nPlease enter an integer value for the new texture width: ", inputWidth, MIN_TEXTURE_SIZE, MAX_TEXTURE_SIZE);
                GetDimensionInput(L"\nPlease enter an integer value for the new texture height: ", inputHeight, MIN_TEXTURE_SIZE, MAX_TEXTURE_SIZE);
                GetDimensionInput(L"\nPlease enter illumination value 0 or 1 (off/on) for the new texture: ", inputIllumination, 0, 1);


                // create and save new .txr file in saves path
                InitCanvasNewTexture(inputWidth, inputHeight, inputIllumination, userFileName + TEXTURE_EXTENSION);
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
    canvas.filePath = SAVE_FOLDER + fileName;
    canvas.xPos = CANVAS_XPOS;
    canvas.yPos = CANVAS_YPOS;
    canvas.width = width;
    canvas.height = height;
    canvas.illumination = illuminated;
    canvas.texture = new Texture(width, height, false);

    // save the new texture to save folder (create an empty file)
    canvas.texture->SaveAs(canvas.filePath);

    canvases.push_back(canvas);

    return true;
}

void TexturePainter::ChangeCanvas(size_t index)
{
    currentCanvas = &canvases.at(index);
}

bool TexturePainter::InitCanvasExistingTexture(const std::wstring& fileName)
{
    // create new canvas
    Canvas canvas;
    canvas.texture = new Texture(SAVE_FOLDER + fileName);
    canvas.fileName = fileName;
    canvas.filePath = SAVE_FOLDER + fileName;
    canvas.xPos = CANVAS_XPOS;
    canvas.yPos = CANVAS_YPOS;
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


bool TexturePainter::HandleKeyPress(float fElapsedTime)
{
    //controls
    if (keyArray[VK_LBUTTON].bHeld)
    {
        currentCanvas->texture->SetColour(mouseCoords.X - currentCanvas->xPos,
            mouseCoords.Y - currentCanvas->xPos, currentColour);
        currentCanvas->texture->SetGlyph(mouseCoords.X - currentCanvas->xPos,
            mouseCoords.Y - currentCanvas->xPos, currentGlyph);
    }

    if (keyArray[VK_RBUTTON].bHeld)
    {
        currentCanvas->texture->SetColour(mouseCoords.X - currentCanvas->xPos,
            mouseCoords.Y - currentCanvas->xPos, FG_BLACK);
        currentCanvas->texture->SetGlyph(mouseCoords.X - currentCanvas->xPos,
            mouseCoords.Y - currentCanvas->xPos, L' ');

    }

    for (size_t i{ 0 }; i < 10; i++)
    {
        wchar_t num = L'0' + i; // Convert the digit to its corresponding wchar_t character

        if (keyArray[num].bHeld)
        {
            if (i < canvases.size())
                nCurrentCanvas = i;
            else
                DisplayAlertMessage(L"Test Input Error!...");
        }
    }

    return true;
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
