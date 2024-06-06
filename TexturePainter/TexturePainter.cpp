#include <iostream>

#include "Utility.h"
#include "TexturePainter.h"

// constructor stuff...
TexturePainter::TexturePainter(std::wstring newTitle)
{
    sConsoleTitle = newTitle;
    eventLog.push_back(GetFormattedDateTime() + L" - Output of Error Log of last " + sConsoleTitle + L" session" + L":\n");
    
    // load the icon textures into the static member variables
    Canvas::InitialiseTextures();
}

TexturePainter::~TexturePainter()
{
    for (Canvas* canvas : canvases)
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

    nCurrentCanvas = 0;
    currentCanvas = canvases.at(nCurrentCanvas);

    SetResizeWindowLock(true);
    SetConsoleFocusPause(true);
    SetWindowPosition(0, 0);

    return true;
}

bool TexturePainter::OnGameUpdate(float fElapsedTime) {
    FillScreenBuffer(); // clear screen before next frame
    HandleKeyPress();

    currentCanvas->UpdateButtons(); // updates buttons checking for changes in any connected bools/states
    currentCanvas->DrawCanvas(); // handles whole canvas (including buttons)

    DrawHeadingInfo(1, 1);
    DrawToolInfo(1, 11);

    return true;
}

bool TexturePainter::GetUserStartInput()
{
    // check if folder exists; create a new one if not. Return false if unable
    if (CheckIfSaveFolderExists() == false)
        return false;

    availableFileList = GetFileList(SAVE_FOLDER, TEXTURE_EXTENSION);
    PrintFiles(); // clear screen and print file lists

    while (true)
    {
        // Prompt the user for a file
        int numberOfFiles{ static_cast<int>(availableFileList.size()) };
        std::wstring prompt{};
        std::wstring letter{ L"T" };
        if (numberOfFiles > 0)
        {
            prompt = L"\nPlease select an existing File Name(1-" + std::to_wstring(numberOfFiles) + L") or\n";
            letter = L"t";
        }
        prompt += letter + L"ype a new name to start new texture project ('q' to exit): ";

        std::wstring userFileName;
        if (GetValidFileName(prompt, userFileName, ILLEGAL_CHARS, MAX_FILE_NAME_LENGTH) == false)
        {
            system("pause");
            PrintFiles();
            continue;
        }

        if (LetterListener(userFileName, L'q')) // quit
            return false;

        int userSelection = WStringToInteger(userFileName);
        bool validSelection = HandleFileSelection(userSelection, userFileName, availableFileList);

        if (validSelection == false) {
            system("pause");
            PrintFiles();
            continue;
        }

        PrintFiles();

        if (GetYesNoInput(L"\nDo you want to add more Textures to edit? (y/n): ") == false)
            break;

    }
    // display choices
    PrintEnteredTextures();
    system("pause"); // wait for key press
    return true;
}

void TexturePainter::CreateCanvasWithNewTexture(const std::wstring& fileName)
{
    int width{ 0 }, height{ 0 }, illumination{ 0 };
    // get new dimentions
    GetDimensionInput(L"\nPlease enter an integer value for the new texture width: ", width, MIN_TEXTURE_WIDTH, MAX_TEXTURE_WIDTH);
    GetDimensionInput(L"\nPlease enter an integer value for the new texture height: ", height, MIN_TEXTURE_HEIGHT, MAX_TEXTURE_HEIGHT);
    GetDimensionInput(L"\nPlease enter illumination value 0 to 255 for the new texture: ", illumination, 0, MAX_ILLUMINATION_VALUE);
    canvases.push_back(new Canvas(*this, width, height, illumination, SAVE_FOLDER, fileName, CANVAS_XPOS, CANVAS_YPOS));
    selectedList.push_back(fileName);
}

void TexturePainter::CreateCanvasWithExistingTexture(const std::wstring& fileName)
{
    canvases.push_back(new Canvas(*this, SAVE_FOLDER, fileName, CANVAS_XPOS, CANVAS_YPOS));
    selectedList.push_back(fileName);
}

