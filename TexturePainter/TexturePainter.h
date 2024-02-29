#pragma once

#include "Utility.h"
#include "Texture.h"

class TexturePainter
{
    short nScreenWidth = 64;  // Console screen width
    short nScreenHeight = 64;  // Console screen height
    short nFontWidth = 10;      // Font width in pixels
    short nFontHeight = 10;    // Font height in pixels

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

    CHAR_INFO* screen;      // pointer to screen buffer
    SMALL_RECT windowSize;  // console window physical size

    bool InitialiseExistingSprite(const std::wstring& fileName)
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

    bool InitialiseNewSprite(int width, int height, bool illuminated, const std::wstring& fileName)
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


    bool GetUserStartInput()
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

    void WriteStringToBuffer(int x, int y, std::wstring string, short colour = 0x000F)
    {
        for (size_t i = 0; i < string.size(); i++)
        {
            screen[y * nScreenWidth + x + i].Char.UnicodeChar = string[i];
            screen[y * nScreenWidth + x + i].Attributes = colour;
        }
    }


    // check if within canvas
    bool WithinCanvas(int x, int y)
    {
        if (x >= nCanvasXpos && x <= nCanvasXpos + nCanvasSizeX && y >= nCanvasYpos && y <= nCanvasYpos + nCanvasSizeY)
            return true;
        else
            return false;
    }

    // only draw if in canvas
    void DrawPointOnSprite(int mouse_x, int mouse_y, short colour = FG_WHITE, short glyph = 0x2588)
    {
        if (WithinCanvas(mouse_x, mouse_y))
        {
            currentTexture->SetColour(mouse_x - nCanvasXpos, mouse_y - nCanvasYpos, colour);
            currentTexture->SetGlyph(mouse_x - nCanvasXpos, mouse_y - nCanvasYpos, glyph);
        }
    }

    void DrawPoint(int x, int y, short colour = FG_WHITE, short glyph = 0x2588)
    {
        if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
        {
            screen[y * nScreenWidth + x].Attributes = colour;
            screen[y * nScreenWidth + x].Char.UnicodeChar = glyph;

        }
    }

    void DrawSpriteToScreen(int x_offset, int y_offset)
    {
        for (int y{ 0 }; y < currentTexture->GetHeight(); y++)
        {
            for (int x{ 0 }; x < currentTexture->GetWidth(); x++)
            {
                DrawPoint(x_offset + x, y_offset + y, currentTexture->GetColour(x, y), currentTexture->GetGlyph(x, y));
            }

        }
    }


