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
    SetWindowPosition(50, 50);

    // container for colour buttons
    colourButtonsContainer = new ButtonContainer(*this, COLOUR_BUTTON_XPOS, COLOUR_BUTTON_YPOS, 2, 9); 

    // populate it 
    for (short colour = 0; colour < 8; ++colour) // For simplicity directly using the index as the color here and OR with FG_INTENSITY.
    {
        colourButtonsContainer->AddButton(5, 5, colour, [this, colour]() { currentCanvas->SetBrushColour(colour); });
        colourButtonsContainer->AddButton(5, 5, colour | FG_INTENSITY, [this, colour]() { currentCanvas->SetBrushColour(colour | FG_INTENSITY); });
    }
    deleteToolIcon = new Texture(L"./ToolIcons/delete_tool_icon.txr");
    colourButtonsContainer->AddButton(11,3, 1, [this]() { currentCanvas->SetBrushToDelete(); });



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
    brushButtonsContainer->AddButton(blockToolIcon, [this]() { currentCanvas->SetBrushType(Canvas::BrushType::BRUSH_BLOCK); });
    brushButtonsContainer->AddButton(increaseToolIcon, [this]() { currentCanvas->ChangeBrushSize(1); });
    brushButtonsContainer->AddButton(decreaseToolIcon, [this]() { currentCanvas->ChangeBrushSize(-1); });
    brushButtonsContainer->AddButton(rectToolIcon, [this]() { currentCanvas->SetBrushType(Canvas::BrushType::BRUSH_RECT); });
    brushButtonsContainer->AddButton(rectFillToolIcon, [this]() { currentCanvas->SetBrushType(Canvas::BrushType::BRUSH_RECT_FILLED); });
    brushButtonsContainer->AddButton(lineToolIcon, [this]() { currentCanvas->SetBrushType(Canvas::BrushType::BRUSH_LINE); });

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
    Canvas* canvas = new Canvas(*this, new Texture(width, height, illumination), fileName, SAVE_FOLDER + fileName, CANVAS_XPOS, CANVAS_YPOS);

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
    Canvas* canvas = new Canvas(*this, existingTexture, fileName, SAVE_FOLDER + fileName, CANVAS_XPOS, CANVAS_YPOS);

    // no need to save a file as texture already exists
    // add to current selection of canvases to edit
    canvases.push_back(canvas);

    return true;
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
    // texture info
    WriteStringToBuffer(x, y,     L"Current File Name:  " + currentCanvas->GetFileName(), FG_CYAN);
    WriteStringToBuffer(x, y + 1, L"File Path:          " + currentCanvas->GetFilePath(), FG_CYAN);
    WriteStringToBuffer(x, y + 3, L"Dimentions:         " + std::to_wstring(currentCanvas->GetTextureWidth()) + L" x " + std::to_wstring(currentCanvas->GetTextureHeight()));
    WriteStringToBuffer(x, y + 4, L"Illuminatation:     " + std::to_wstring(currentCanvas->GetIllumination()));

    // instructions
    WriteStringToBuffer(x + 60, y,     L"Save Current Canvas    F5", FG_GREEN);
    WriteStringToBuffer(x + 60, y + 1, L"Load Current Canvas    F9", FG_GREEN);
    WriteStringToBuffer(x + 60, y + 2, L"Select Canvas          1-9", FG_GREEN);
    WriteStringToBuffer(x + 60, y + 3, L"Untitled Canvas        0", FG_GREEN);
    WriteStringToBuffer(x + 60, y + 4, L"Change Zoom            +  ", FG_GREEN);
    WriteStringToBuffer(x + 60, y + 5, L"Exit                   ESC", FG_GREEN);
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
    if ( keyArray[VK_LBUTTON].bPressed)
    {
        if (currentCanvas->IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
            currentCanvas->ApplyBrushTool(mouseCoords.X, mouseCoords.Y);

        // check if over any of the buttons when clicked
        colourButtonsContainer->HandleMouseClick(mouseCoords.X, mouseCoords.Y);
        brushButtonsContainer->HandleMouseClick(mouseCoords.X, mouseCoords.Y);
    }

    if (keyArray[VK_LBUTTON].bHeld)
    {
        if (currentCanvas->IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
        {
            currentCanvas->ApplyBrushPaint(mouseCoords.X, mouseCoords.Y);
        }
    }

    if (keyArray[VK_RBUTTON].bHeld)
    {
        if (currentCanvas->IsMouseWithinCanvas(mouseCoords.X, mouseCoords.Y))
        {
            currentCanvas->SetBrushToDelete();
            currentCanvas->ApplyBrushPaint(mouseCoords.X, mouseCoords.Y);
        }
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
        currentCanvas->ChangeBrushType(Canvas::BrushType::BRUSH_RECT);
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

    return true;
}
