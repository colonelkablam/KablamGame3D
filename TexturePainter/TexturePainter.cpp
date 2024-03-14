#include <iostream>
#include "TexturePainter.h"

// constructor stuff...
TexturePainter::TexturePainter(std::wstring title)
    : currentPixel{ PIXEL_SOLID, FG_BLUE }, deletePixel{ L' ', 0 }, currentBrushType{ BrushType::BRUSH_POINT }, brushSize{ 1 }
{
    sConsoleTitle = title;
    eventLog.push_back(GetFormattedDateTime() + L" - Output of Error Log of last " + sConsoleTitle + L" session" + L":\n");
}

TexturePainter::~TexturePainter()
{
    for (Canvas* canvas: canvases)
    {
        delete canvas->texture;
        canvas->texture = nullptr;
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

    return true;
}


bool TexturePainter::OnGameUpdate(float fElapsedTime) {

    HandleKeyPress();
    FillScreenBuffer(); // clear screen before next frame

    // square around canvas position
    WriteStringToBuffer(currentCanvas->xPos - 1, currentCanvas->yPos - 4, L"ZOOM LEVEL: " + std::to_wstring(currentCanvas->zoomLevel));
    WriteStringToBuffer(currentCanvas->xPos - 1, currentCanvas->yPos - 2, L"COORDS X: -");
    WriteStringToBuffer(currentCanvas->xPos + 12, currentCanvas->yPos - 2, L"Y: -");



    // if coords withing canvas diplay (index starting from 1 for user)
    if (IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        WriteStringToBuffer(currentCanvas->xPos + 9, currentCanvas->yPos - 2, std::to_wstring(ConvertMouseCoordsToTextureCoords().X + 1));
        WriteStringToBuffer(currentCanvas->xPos + 15, currentCanvas->yPos - 2, std::to_wstring(ConvertMouseCoordsToTextureCoords().Y + 1));
    }

    DrawRectangleEdgeLength(currentCanvas->xPos - 1, currentCanvas->yPos - 1, (currentCanvas->width * currentCanvas->zoomLevel) + 2, (currentCanvas->height * currentCanvas->zoomLevel) + 2, FG_RED);

    // add texture to screen buffer
    DrawTextureToScreen(currentCanvas->texture, currentCanvas->xPos, currentCanvas->yPos, currentCanvas->zoomLevel);


    // update info display
    // texture info
    WriteStringToBuffer(1, 1, L"Current File Name:  " + currentCanvas->fileName, FG_CYAN);
    WriteStringToBuffer(1, 2, L"File Path:          " + currentCanvas->filePath, FG_CYAN);
    WriteStringToBuffer(1, 4, L"Dimentions:         " + std::to_wstring(currentCanvas->width) + L" x " + std::to_wstring(currentCanvas->height));
    WriteStringToBuffer(1, 5, L"Illuminatation:     " + std::to_wstring(currentCanvas-> illumination));

    // instructions
    WriteStringToBuffer(60, 1, L"Save Current Canvas    F5", FG_GREEN);
    WriteStringToBuffer(60, 2, L"Load Current Canvas    F9", FG_GREEN);
    WriteStringToBuffer(60, 3, L"Select Canvas          0-9", FG_GREEN);
    WriteStringToBuffer(60, 4, L"Change Zoom            +  ", FG_GREEN);
    WriteStringToBuffer(60, 5, L"Exit                   ESC", FG_GREEN);

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
        std::wcout << L"\nFile Name:      " << canvas->fileName;
        std::wcout << L"\nSize (w * h):   " << canvas->width << L" x " << canvas->height;
        std::wcout << L"\nIllumination:    " << canvas->illumination << std::endl << std::endl;
    }


    system("pause"); // wait for key press

    // free memory from file list ptr
    delete fileList;

    return true;
}

bool TexturePainter::InitCanvasNewTexture(int width, int height, int illumination, const std::wstring& fileName)
{
    // create new canvas
    Canvas* canvas = new Canvas;
    canvas->zoomLevel = 1;
    canvas->fileName = fileName;
    canvas->filePath = SAVE_FOLDER + fileName;
    canvas->xPos = CANVAS_XPOS;
    canvas->yPos = CANVAS_YPOS;
    canvas->width = width;
    canvas->height = height;
    canvas->illumination = illumination;
    canvas->texture = new Texture(width, height, illumination);

    // save the new texture to save folder (create an empty file)
    canvas->texture->SaveAs(canvas->filePath);
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
}