bool TexturePainter::HandleFileSelection(int selection, const std::wstring& fileName, const std::vector<std::wstring>& fileList)
{
    size_t size{ fileList.size() };

    // User entered a number
    if (selection != -1) 
    { 
        if (size == 0)
        {
            std::wcout << L"\nNo files to select. Please type a valid file name.\n\n";
            return false;
        }
        if (selection > size)
        {
            std::wcout << L"\nInvalid selection. Please enter a number from 1 - " << size << ".\n\n";
            return false;
        }

        std::wstring selectedFile = fileList.at(selection - 1);

        if (IsFileAlreadySelected(selectedFile) == true) 
            return false;

        CreateCanvasWithExistingTexture(selectedFile);
    }
    // User entered a file name
    else { 
        std::wstring fullFileName = fileName + TEXTURE_EXTENSION;
        if (IsFileAlreadySelected(fullFileName) == true)
            return false;

        if (FileExistInDir(fileList, fullFileName) == true) 
        {
            CreateCanvasWithExistingTexture(fullFileName);
        }
        else
        {
            CreateCanvasWithNewTexture(fullFileName);
        }
    }
    return true;
}

void TexturePainter::PrintFiles()
{
    system("cls");   // clear the screen
    PrintFileList(L"--- " + TEXTURE_EXTENSION + L" files in " + SAVE_FOLDER + L" ---", availableFileList);
    PrintFileList(L"--- Selected File List ---", selectedList);
}

void TexturePainter::PrintEnteredTextures()
{
    system("cls");
    for (Canvas* canvas : canvases)
    {
        std::wcout << L"\nFile Name: " << canvas->GetFileName()
                   << L"\nSize (w * h): " << canvas->GetTextureWidth() << L" x " << canvas->GetTextureHeight()
                   << L"\nIllumination: " << canvas->GetIllumination() << std::endl << std::endl;
    }
}

bool TexturePainter::IsFileAlreadySelected(const std::wstring& fileName)
{
    // Iterate over the vector of canvases
    for (const auto& canvas : canvases)
    {
        if (canvas->GetFileName() == fileName)
        {
            std::wcout << L"\nThe file '" << fileName << L"' is already selected. Please choose a different file.\n\n";
            return true;
        }
    }
    return false; // Return false if no matching file name is found
}

bool TexturePainter::ChangeCanvas(size_t index)
{
    if (index < canvases.size())
    {
        // swap to new canvas
        currentCanvas = canvases.at(index);
        return true;
    }
    else
    {
        AddToLog(L"Attempted to access non existent canvas.");
        return false;
    }
}

void TexturePainter::DrawHeadingInfo(int x, int y)
{
    // positions of screen info rows
    int row1{ x };
    int row2{ x + 74 };
    int row3{ x + 116 };

    // texture info
    WriteStringToBuffer(row1, y,     L"Current File Name:  " + currentCanvas->GetFileName(), FG_CYAN);
    WriteStringToBuffer(row1, y + 1, L"File Path:          " + currentCanvas->GetFilePath(), FG_CYAN);
    WriteStringToBuffer(row1, y + 3, L"Dimentions:         " + std::to_wstring(currentCanvas->GetTextureWidth()) + L" x " + std::to_wstring(currentCanvas->GetTextureHeight()));
    WriteStringToBuffer(row1, y + 4, L"Illuminatation:     " + std::to_wstring(currentCanvas->GetIllumination()));


    // current textures
    WriteStringToBuffer(row2, y,     L"Texture List (number key to switch):", FG_BLUE);
    int count{ 1 };
    for (const auto& canvas : canvases)
    {
        std::wstring fileName{ canvas->GetFileName() };
        std::wstring savedState = canvas->GetSavedState() ? L"  " : L" *"; // add '*' after name if not saved
        WriteStringToBuffer(row2, y + count + 1, std::to_wstring(count) + L". " + fileName + savedState, FG_BLUE);
        count++;
    }

    // Keys
    WriteStringToBuffer(row3, y,     L"Save Current Canvas    F5", FG_GREEN);
    WriteStringToBuffer(row3, y + 1, L"Load Current Canvas    F9", FG_GREEN);
    WriteStringToBuffer(row3, y + 2, L"Select Canvas          1-9", FG_GREEN);
    WriteStringToBuffer(row3, y + 3, L"Untitled Canvas        0", FG_GREEN);
    WriteStringToBuffer(row3, y + 4, L"Change Zoom            +/-  ", FG_GREEN);
    WriteStringToBuffer(row3, y + 5, L"Exit                   ESC", FG_GREEN);

    WriteStringToBuffer(row3, y + 7, L"UNDO action            Ctrl Z", FG_GREEN);
    WriteStringToBuffer(row3, y + 8, L"REDO action            Ctrl Y", FG_GREEN);
    WriteStringToBuffer(row3, y + 7, L"(" + std::to_wstring(currentCanvas->GetSizeUndoStack()) + L")", FG_GREEN);
    WriteStringToBuffer(row3, y + 8, L"(" + std::to_wstring(currentCanvas->GetSizeRedoStack()) + L")", FG_GREEN);
}

