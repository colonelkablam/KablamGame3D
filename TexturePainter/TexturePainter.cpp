#include <iostream>

#include "Utility.h"
#include "TexturePainter.h"

// constructor stuff...
TexturePainter::TexturePainter(std::wstring newTitle)
{
    sConsoleTitle = newTitle;
    eventLog.push_back(GetFormattedDateTime() + L" - Output of Error Log of last " + sConsoleTitle + L" session" + L":\n");

    deleteToolIcon = nullptr;
    blockToolIcon = nullptr;
    increaseToolIcon = nullptr;
    decreaseToolIcon = nullptr;
    rectToolIcon = nullptr;
    rectFillToolIcon = nullptr;
    lineToolIcon = nullptr;
}

TexturePainter::~TexturePainter()
{
    for (Canvas* canvas: canvases)
    {
        delete canvas;
        canvas = nullptr;
    }

     delete deleteToolIcon;
     delete blockToolIcon;
     delete increaseToolIcon;
     delete decreaseToolIcon;
     delete rectToolIcon;
     delete rectFillToolIcon;
     delete lineToolIcon;
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

    // container for colour buttons
    colourButtonsContainer = new ButtonContainer(*this, COLOUR_BUTTON_XPOS, COLOUR_BUTTON_YPOS, 2, 9); 

    // populate it 
    for (short colour = 0; colour < 8; ++colour) // For simplicity directly using the index as the color here and OR with FG_INTENSITY.
    {
        colourButtonsContainer->AddButton(5, 5, colour, [this, colour]() { currentCanvas->SetBrushColour(colour); });
        colourButtonsContainer->AddButton(5, 5, colour | FG_INTENSITY, [this, colour]() { currentCanvas->SetBrushColour(colour | FG_INTENSITY); });
    }
    deleteToolIcon = new Texture(L"./ToolIcons/delete_tool_icon.txr");
    colourButtonsContainer->AddButton(deleteToolIcon, [this]() { currentCanvas->SetBrushToDelete(); });



    // container for tool bottons
    brushButtonsContainer = new ButtonContainer(*this, BRUSH_BUTTON_XPOS, BRUSH_BUTTON_YPOS, 8, 1);

    // load textures
    blockToolIcon = new Texture(L"./ToolIcons/block_tool_icon.txr");
    increaseToolIcon = new Texture(L"./ToolIcons/increase_tool_icon.txr");
    decreaseToolIcon = new Texture(L"./ToolIcons/decrease_tool_icon.txr");
    rectToolIcon = new Texture(L"./ToolIcons/rect_tool_icon.txr");
    rectFillToolIcon = new Texture(L"./ToolIcons/rect_fill_tool_icon.txr");
    lineToolIcon = new Texture(L"./ToolIcons/line_tool_icon.txr");
    
    // populate it
    brushButtonsContainer->AddButton(blockToolIcon, [this]() { currentCanvas->SwitchTool(ToolType::BRUSH_BLOCK); });
    brushButtonsContainer->AddButton(increaseToolIcon, [this]() { currentCanvas->ChangeBrushSize(1); });
    brushButtonsContainer->AddButton(decreaseToolIcon, [this]() { currentCanvas->ChangeBrushSize(-1); });
    brushButtonsContainer->AddButton(lineToolIcon, [this]() { currentCanvas->SwitchTool(ToolType::BRUSH_LINE); });
    brushButtonsContainer->AddButton(rectToolIcon, [this]() { currentCanvas->SwitchTool(ToolType::BRUSH_RECT); });
    brushButtonsContainer->AddButton(rectFillToolIcon, [this]() { currentCanvas->SwitchTool(ToolType::BRUSH_RECT_FILLED); });

    return true;
}


bool TexturePainter::OnGameUpdate(float fElapsedTime) {
    FillScreenBuffer(); // clear screen before next frame
    HandleKeyPress();

    currentCanvas->DrawCanvas();

    DrawHeadingInfo(1, 1);
    DrawToolInfo(1, 11);
    DrawButtons();

    return true;
}