void TexturePainter::ChangeCanvas(size_t index)
{
    currentCanvas = canvases.at(index);
}


bool TexturePainter::InitCanvasExistingTexture(const std::wstring& fileName)
{
    // create new canvas
    Canvas* canvas = new Canvas;
    canvas->zoomLevel = 1;
    // load up existing texture
    canvas->texture = new Texture(SAVE_FOLDER + fileName);
    canvas->fileName = fileName;
    canvas->filePath = SAVE_FOLDER + fileName;
    canvas->xPos = CANVAS_XPOS;
    canvas->yPos = CANVAS_YPOS;
    canvas->width = canvas->texture->GetWidth();
    canvas->height = canvas->texture->GetWidth();
    canvas->illumination = canvas->texture->GetIllumination();

    // no need to save a file as texture already exists
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
}


// check if within canvas
bool TexturePainter::IsMouseWithinCanvas(int x, int y)
{
    int zoom = currentCanvas->zoomLevel;
    int xPos = currentCanvas->xPos;
    int yPos = currentCanvas->yPos;
    int width = currentCanvas->width;
    int height = currentCanvas->height;

    // Check if the mouse coordinates are within the zoom-adjusted canvas boundaries
    if (x >= xPos && x < xPos + width*zoom && y >= yPos && y < yPos + height*zoom)
        return true;
    else
        return false;
}

COORD TexturePainter::ConvertMouseCoordsToTextureCoords()
{
    COORD coords;
    int zoom = currentCanvas->zoomLevel;
    int xPos = currentCanvas->xPos;
    int yPos = currentCanvas->yPos;

    if (IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
    {
        // Correct calculation for both X and Y coordinates
        coords.X = (mouseCoords.X - xPos) / zoom;
        coords.Y = (mouseCoords.Y - yPos) / zoom;
    }
    else
    {
        coords.X = -1; // Indicate invalid coordinates
        coords.Y = -1;
    }
    return coords;
}

void TexturePainter::ApplyBrush(int x, int y)
{
    switch (currentBrushType) {
    case BrushType::BRUSH_POINT:
        currentCanvas->texture->SetPixel(x, y, currentPixel); // Draws a single pixel
        break;
    case BrushType::BRUSH_SQUARE:
        DrawSquare(x, y, brushSize, currentPixel.Attributes, currentPixel.Char.UnicodeChar, true); // Draws a square
        break;
    case BrushType::BRUSH_LINE:
         //For a line, you'll likely need to store the initial click position
         //and then draw the line to the current mouse position
        //if (initialClick) {
        //    initialX = x;
        //    initialY = y;
        //    initialClick = false; // Reset after the initial click is recorded
        //}
        //else {
        //    DrawLine(initialX, initialY, x, y, currentPixel); // Draws a line
        //}
        break;
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
        COORD textureCoord = ConvertMouseCoordsToTextureCoords();
        ApplyBrush(textureCoord.X, textureCoord.Y);
    }

    if (keyArray[VK_RBUTTON].bHeld)
    {
        COORD textureCoord = ConvertMouseCoordsToTextureCoords();
        currentCanvas->texture->SetPixel(textureCoord.X, textureCoord.Y, deletePixel);
    }

    if (keyArray[VK_OEM_PLUS].bPressed)
    {
        currentCanvas->IncreaseZoomLevel();
    }

    for (size_t i{ 0 }; i < 10; i++)
    {
        wchar_t num = L'0' + i; // Convert the digit to its corresponding wchar_t character

        if (keyArray[num].bPressed)
        {
            if (i < canvases.size())
            {
                std::wstring message = L"Loading Canvas " + std::to_wstring(i) + L". " + canvases.at(i)->fileName;
                DisplayAlertMessage(message);
                nCurrentCanvas = i;
            }
            else
                DisplayAlertMessage(L"No canvas loaded at position " + std::to_wstring(i));
        }
    }

    if (keyArray[VK_F5].bPressed)
    {
        currentCanvas->texture->SaveAs(currentCanvas->filePath);
    }

    if (keyArray[VK_F9].bPressed)
    {
        currentCanvas->texture->LoadFrom(currentCanvas->filePath);
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
