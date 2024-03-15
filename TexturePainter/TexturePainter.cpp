#include <iostream>
#include "TexturePainter.h"

// constructor stuff...
TexturePainter::TexturePainter(std::wstring newTitle)
{
    sConsoleTitle = newTitle;
    eventLog.push_back(GetFormattedDateTime() + L" - Output of Error Log of last " + sConsoleTitle + L" session" + L":\n");
}

TexturePainter::~TexturePainter()
{
    for (Canvas* canvas: canvases)
    {
        delete canvas;
        canvas = nullptr;
    }
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
    currentCanvas = canvases.at(nCurrentCanvas);

    SetResizeWindowLock(true);
    SetConsoleFocusPause(true);
    SetWindowPosition(50, 50);

    buttons.push_back(Button(1, 20, 4, FG_RED, [this]() {currentCanvas->SetBrushColour(FG_RED); }));
    buttons.push_back(Button(6, 20, 4, FG_GREEN, [this]() {currentCanvas->SetBrushColour(FG_GREEN); }));
    buttons.push_back(Button(1, 25, 4, FG_BLUE, [this]() {currentCanvas->SetBrushColour(FG_BLUE); }));
    buttons.push_back(Button(6, 25, 4, FG_WHITE, [this]() {currentCanvas->SetBrushColour(FG_WHITE); }));

    return true;
}


bool TexturePainter::OnGameUpdate(float fElapsedTime) {

    HandleKeyPress();
    FillScreenBuffer(); // clear screen before next frame

    DrawHeadingInfo();
    DrawCanvas();

    DrawToolInfo();
    DrawButtons();

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
                GetDimensionInput(L"\nPlease enter illumination value 0 to 255 for the new texture: ", inputIllumination, 0, 255);


                // create and save new .txr file in saves path
                InitCanvasNewTexture(inputWidth, inputHeight, inputIllumination, userFileName + TEXTURE_EXTENSION);
            }
        }
        if (!GetYesNoInput(L"\nDo you want to add more Textures to edit? (y/n): "))
            finishedInput = true;

    } while (finishedInput == false);


    // Output the entered textures (optional, for verification)
    for (Canvas* canvas : canvases)
    {
        std::wcout << L"\nFile Name:      " << canvas->GetFileName();
        std::wcout << L"\nSize (w * h):   " << canvas->GetTextureWidth() << L" x " << canvas->GetTextureHeight();
        std::wcout << L"\nIllumination:    " << canvas->GetIllumination() << std::endl << std::endl;
    }


    system("pause"); // wait for key press

    // free memory from file list ptr
    delete fileList;

    return true;
}

bool TexturePainter::InitCanvasNewTexture(int width, int height, int illumination, const std::wstring& fileName)
{
    // create new canvas
    Canvas* canvas = new Canvas(new Texture(width, height, illumination), fileName, SAVE_FOLDER + fileName, CANVAS_XPOS, CANVAS_YPOS);

    // save the new texture to save folder (create an empty file)
    canvas->SaveTexture(SAVE_FOLDER + fileName);
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
}


bool TexturePainter::InitCanvasExistingTexture(const std::wstring& fileName)
{
    // load up an existing texture
    Texture* existingTexture = new Texture(SAVE_FOLDER + fileName);
    // create new canvas with existing texture
    Canvas* canvas = new Canvas(existingTexture, fileName, SAVE_FOLDER + fileName, CANVAS_XPOS, CANVAS_YPOS);

    // no need to save a file as texture already exists
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
}

void TexturePainter::ChangeCanvas(size_t index)
{
    currentCanvas = canvases.at(index);
}

void TexturePainter::DrawCanvas()
{
    COORD coords = currentCanvas->GetPositionCoords();
    int zoom = currentCanvas->GetZoomLevel();

    WriteStringToBuffer(coords.X - 1, coords.Y - 4, L"ZOOM LEVEL: " + std::to_wstring(zoom));
    WriteStringToBuffer(coords.X - 1, coords.Y - 2, L"COORDS X: -");
    WriteStringToBuffer(coords.X + 12, coords.Y - 2, L"Y: -");

    // if coords withing canvas diplay (index starting from 1 for user)
    if (currentCanvas->IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        COORD textureCoords = currentCanvas->ConvertMouseCoordsToTextureCoords(mouseCoords.X, mouseCoords.Y);
        WriteStringToBuffer(coords.X + 9, coords.Y - 2, std::to_wstring(textureCoords.X + 1));
        WriteStringToBuffer(coords.X + 15, coords.Y - 2, std::to_wstring(textureCoords.Y + 1));
    }

    DrawRectangleEdgeLength(coords.X - 1, coords.Y - 1, (currentCanvas->GetTextureWidth() * zoom) + 2, (currentCanvas->GetTextureHeight() * zoom) + 2, FG_RED);

    // add texture to screen buffer
    DrawTextureToScreen(currentCanvas->GetTexture(), coords.X, coords.Y, zoom);
}