bool TexturePainter::GetUserStartInput()
{
    // lambda expression for resetting
    auto ResetAsking = []() -> bool {
        system("pause");  // Wait for key press
        system("cls");    // Clear the screen
        return true;      // Indicate to 'continue' the loop
    };

    bool finishedInput{ false };
    int inputWidth{ 0 };
    int inputHeight{ 0 };
    int inputIllumination{ 0 };
    std::wstring userFileName;
    std::vector <std::wstring> availableFileList{};


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
        availableFileList = PrintAndGetFileList(SAVE_FOLDER, TEXTURE_EXTENSION);
        std::wcout << std::endl;

        PrintSelectedTexturesList();
        std::wcout << std::endl;

        // Prompt the user for a file
        int numberOfFiles{ static_cast<int>(availableFileList.size()) };
        std::wstring prompt{};
        std::wstring letter{ L"T" };
        if (numberOfFiles > 0)
        {
            prompt =  L"Please select an existing File Name(1-" + std::to_wstring(numberOfFiles) + L"), or ";
            letter = L"t";
        }
        prompt += letter + L"ype a new name to start new texture project ('q' to exit): ";

        if (!GetValidFileName(prompt, userFileName))
            return false; // if 'q' selected returns false and user input terminated

        // returns -1 if not an integer - used to test if user selecting file with index or typing in name
        int userSelection{ WStringToInteger(userFileName) };

        // if a valid number selection then load existing file 
        if (userSelection > 0 && userSelection <= numberOfFiles)
        {
            std::wstring name{ availableFileList.at(userSelection - 1) };

            if (IsFileAlreadySelected(name))
            {
                if (ResetAsking())
                    continue;
            }
            else // load texture from existing file
            {
                InitCanvasExistingTexture(name);
                selectedList.push_back(name);
            }
        }
        // if a non-valid number selection then restart
        else if (userSelection > numberOfFiles)
        {
            std::wcout << L"\nA file name cannot be a number.\n";
            if (ResetAsking())
                continue;
        }
        // else as userFileName is valid string test to see if file exists
        else 
        {
            if (IsFileAlreadySelected(userFileName + TEXTURE_EXTENSION))
            {
                if (ResetAsking())
                    continue;
            }
            else if (FileExistInDir(availableFileList, userFileName + TEXTURE_EXTENSION)) // if exists
            {
                // load texture from existing file
                InitCanvasExistingTexture(userFileName + TEXTURE_EXTENSION);
                selectedList.push_back(userFileName + TEXTURE_EXTENSION);
            }
            else
            {
                // get new dimentions
                GetDimensionInput(L"\nPlease enter an integer value for the new texture width: ", inputWidth, MIN_TEXTURE_WIDTH, MAX_TEXTURE_WIDTH);
                GetDimensionInput(L"\nPlease enter an integer value for the new texture height: ", inputHeight, MIN_TEXTURE_HEIGHT, MAX_TEXTURE_HEIGHT);
                GetDimensionInput(L"\nPlease enter illumination value 0 to 255 for the new texture: ", inputIllumination, 0, 255);

                // create and save new .txr file in saves path
                InitCanvasNewTexture(inputWidth, inputHeight, inputIllumination, userFileName + TEXTURE_EXTENSION);
                selectedList.push_back(userFileName + TEXTURE_EXTENSION);
            }
        }
        if (!GetYesNoInput(L"\nDo you want to add more Textures to edit? (y/n): "))
            finishedInput = true;

        // Clear the console screen
        system("cls"); // Only for Windows - by the nature of project!

    } while (finishedInput == false);


    // Output the entered textures (optional, for verification)
    for (Canvas* canvas : canvases)
    {
        std::wcout << L"\nFile Name:      " << canvas->GetFileName();
        std::wcout << L"\nSize (w * h):   " << canvas->GetTextureWidth() << L" x " << canvas->GetTextureHeight();
        std::wcout << L"\nIllumination:    " << canvas->GetIllumination() << std::endl << std::endl;
    }

    system("pause"); // wait for key press

    return true;
}

bool TexturePainter::InitCanvasNewTexture(int width, int height, int illumination, const std::wstring& fileName)
{
    // create new canvas
    Canvas* canvas = new Canvas(*this, width, height, illumination, SAVE_FOLDER, fileName, CANVAS_XPOS, CANVAS_YPOS);

    // save the new texture to save folder (create an empty file)
    canvas->SaveTexture(SAVE_FOLDER + fileName);
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
}