    // Bresenham's line algorithm
    void DrawLine(int x0, int y0, int x1, int y1, short colour = FG_WHITE, short glyph = 0x2588) {
        int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy, e2; /* error value e_xy */

        while (true) {
            if (x0 >= 0 && x0 < nScreenWidth && y0 >= 0 && y0 < nScreenWidth) {
                DrawPoint(x0, y0, colour, glyph); // Set the character at the current position
            }
            if (x0 == x1 && y0 == y1) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    void DrawSquare(int x, int y, int sideLength, short colour = FG_WHITE, short glyph = 0x2588) {
        // Top side
        DrawLine(x, y, x + sideLength, y, colour, glyph);
        // Right side
        DrawLine(x + sideLength, y, x + sideLength, y + sideLength, colour, glyph);
        // Bottom side
        DrawLine(x, y + sideLength, x + sideLength, y + sideLength, colour, glyph);
        // Left side
        DrawLine(x, y, x, y + sideLength, colour, glyph);
    }

    void DrawRectangle(int x0, int y0, int x1, int y1, short colour = FG_WHITE, short glyph = 0x2588) {
        // Top side
        DrawLine(x0, y0, x1, y0, colour, glyph);
        // Right side
        DrawLine(x1, y0, x1, y1, colour, glyph);
        // Bottom side
        DrawLine(x0, y1, x1, y1, colour, glyph);
        // Left side
        DrawLine(x0, y0, x0, y1, colour, glyph);
    }



    int main() {

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

        SetResizeWinowLock(true);


        // right... this is painful - let's go!

        // grab OG buffer - will revert to on clean-up
        HANDLE hOriginalBuffer = GetStdHandle(STD_OUTPUT_HANDLE);

        // adjust screen size
        nScreenWidth = currentTexture->GetWidth() + 32;
        nScreenHeight = currentTexture->GetHeight() + 32;

        // Create a new display screen buffer
        screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
        memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);


        // create new console buffer
        HANDLE hNewBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        if (hNewBuffer == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create screen buffer." << std::endl;
            delete[] screen;  // Clean up allocated memory
            return 1;
        }

        // temp size for screen so can be resized later without issue
        windowSize = { 0, 0, 1, 1 };
        SetConsoleWindowInfo(hNewBuffer, TRUE, &windowSize);

        // set new buffer to active buffer
        if (!SetConsoleActiveScreenBuffer(hNewBuffer)) {
            std::cerr << "Failed to set active screen buffer." << std::endl;
            CloseHandle(hNewBuffer);
            delete[] screen;  // Clean up allocated memory
            return 1;
        }

        // Set the console font size
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = nFontWidth;   // Width of each character in pixels
        cfi.dwFontSize.Y = nFontHeight;  // Height
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;

        // Choose a font, the name can be changed as needed
        wcscpy_s(cfi.FaceName, L"Lucida Console");
        // set font/ont size etc
        if (!SetCurrentConsoleFontEx(hNewBuffer, FALSE, &cfi)) {
            std::cerr << "Failed to set console font size." << std::endl;
            CloseHandle(hNewBuffer);
            delete[] screen;  // Clean up allocated memory
            return 1;
        }

        // Set the screen buffer size to match the window size
        // jeez_louise this needs to be done before setting console window size, ugh :(
        COORD bufferSize = { nScreenWidth, nScreenHeight };
        if (!SetConsoleScreenBufferSize(hNewBuffer, bufferSize)) {
            std::cerr << "Failed to set screen buffer size." << std::endl;
            CloseHandle(hNewBuffer);
            delete[] screen;  // Clean up allocated memory
            return 1;
        }

        // inform why failed to resize
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hNewBuffer, &csbi)) {
            std::cerr << "GetConsoleScreenBufferInfo failed" << std::endl;
            return 1;
        }
        if (nScreenHeight > csbi.dwMaximumWindowSize.Y) {
            std::cerr << "New Screen Height or Font Height Too Big for screen" << std::endl;
            SetResizeWinowLock(false);
            return 1;
        }
        if (nScreenWidth > csbi.dwMaximumWindowSize.X) {
            std::cerr << "New Screen Width or Font Width Too Big for screen" << std::endl;
            SetResizeWinowLock(false);
            return 1;
        }

        // Set the console window size to app size
        windowSize = { 0, 0, (short)(nScreenWidth - 1), (short)(nScreenHeight - 1) };
        if (!SetConsoleWindowInfo(hNewBuffer, TRUE, &windowSize)) {
            std::cerr << "Failed to set console window size." << std::endl;
            CloseHandle(hNewBuffer);
            delete[] screen;  // Clean up allocated memory
            return 1;
        }

        // 'double word' type storage of number of characters written to screen buffer
        //DWORD dwBytesWritten = 0;

        // grab input buffer
        HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

        // Enable mouse input
        SetConsoleMode(hConsoleInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT);

        // storage of events
        INPUT_RECORD irInputBuffer[128];    // stores a structure representing input event in console
        DWORD dwEventsRead = 0;                 // number of events


        bool bRunning{ true };
        COORD mousePos{ (short)0, (short)0 };