void TexturePainter::DrawHeadingInfo()
{
    // texture info
    WriteStringToBuffer(1, 1, L"Current File Name:  " + currentCanvas->GetFileName(), FG_CYAN);
    WriteStringToBuffer(1, 2, L"File Path:          " + currentCanvas->GetFilePath(), FG_CYAN);
    WriteStringToBuffer(1, 4, L"Dimentions:         " + std::to_wstring(currentCanvas->GetTextureWidth()) + L" x " + std::to_wstring(currentCanvas->GetTextureHeight()));
    WriteStringToBuffer(1, 5, L"Illuminatation:     " + std::to_wstring(currentCanvas->GetIllumination()));

    // instructions
    WriteStringToBuffer(60, 1, L"Save Current Canvas    F5", FG_GREEN);
    WriteStringToBuffer(60, 2, L"Load Current Canvas    F9", FG_GREEN);
    WriteStringToBuffer(60, 3, L"Select Canvas          1-9", FG_GREEN);
    WriteStringToBuffer(60, 3, L"Untitled Canvas        0", FG_GREEN);
    WriteStringToBuffer(60, 4, L"Change Zoom            +  ", FG_GREEN);
    WriteStringToBuffer(60, 5, L"Exit                   ESC", FG_GREEN);
}

void TexturePainter::DrawToolInfo()
{
    WriteStringToBuffer(1, 8,  L"    Brush Size: " + std::to_wstring(currentCanvas->GetBrushSize()), FG_GREEN);
    WriteStringToBuffer(1, 9,  L" Current Brush: " + std::to_wstring(currentCanvas->GetBrushTypeInt()), FG_GREEN);
    WriteStringToBuffer(1, 10, L"Current Colour: ");
    DrawPoint(17, 10, currentCanvas->GetBrushColour(), PIXEL_SOLID);

}

void TexturePainter::DrawButtons()
{
    for (const Button& button : buttons)
    {
        DrawSquare(button.xPos, button.yPos, button.size, button.colour, PIXEL_SOLID, true);
    }

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


bool TexturePainter::HandleKeyPress()
{
    //controls
    if (keyArray[VK_LBUTTON].bHeld)
    {
        if (currentCanvas->IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
            currentCanvas->ApplyBrush(mouseCoords.X, mouseCoords.Y);

        // check if over any of the buttons
        for (Button& button : buttons)
            if (button.IsMouseClickOnButton(mouseCoords.X, mouseCoords.Y))
                button.Clicked();
    }

    if (keyArray[VK_RBUTTON].bHeld)
    {
        currentCanvas->ApplyBrush(mouseCoords.X, mouseCoords.Y, true);
    }

    if (keyArray[VK_OEM_PLUS].bPressed)
    {
        currentCanvas->IncreaseZoomLevel();
    }

    if (keyArray[L'B'].bPressed)
    {
        currentCanvas->ChangeBrushSize(1);
    }

    if (keyArray[L'P'].bPressed)
    {
        currentCanvas->ChangeBrushType(Canvas::BrushType::BRUSH_SQUARE);
    }


    for (size_t i{ 0 }; i < 10; i++)
    {
        wchar_t num = L'0' + i; // Convert the digit to its corresponding wchar_t character

        if (keyArray[num].bPressed)
        {
            if (i < canvases.size())
            {
                std::wstring message = L"Loading Canvas " + std::to_wstring(i) + L". " + canvases.at(i)->GetFileName();
                DisplayAlertMessage(message);
                nCurrentCanvas = i;
            }
            else
                DisplayAlertMessage(L"No canvas loaded at position " + std::to_wstring(i));
        }
    }

    if (keyArray[VK_F5].bPressed)
    {
        currentCanvas->SaveTexture(currentCanvas->GetFilePath());
    }

    if (keyArray[VK_F9].bPressed)
    {
        currentCanvas->LoadTexture(currentCanvas->GetFilePath());
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