void TexturePainter::DrawToolInfo(int x, int y)
{
    WriteStringToBuffer(x + 1, y, L"Current Brush");
    DrawBlock(x + 4, y + 2, currentCanvas->GetBrushSize(), currentCanvas->GetBrushColour() | BG_DARK_GREY, PIXEL_THREEQUARTERS);
}

bool TexturePainter::HandleKeyPress()
{
    //controls

    // when left mouse button pressed
    if (keyArray[VK_LBUTTON].bPressed)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
            currentCanvas->HandleLeftMouseClick(mouseCoords);
        else
            currentCanvas->HandleAnyButtonsClicked(mouseCoords);

    } // if continues to be held
    else if (keyArray[VK_LBUTTON].bHeld)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
            currentCanvas->HandleLeftMouseClick(mouseCoords);
    }

    // when left mouse lifted
    if (keyArray[VK_LBUTTON].bReleased)
    {
        // only mouse release if not over buttons
        if (!currentCanvas->AreCoordsWithinButtons(mouseCoords))
            currentCanvas->HandleLeftMouseRelease(mouseCoords);
    }

    // makes right click a delete action - will not activate buttons
    if (keyArray[VK_RBUTTON].bHeld)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
        {
            currentCanvas->SetBrushToDelete();
            currentCanvas->HandleLeftMouseClick(mouseCoords);
        }
    }

    // when right mouse lifted
    if (keyArray[VK_RBUTTON].bReleased)
    {
        currentCanvas->SetBrushToCurrentPixel(); // back to the current pixel
        currentCanvas->HandleLeftMouseRelease(mouseCoords);
    }

    if (keyArray[VK_OEM_PLUS].bPressed)
    {
        currentCanvas->IncreaseZoomLevel();
    }

    if (keyArray[VK_OEM_MINUS].bPressed)
    {
        currentCanvas->DecreaseZoomLevel();
    }

    // loop through keys 1-9 to swap between preloaded textures
    for (size_t i{ 0 }; i < 9; i++)
    {
        wchar_t numKey = L'1' + i; // Correctly convert the digit to its corresponding wchar_t character for the keyArray

        if (keyArray[numKey].bPressed)
        {
            if (i < canvases.size())
            {
                // i is position in the canvas vector
                std::wstring fileName = canvases.at(i)->GetFileName();
                std::wstring message = L"Loading Canvas " + std::to_wstring(i + 1) + L". " + fileName;
                DisplayAlertMessage(message);
                ChangeCanvas(i);
            }
            else
            {
                DisplayAlertMessage(L"No canvas loaded at position " + std::to_wstring(i + 1));
            }
        }
    }

    if (keyArray[L'0'].bPressed) {
        if (canvases.size() >= 9) {
            DisplayAlertMessage(L"Maximum number of textures loaded, new texture creation aborted.");
        }
        else {
            std::wstring textureName;
            int textureWidth{ 0 }, textureHeight{ 0 }, illumination{ 0 };

            // populate the new values
            if (GatherNewTextureValues(textureName, textureWidth, textureHeight, illumination))
            {
                // Create the texture
                canvases.push_back(new Canvas(*this, textureWidth, textureHeight, illumination, SAVE_FOLDER, textureName, CANVAS_XPOS, CANVAS_YPOS));
                selectedList.push_back(textureName);
                ChangeCanvas(canvases.size() - 1);
                DisplayAlertMessage(L"Texture created successfully.");
            }
            else
            {
                DisplayAlertMessage(L"Texture creation unsuccessful.");
            }
        }
    }


    if (keyArray[VK_F5].bPressed)
    {
        std::wstring filePath{ currentCanvas->GetFilePath() };
        if (currentCanvas->SaveTexture(filePath))
            DisplayAlertMessage(filePath + L" saved");
        else
            DisplayAlertMessage(L"Unable to save " + filePath);
    }

    if (keyArray[VK_F9].bPressed)
    {
        std::wstring filePath{ currentCanvas->GetFilePath() };
        if (currentCanvas->LoadTexture(filePath))
            DisplayAlertMessage(filePath + L" re-loaded");
        else
            DisplayAlertMessage(L"Unable to load " + filePath);
    }

    // arrow keys to scroll canvas view
    if (keyArray[VK_RIGHT].bHeld)
    {
        currentCanvas->ChangeCanvasOffset({1, 0});
        Sleep(20);
    }
    if (keyArray[VK_LEFT].bHeld)
    {
        currentCanvas->ChangeCanvasOffset({ -1, 0 });
        Sleep(20);

    }
    if (keyArray[VK_UP].bHeld)
    {
        currentCanvas->ChangeCanvasOffset({ 0, -1 });
        Sleep(20);

    }
    if (keyArray[VK_DOWN].bHeld)
    {
        currentCanvas->ChangeCanvasOffset({ 0, 1 });
        Sleep(20);

    }

    // UNDO functionality
    if (keyArray[VK_CONTROL].bHeld && keyArray[L'Z'].bPressed)
    {
        currentCanvas->UndoLastCommand();
    }
    // REDO functionality
    if (keyArray[VK_CONTROL].bHeld && keyArray[L'Y'].bPressed)
    {
        currentCanvas->RedoLastCommand();
    }

    return true;
}