        while (bRunning) {

            ReadConsoleInput(hConsoleInput, irInputBuffer, 128, &dwEventsRead);

            // Handle events
            for (DWORD i = 0; i < dwEventsRead; ++i) {

                switch (irInputBuffer[i].EventType) {
                case MOUSE_EVENT:
                    MOUSE_EVENT_RECORD mouseEvent = irInputBuffer[i].Event.MouseEvent;
                    mousePos = mouseEvent.dwMousePosition;

                    if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        // Calculate buffer index and update screen buffer if mouse is within bounds
                        int index = mousePos.Y * nScreenWidth + mousePos.X;
                        if (index >= 0 && index < (nScreenWidth * nScreenHeight)) {
                            // leave a trail
                            DrawPointOnSprite(mousePos.X, mousePos.Y, currentColour, currentGlyph);
                        }
                    }
                    else if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                        // Calculate buffer index and update screen buffer if mouse is within bounds
                        int index = mouseEvent.dwMousePosition.Y * nScreenWidth + mouseEvent.dwMousePosition.X;
                        if (index >= 0 && index < nScreenWidth * nScreenHeight) {
                            // draw 'blank' char
                            DrawPointOnSprite(mousePos.X, mousePos.Y, BG_BLACK, L' ');
                        }
                    }
                    break;

                case KEY_EVENT:
                    KEY_EVENT_RECORD keyEvent = irInputBuffer[i].Event.KeyEvent;

                    // Check if the 'q' key was pressed
                    if (keyEvent.bKeyDown && (keyEvent.wVirtualKeyCode == 'Q' || keyEvent.wVirtualKeyCode == 'q')) {
                        bRunning = false;  // Set running to false to exit the loop
                        break;  // Exit the for loop
                    }
                    else if (keyEvent.bKeyDown && (keyEvent.wVirtualKeyCode == 'S' || keyEvent.wVirtualKeyCode == 's')) {
                        currentTexture->SaveAs(sCurrentFilePath);  // Save current texture
                        break;  // Exit the for loop
                    }
                    else if (keyEvent.bKeyDown && (keyEvent.wVirtualKeyCode == 'L' || keyEvent.wVirtualKeyCode == 'l')) {
                        currentTexture->LoadFrom(sCurrentFilePath);  // load current texture
                        break;  // Exit the for loop
                    }
                    else if (keyEvent.bKeyDown && (keyEvent.wVirtualKeyCode == '1')) {
                        currentColour = FG_RED;
                        break;  // Exit the for loop
                    }
                    else if (keyEvent.bKeyDown && (keyEvent.wVirtualKeyCode == '2')) {
                        currentColour = FG_GREEN;
                        break;  // Exit the for loop
                    }
                    else if (keyEvent.bKeyDown && (keyEvent.wVirtualKeyCode == '3')) {
                        currentColour = FG_BLUE;
                        break;  // Exit the for loop
                    }

                    break;

                default:
                    break;

                } // end of switch statement
            }

            // square around canvas position
            DrawRectangle(nCanvasXpos - 1, nCanvasYpos - 1, nCanvasXpos + nCanvasSizeX, nCanvasYpos + nCanvasSizeY, FG_RED, L'#');

            // add texture to screen buffer
            DrawSpriteToScreen(nCanvasXpos, nCanvasYpos);


            // update info display
            WriteStringToBuffer(1, 1, L"Current File Name: " + sCurrentFileName);
            WriteStringToBuffer(1, 3, L"Dimentions: " + std::to_wstring(nCanvasSizeX) + L" x " + std::to_wstring(nCanvasSizeY));
            WriteStringToBuffer(1, 5, L"                                                    "); // hack to clear mouse position info
            WriteStringToBuffer(1, 5, L"Canvas COORDS X: " + std::to_wstring(mousePos.X - nCanvasXpos) + L", Y: " + std::to_wstring(mousePos.Y - nCanvasYpos));

            // Update Title & Present Screen Buffer
            wchar_t s[256];
            std::wstring appTitle = L"Texture Painter! - " + sCurrentFileName + L" " + std::to_wstring(nCanvasSizeX) + L" x " + std::to_wstring(nCanvasSizeY);
            SetConsoleTitle(appTitle.c_str());
            WriteConsoleOutput(hNewBuffer, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &windowSize);


            //Sleep(10); // Reduce CPU usage
        }

        // Cleanup and restore the original buffer before exiting
        SetConsoleActiveScreenBuffer(hOriginalBuffer);  // Restore the original buffer
        CloseHandle(hNewBuffer);  // Close the new buffer handle
        SetResizeWinowLock(false); // allow resizing of console again

        delete[] screen;  // Free allocated memory
        delete currentTexture;

        return 0;

    }
};