bool TexturePainter::InitCanvasExistingTexture(const std::wstring& fileName)
{
    // create new canvas with existing texture
    Canvas* canvas = new Canvas(*this, SAVE_FOLDER, fileName, CANVAS_XPOS, CANVAS_YPOS);

    // no need to save a file as texture already exists
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
}

void TexturePainter::PrintSelectedTexturesList()
{
    std::wcout << L"\n      --------- Displaying Current Canvas List ---------\n\n";

    short count{ 1 };
    if (canvases.size() != 0)
        for (Canvas* canvas : canvases)
        {
            std::wstring name{ canvas->GetFileName() };
            std::wcout << count << L". " << name << std::endl;
            count++;
        }
    else
        std::wcout << L"No canvases selected...\n";
}

bool TexturePainter::IsFileAlreadySelected(std::wstring fileName)
{
    if (std::find(selectedList.begin(), selectedList.end(), fileName) != selectedList.end())
    {
        std::wcout << L"\n **** The name '" << fileName << L"' is already selected. Please choose a different name ****" << std::endl;
        // If the name is found in the selection list, inform the user and continue the loop by returning true
        return true;
    }
    return false;
}

bool TexturePainter::ChangeCanvas(size_t index)
{
    if (index < canvases.size())
    {
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
    int row1{ x };
    int row2{ x + 60 };
    int row3{ x + 100 };

    // texture info
    WriteStringToBuffer(row1, y,     L"Current File Name:  " + currentCanvas->GetFileName(), FG_CYAN);
    WriteStringToBuffer(row1, y + 1, L"File Path:          " + currentCanvas->GetFilePath(), FG_CYAN);
    WriteStringToBuffer(row1, y + 3, L"Dimentions:         " + std::to_wstring(currentCanvas->GetTextureWidth()) + L" x " + std::to_wstring(currentCanvas->GetTextureHeight()));
    WriteStringToBuffer(row1, y + 4, L"Illuminatation:     " + std::to_wstring(currentCanvas->GetIllumination()));


    // current textures
    WriteStringToBuffer(row2, y,     L"Texture List:", FG_BLUE);
    int count{ 1 };
    for (const auto fileName : selectedList)
    {
        WriteStringToBuffer(row2, y + count + 1, std::to_wstring(count) + L". " + fileName, FG_BLUE);
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
    WriteStringToBuffer(x + 4, y, L"Brush");
    DrawBlock(x + 4, y + 2, currentCanvas->GetBrushSize(), currentCanvas->GetBrushColour() | BG_MAGENTA, PIXEL_THREEQUARTERS);
}

void TexturePainter::DrawButtons()
{
    colourButtonsContainer->DrawButtons();
    brushButtonsContainer->DrawButtons();
}


bool TexturePainter::HandleKeyPress()
{
    //controls

    // when left mouse button pressed
    if (keyArray[VK_LBUTTON].bPressed)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
            currentCanvas->ApplyToolToBrushTexture(mouseCoords);
        else
        {
            // check if over any of the buttons when clicked
            colourButtonsContainer->HandleMouseClick(mouseCoords);
            brushButtonsContainer->HandleMouseClick(mouseCoords);
        }

    }
    else if (keyArray[VK_LBUTTON].bHeld)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
            currentCanvas->ApplyToolToBrushTexture(mouseCoords);
    }


    // when left mouse lifted
    if (keyArray[VK_LBUTTON].bReleased)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
            currentCanvas->SetBrushTextureToBackground();
    }

    if (keyArray[VK_RBUTTON].bHeld)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
        {
            currentCanvas->SetBrushToDelete();
            currentCanvas->ApplyToolToBrushTexture(mouseCoords);
        }
    }

    // when right mouse lifted
    if (keyArray[VK_RBUTTON].bReleased)
    {
        if (currentCanvas->AreCoordsWithinCanvas(mouseCoords))
        {
            colourButtonsContainer->ActivateLastClicked();
            currentCanvas->SetBrushTextureToBackground();
        }
    }

    if (keyArray[VK_OEM_PLUS].bPressed)
    {
        currentCanvas->IncreaseZoomLevel();
    }

    if (keyArray[VK_OEM_MINUS].bPressed)
    {
        currentCanvas->DecreaseZoomLevel();
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
                ChangeCanvas(i);
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