bool TexturePainter::GetLeftMouseHeld()
{
    return keyArray[VK_LBUTTON].bHeld;
}

bool TexturePainter::GetCurrentSharedClipboardTextureState()
{
    return currentCanvas->GetSharedClipboardTextureState();
}

bool TexturePainter::GatherNewTextureValues(std::wstring& textureName, int& textureWidth, int& textureHeight, int& illumination)
{
    while (true) // keep asking if input incorrect
    {
        DisplayAlertMessageWithInput(L"Please enter a new texture name (enter 'Q' to quit)...", textureName);
        if (textureName == L"Q" || textureName == L"q") {
            DisplayAlertMessage(L"Creating new texture aborted.");
            return false;  // Exit if user chooses to quit
        }

        textureName += TEXTURE_EXTENSION;
        if (FileExistInDir(availableFileList, textureName)) {
            DisplayAlertMessage(L"File name already exists. Please enter a new name.");
            continue;
        }

        // Get texture width
        if (!GetInputWithValidation(
            L"Please enter texture width (" + std::to_wstring(MIN_TEXTURE_WIDTH) + L" - " + std::to_wstring(MAX_TEXTURE_WIDTH) + L")",
            textureWidth,
            [](int w) { return w >= MIN_TEXTURE_WIDTH && w <= MAX_TEXTURE_WIDTH; },
            L"Width must be between " + std::to_wstring(MIN_TEXTURE_WIDTH) + L" and " + std::to_wstring(MAX_TEXTURE_WIDTH) + L"."
        )) continue;

        // Get texture height
        if (!GetInputWithValidation(
            L"Please enter texture height (" + std::to_wstring(MIN_TEXTURE_HEIGHT) + L" - " + std::to_wstring(MAX_TEXTURE_HEIGHT) + L")", textureHeight,
            [](int h) { return h >= MIN_TEXTURE_HEIGHT && h <= MAX_TEXTURE_HEIGHT; },
            L"Height must be between " + std::to_wstring(MIN_TEXTURE_HEIGHT) + L" and " + std::to_wstring(MAX_TEXTURE_HEIGHT) + L"."
        )) continue;

        // Get illumination value
        if (!GetInputWithValidation(
            L"Please enter illumination value (0 - " + std::to_wstring(MAX_ILLUMINATION_VALUE) + L"): ",
            illumination,
            [](int i) { return i >= 0 && i <= MAX_ILLUMINATION_VALUE; },
            L"Illumination value must be between 0 and " + std::to_wstring(MAX_ILLUMINATION_VALUE) + L"."
        )) continue;

        return true; // all inputs correct
    }
}

// Function to handle input with custom validation
bool TexturePainter::GetInputWithValidation(const std::wstring& prompt, int& value, std::function<bool(int)> validator, const std::wstring& errorMessage) {
    std::wstring userInput;
    bool validInput = false;
    while (!validInput) {
        DisplayAlertMessageWithInput(prompt, userInput);
        try {
            value = std::stoi(userInput);
            if (validator(value)) {
                validInput = true;
            }
            else {
                DisplayAlertMessage(errorMessage);
            }
        }
        catch (const std::exception&) {
            DisplayAlertMessage(L"Invalid input. Please enter a valid number.");
        }
    }
    return validInput;
}

bool TexturePainter::CheckIfSaveFolderExists()
{    
    // check if folder exists; create a new one if not.
    if (!CheckFolderExists(SAVE_FOLDER))
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
    return true;
}
