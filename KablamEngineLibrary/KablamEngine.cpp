// KablamEngine.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "KablamEngine.h"

// undefine the macros from windows.h
#undef max
#undef min

// constructor

KablamEngine::KablamEngine()
    :nScreenWidth{ 30 }, nScreenHeight{ 10 }, nFontWidth{ 5 }, nFontHeight{ 10 }, hConsoleWindow{ GetConsoleWindow() },
    hOGBuffer{ GetStdHandle(STD_OUTPUT_HANDLE) }, hConsoleInput{ GetStdHandle(STD_INPUT_HANDLE) }, hNewBuffer{ INVALID_HANDLE_VALUE },
    dwPreviousConsoleMode{ 0 }, screen{ nullptr }, screenBilinear{ nullptr }, bBilinearApplied{ false }, nScreenArrayLength { 0 }, 
    windowSize{ 0, 0, 1, 1 }, mouseCoords{ 0,0 }, sConsoleTitle{ L"no_name_given" }, bConsoleFocus{ true }, bGameUpdatePaused{ false },
    bFocusPause{ false }, bFullScreen{ false } {

    // initialise storage of input events - key and mouse
    // use standard library fill
    std::fill(std::begin(newKeyStateArray), std::end(newKeyStateArray), false);
    std::fill(std::begin(oldKeyStateArray), std::end(oldKeyStateArray), false);

    for (KeyState& key : keyArray) {
        key.bPressed = false;
        key.bHeld = false;
        key.bReleased = false;
    }
    
    // grab the ptr to this instance into static instance (needed for handling closing console)
    instance = this;

    // set how to handle a close console event
    SetConsoleCtrlHandler(KablamEngine::ConsoleControlHandler, TRUE);
}

// destructor
KablamEngine::~KablamEngine()
{
    SetConsoleActiveScreenBuffer(hOGBuffer);

    // Close the handle if they are still valid
    if (hNewBuffer != INVALID_HANDLE_VALUE) {
        CloseHandle(hNewBuffer);
        hNewBuffer = INVALID_HANDLE_VALUE; // Prevent reuse
    }

    delete[] screen;
    screen = nullptr;
    delete[] screenBilinear;
    screenBilinear = nullptr;
}


// member methods

//virtual method that needs overriding
bool KablamEngine::OnGameCreate()
{
    // load textures/sprites/levels etc.
    return false;
}

// virtual method that needs overriding
bool KablamEngine::OnGameUpdate(float elaspsedTime)
{
    return false;
}

int KablamEngine::BuildConsole(int screenWidth, int screenHeight, int pixelWidth, int pixelHeight, int winPosX, int winPosY)
{
    // set the users console size
    nScreenWidth = screenWidth;  // Console screen width
    nScreenHeight = screenHeight;  // Console screen height
    nFontWidth = pixelWidth;      // Font width in pixels
    nFontHeight = pixelHeight;    // Font height in pixels

    // right... this is painful - let's go!

    if (hOGBuffer == INVALID_HANDLE_VALUE)
        return Error(L"Bad STD_OUTPUT_HANDLE.");

    if (hConsoleInput == INVALID_HANDLE_VALUE)
        return Error(L"Bad INVALID_HANDLE_VALUE.");

    // Create a new screen buffer
    nScreenArrayLength = nScreenWidth * nScreenHeight;
    screen = new CHAR_INFO[nScreenArrayLength];
    // clear memory for screen buffer
    memset(screen, 0, sizeof(CHAR_INFO) * nScreenArrayLength);

    screenBilinear = new CHAR_INFO[nScreenArrayLength];
    // clear memory for screen buffer
    memset(screenBilinear, 0, sizeof(CHAR_INFO) * nScreenArrayLength);

    // create new console buffer
    hNewBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (hNewBuffer == INVALID_HANDLE_VALUE) {
        delete[] screen;  // Clean up allocated memory
        delete[] screenBilinear;  // Clean up allocated memory
        return Error(L"Failed to CreateConsoleScreenBuffer.");
    }

    // temp size for screen so can be resized later without issue
    // start with windowSize = { 0, 0, 1, 1 };
    if (!SetConsoleWindowInfo(hNewBuffer, TRUE, &windowSize)) {
        CleanUp();
        return Error(L"Failed to SetConsoleWindowInfo.");
    }

    // set new buffer to active buffer
    if (!SetConsoleActiveScreenBuffer(hNewBuffer)) {
        CleanUp();
        return Error(L"Failed to SetConsoleActiveScreenBuffer.");
    }

    // Set the console font size
    CONSOLE_FONT_INFOEX cfi{};
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = nFontWidth;   // Width of each character in pixels
    cfi.dwFontSize.Y = nFontHeight;  // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    // Choose a font, the name can be changed as needed
    wcscpy_s(cfi.FaceName, L"Lucida Console");
    // set font/font size etc
    if (!SetCurrentConsoleFontEx(hNewBuffer, FALSE, &cfi)) {
        CleanUp();
        return Error(L"Failed to SetCurrentConsoleFontEx.");
    }

    // Set the screen buffer size to match the window size
    // jeez_louise this needs to be done before setting console window size, ugh :(
    COORD bufferSize = { nScreenWidth, nScreenHeight };
    if (!SetConsoleScreenBufferSize(hNewBuffer, bufferSize)) {
        CleanUp();
        return Error(L"Failed to SetConsoleScreenBufferSize");
    }

    // grab info so able to inform why failed to resize
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hNewBuffer, &csbi)) {
        CleanUp();
        return Error(L"Unable to GetConsoleScreenBufferInfo.");
    }

    // Set the console window size to app size
    windowSize = { 0, 0, (short)(nScreenWidth - 1), (short)(nScreenHeight - 1) };

    // Set console with new window size
    if (!SetConsoleWindowInfo(hNewBuffer, TRUE, &windowSize)) {

        const wchar_t* error_message{ nullptr };

        if (nScreenHeight > csbi.dwMaximumWindowSize.Y) {
            error_message = L"Failed to SetConsoleWindowInfo: New Screen Height or Font Height Too Big for screen.";
            //SetResizeWinowLock(false);
        }
        else if (nScreenWidth > csbi.dwMaximumWindowSize.X) {
            error_message = L"Failed to SetConsoleWindowInfo: New Screen Width or Font Width Too Big for screen.";
            //SetResizeWinowLock(false);
        }
        else
            error_message = L"Failed to SetConsoleWindowInfo: Unknown reason... sorry.";

        CleanUp();
        return Error(error_message);
    }

    // Enable mouse input
    // save previous mode
    GetConsoleMode(hConsoleInput, &dwPreviousConsoleMode);
    // set new mode
    if (!SetConsoleMode(hConsoleInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT)) {
        CleanUp();
        return Error(L"Failed to SetConsoleMode 'ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT'");
    }

    SetWindowPosition(winPosX, winPosY);

    AddToLog(L"BuildConsole() successful.");

    return 0; // successful Building of the console!
}

void KablamEngine::Start()
{
    if (bGameThreadRunning == false)
    {
        bGameThreadRunning = true;

        GameThread();

    }
    else
    {
        Error(L"Game Thread Already running.");
    }


    CleanUp(); // delete[] screen etc...

    AddToLog(L"Exiting Start().");

    // send contents of outputLog to outoutLog.txt
    OutputLogFile();

    // notify the condition variable that CleanUp/tidy-up/log report has finished 
    KablamEngine::bGracefulExitCompleted = true;
    KablamEngine::shutdownCv.notify_all();
}


int KablamEngine::GameThread()
{
    // create user defined game!
    if (OnGameCreate() == false)
        return Error(L"OnUserCreate not defined in, or returning false from, derived class.");

    AddToLog(L"OnGameCreate() loaded.");

    // handle timing
    // get elapsed time to regulate update method
    tp1 = std::chrono::system_clock::now();
    tp2 = std::chrono::system_clock::now();


        /////       MAIN GAME LOOP      /////

    while (bGameThreadRunning == true)
    {
        // handling input
        KablamEngine::UpdateInputStates();

        // exit app with ESCAPE
        if (keyArray[VK_ESCAPE].bPressed && bConsoleFocus)
        {
            AddToLog(L"ESCAPE key pressed, exiting program.");
            bGameThreadRunning = false;
            bGameUpdatePaused = true;
        }

        // toggle FULL SCREEN with F12
        if (keyArray[VK_F12].bPressed && bConsoleFocus)
        {
            // could change to toggle fullscreen but prefer explicit true/false
            bFullScreen = !bFullScreen;
            SetFullScreen(bFullScreen);
        }

        if (!bGameUpdatePaused) // Check if the game is paused
        {
            // Handle Timing
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;
            float fElapsedTime = elapsedTime.count();

            // (user defined) game update
            if (OnGameUpdate(fElapsedTime) == false)
                return Error(L"OnUserUpdate not defined in, or returning false from, derived class.");

            // Update Title
            wchar_t title[256];
            swprintf_s(title, 256, L"%s (%i x %i) FPS: %3.1f", sConsoleTitle.c_str(), nScreenWidth, nScreenHeight, GetAverageFPS(fElapsedTime));
            SetConsoleTitle(title);

            // Update Console Buffer with screen buffer
            WriteConsoleOutput(hNewBuffer, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &windowSize);

        }
        else
        {
            Sleep(100); // Sleep briefly to reduce CPU usage
        }

    }
        /////   END OF MAIN GAME LOOP   /////


    AddToLog(L"Exiting GameThread().");

    return 0;
}

int KablamEngine::GetConsoleWidth()
{
    return nScreenWidth;
}

int KablamEngine::GetConsoleHeight()
{
    return nScreenHeight;
}

void KablamEngine::WriteStringToBuffer(int x, int y, const std::wstring& string, short colour)
{
    if (y < 0 || y >= nScreenHeight) return; // Y out of bounds

    for (size_t i = 0; i < string.size(); i++)
    {
        if (x + i < 0 || x + i >= nScreenWidth) continue; // X out of bounds

        screen[y * nScreenWidth + x + i].Char.UnicodeChar = string[i];
        screen[y * nScreenWidth + x + i].Attributes = colour;
    }
}

void KablamEngine::DrawPoint(int x, int y, short colour, short glyph)
{
    if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
    {
        screen[y * nScreenWidth + x].Attributes = colour;
        screen[y * nScreenWidth + x].Char.UnicodeChar = glyph;
    }
    else
    {
        //AddToLog(L"Attempting to DrawPoint() outside of screen array.");
    }
}

void KablamEngine::DrawPoint(int x, int y, const CHAR_INFO& pixel)
{
    if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
    {
        screen[y * nScreenWidth + x].Attributes = pixel.Attributes;
        screen[y * nScreenWidth + x].Char.UnicodeChar = pixel.Char.UnicodeChar;
    }
    else
    {
        //AddToLog(L"Attempting to DrawPoint() outside of screen array.");
    }
}

void KablamEngine::DrawBlock(int x, int y, int size, short colour, short glyph)
{
    for (int xp{ 0 }; xp < size; xp++)
        for (int yp{ 0 }; yp < size; yp++)
            DrawPoint(x + xp, y + yp, colour, glyph);

}

// Bresenham's line algorithm
// ChatGPT used to add line thickness... draws a square along each point of the line
void KablamEngine::DrawLine(int x0, int y0, int x1, int y1, short colour, short glyph, int lineThickness)
{
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    // Adjust the start point for even thickness to ensure symmetric distribution
    int offset = lineThickness % 2 == 0 ? 1 : 0;

    while (true) {
        // Draw a square of thickness around the current point
        for (int i = -lineThickness / 2; i <= lineThickness / 2 - offset; ++i) {
            for (int j = -lineThickness / 2; j <= lineThickness / 2 - offset; ++j) {
                DrawPoint(x0 + i, y0 + j, colour, glyph);
            }
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// top left and bottom right coords
void KablamEngine::DrawRectangleCoords(int x0, int y0, int x1, int y1, short colour, bool filled, short glyph, int lineWidth) {
    // Normalize coordinates
    int left = std::min(x0, x1);
    int top = std::min(y0, y1);
    int right = std::max(x0, x1);
    int bottom = std::max(y0, y1);

    if (filled) {
        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                DrawPoint(x, y, colour, glyph);
            }
        }
    }
    else {
        // Top and bottom sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int x = left; x <= right; ++x) {
                DrawPoint(x, top + i, colour, glyph); // Top side
                DrawPoint(x, bottom - i, colour, glyph); // Bottom side
            }
        }
        // Left and right sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int y = top; y <= bottom; ++y) {
                DrawPoint(left + i, y, colour, glyph); // Left side
                DrawPoint(right - i, y, colour, glyph); // Right side
            }
        }
    }
}

// top left coords and width and height
void KablamEngine::DrawRectangleEdgeLength(int x, int y, int width, int height, short colour, bool filled, short glyph, int lineWidth) {
    // Calculate the right and bottom edges of the rectangle
    int right = x + width - 1;  // Subtract 1 to include starting point
    int bottom = y + height - 1;  // Subtract 1 to include starting point

    if (filled) {
        for (int yPos = y; yPos <= bottom; ++yPos) {
            for (int xPos = x; xPos <= right; ++xPos) {
                DrawPoint(xPos, yPos, colour, glyph);  // Assuming DrawPoint takes colour and glyph
            }
        }
    }
    else {
        // Top and bottom sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int xPos = x; xPos <= right; ++xPos) {
                DrawPoint(xPos, y + i, colour, glyph);  // Top side
                DrawPoint(xPos, bottom - i, colour, glyph);  // Bottom side
            }
        }
        // Left and right sides
        for (int i = 0; i < lineWidth; ++i) {
            for (int yPos = y + i; yPos <= bottom - i; ++yPos) {
                DrawPoint(x + i, yPos, colour, glyph);  // Left side, adjusted for lineWidth
                DrawPoint(right - i, yPos, colour, glyph);  // Right side, adjusted for lineWidth
            }
        }
    }
}


// GhatGPT solution to circle drawing
void KablamEngine::DrawCircle(int xCenter, int yCenter, int radius, short colour, short glyph, bool filled)
{
    auto drawPoint = [&](int x, int y) {
        DrawPoint(x, y, glyph, colour);
    };

    auto plotCirclePoints = [&](int xc, int yc, int x, int y) {
        if (filled) {
            // Draw horizontal lines to fill the circle
            DrawLine(xc - x, yc + y, xc + x, yc + y, colour, glyph);
            DrawLine(xc - x, yc - y, xc + x, yc - y, colour, glyph);
            DrawLine(xc - y, yc + x, xc + y, yc + x, colour, glyph);
            DrawLine(xc - y, yc - x, xc + y, yc - x, colour, glyph);
        }
        else {
            // Draw only the edge points
            drawPoint(xc + x, yc + y);
            drawPoint(xc - x, yc + y);
            drawPoint(xc + x, yc - y);
            drawPoint(xc - x, yc - y);
            drawPoint(xc + y, yc + x);
            drawPoint(xc - y, yc + x);
            drawPoint(xc + y, yc - x);
            drawPoint(xc - y, yc - x);
        }
    };

    int x = 0;
    int y = radius;
    int p = 1 - radius;

    plotCirclePoints(xCenter, yCenter, x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        }
        else {
            y--;
            p += 2 * (x - y) + 1;
        }
        plotCirclePoints(xCenter, yCenter, x, y);
    }
}

// this is for drawing texture directly to screen pixels
int KablamEngine::DrawTextureToScreen(const Texture& texture, int xScreen, int yScreen, float scale, bool showEmptyPix, bool greyedOut)
{
    for (int x{ 0 }; x < texture.GetWidth() * scale; x++)
    {
        for (int y = 0; y < texture.GetHeight() * scale; y++)
        {
            // Find the corresponding texture coordinates
            int texX = static_cast<int>(x / scale);
            int texY = static_cast<int>(y / scale);

            short glyph{ texture.GetGlyph(texX, texY) };
            short colour{ texture.GetColour(texX, texY) };

            // for displaying 'empty' pixels
            if (glyph == L' ' && showEmptyPix)
            {
                glyph = L'X';
                colour = FG_DARK_GREY;
            }
            else if (greyedOut) // useful for buttons etc.
            {
                colour = colour;
                glyph = PIXEL_QUARTER;
            }

            // Draw scaled pixel
            DrawPoint(xScreen + x, yScreen + y, colour, glyph);
        }
    }
    return 0; 
}

// this is for drawing texture directly to screen pixels - any L' ' glyphs will be ignored (can see background)
int KablamEngine::DrawPartialTextureToScreen(const Texture& texture, int xScreen, int yScreen, float scale)
{
    for (int x = 0; x < static_cast<int>(texture.GetWidth() * scale); x++)
    {
        for (int y = 0; y < static_cast<int>(texture.GetHeight() * scale); y++)
        {
            // Find the corresponding texture coordinates, adjusted for scale
            int texX = static_cast<int>(x / scale);
            int texY = static_cast<int>(y / scale);

            short glyph{ texture.GetGlyph(texX, texY) };
            short colour{ texture.GetColour(texX, texY) };

            // Only draw pixels, not 'empty' spaces represented by ' '
            if (glyph != L' ')
            {
                // Add screenPos to the calculated position to get the final screen coordinates
                DrawPoint(xScreen + x, yScreen + y, colour, glyph);
            }
        }
    }
    return 0;
}

// overloaded to take a COORD 
int KablamEngine::DrawPartialTextureToScreen(const Texture& texture, COORD screenPos, float scale)
{
    return DrawPartialTextureToScreen(texture, screenPos.X, screenPos.Y, scale);
}

// drawing a rectangular section defined by x0 y0 and x1  y1 to screen - xScreen, yScreen will be top left
int KablamEngine::DrawSectionOfTextureToScreen(const Texture& texture, int xScreen, int yScreen, int x0, int y0, int x1, int y1, float scale, bool showEmptyPix)
{
    // Clamp the rectangle coordinates to ensure they are within the texture's dimensions
    x0 = std::max(x0, 0);
    y0 = std::max(y0, 0);
    x1 = std::min(x1, texture.GetWidth() - 1);
    y1 = std::min(y1, texture.GetHeight() - 1);

    for (int x = x0; x <= x1; x++)
    {
        for (int y = y0; y <= y1; y++)
        {
            // Find the corresponding texture coordinates
            int texX = x; // No need to adjust by scale since we're looping over actual texture coordinates
            int texY = y;

            short glyph{ texture.GetGlyph(texX, texY) };
            short colour{ texture.GetColour(texX, texY) };

            // Modify behavior based on showEmptyPix flag
            if (showEmptyPix && glyph == L' ') {
                glyph = L'X';  // Change glyph to 'X'
                colour = FG_DARK_GREY;
            }
            
            DrawBlock(xScreen + static_cast<int>((x - x0) * scale), yScreen + static_cast<int>((y - y0) * scale), scale, colour, glyph);
        }
    }
    return 0;
}


// overloaded method for using COORD for positions and rectangle corners
int KablamEngine::DrawSectionOfTextureToScreen(const Texture& texture, COORD screenPos, COORD topLeft, COORD bottomRight, float scale, bool showEmptyPix)
{
    // Clamp the rectangle coordinates to ensure they are within the texture's dimensions
    topLeft.X = std::max(topLeft.X, (short)0);
    topLeft.Y = std::max(topLeft.Y, (short)0);
    bottomRight.X = std::min(bottomRight.X, (short)(texture.GetWidth() - 1));
    bottomRight.Y = std::min(bottomRight.Y, (short)(texture.GetHeight() - 1));

    for (int x = topLeft.X; x <= bottomRight.X; x++)
    {
        for (int y = topLeft.Y; y <= bottomRight.Y; y++)
        {
            // Find the corresponding texture coordinates
            int texX = x; // No need to adjust by scale since we're looping over actual texture coordinates
            int texY = y;

            short glyph{ texture.GetGlyph(texX, texY) };
            short colour{ texture.GetColour(texX, texY) };

            // Modify behavior based on showEmptyPix flag
            if (showEmptyPix && glyph == L' ') {
                glyph = L'X';  // Change glyph to 'X'
                colour = FG_DARK_GREY;
            }

            DrawBlock(screenPos.X + static_cast<int>((x - topLeft.X) * scale), screenPos.Y + static_cast<int>((y - topLeft.Y) * scale), scale, colour, glyph);
        }
    }
    return 0;
}

// goes through screen[] and applies linear glyph shading
void KablamEngine::ApplyDominantColorGlyphBlend()
{
    // iterate through screen array to smooth out display
    for (int x{ 0 }; x < nScreenWidth; x++)
    {
        for (int y{ 0 }; y < nScreenHeight; y++)
        {
            // samples center, above, right, down, and left texel
            Colour5Sample colourSample;
            SampleSurroundingTexels(x, y, colourSample);

            std::map<short, int> colourMap;

            colourMap[colourSample.c]++;
            colourMap[colourSample.c00]++;
            colourMap[colourSample.c01]++;
            colourMap[colourSample.c10]++;
            colourMap[colourSample.c11]++;

            // defaults if only one colour
            short fgColour{ colourSample.c };
            short bgColour{ BG_BLACK };
            short glyph{ PIXEL_SOLID };

            if (colourMap.size() > 1)
            {
                // pairs to hold the colour (short value) and count of two main colours
                std::pair<short, int> dominantColour = { 0, 0 };
                std::pair<short, int> secondColour = { 0, 0 };

                TwoMainColourCounts(colourMap, dominantColour, secondColour);

                // 
                fgColour = dominantColour.first;
                bgColour = secondColour.first;

                // calculate ratio of main colour to determine glyph 'shade'
                float ratio = secondColour.second == 0 ? 1 : dominantColour.second / (float)(dominantColour.second + secondColour.second); // ratio of main colour

                // the ratios of two colours can be 4:1, 3:2, 3:1, 2:2, 1:1 -> 0.8, 0.6 or 0.5.
                if (ratio > 0.81f)
                    glyph = PIXEL_SOLID;
                else if (ratio > 0.61f)
                    glyph = PIXEL_THREEQUARTERS;
                else 
                    glyph = PIXEL_HALF;

            }

            // assign colors and glyph to bilinear array
            screenBilinear[y * nScreenWidth + x].Attributes = fgColour | (bgColour << 4);
            screenBilinear[y * nScreenWidth + x].Char.UnicodeChar = glyph;
        }
    }

    // copy mem accross in screen from screenBilinear
    // easier than setting flags and managing two screen arrays but possibly more costly - seems fast on testing
    memcpy_s(screen, nScreenWidth * nScreenHeight * sizeof(screen[0]), screenBilinear, nScreenWidth * nScreenHeight * sizeof(screen[0]));
}

// 
void KablamEngine::SampleSurroundingTexels(int x, int y, Colour5Sample& sample)
{
    //sample the four texels 
    sample.c = screen[y * nScreenWidth + x].Attributes;                                       // center pixel
    sample.c00 = screen[y * nScreenWidth + std::min(x + 1, nScreenWidth - 1)].Attributes;     // right
    sample.c01 = screen[y * nScreenWidth + std::max(x - 1, 0)].Attributes;                    // left
    sample.c10 = screen[std::min(y + 1, nScreenHeight - 1) * nScreenWidth + x].Attributes;    // bottom
    sample.c11 = screen[std::max(y - 1, 0) * nScreenWidth + x].Attributes;                    // top

}


void KablamEngine::TwoMainColourCounts(const std::map<short, int>& colourMap, std::pair<short, int>& firstColour, std::pair<short, int>& secondColour)
{
    // iterate through to get two main colours
    for (const auto& pair : colourMap) {
        if (pair.second > firstColour.second) {
            secondColour = firstColour; // Second becomes what used to be the max
            firstColour = pair;
        }
        else if (pair.second > secondColour.second) {
            secondColour = pair;
        }
    }
}

void KablamEngine::UpdateInputStates() // mouse location and focus state of window
{
    for (int i{ 0 }; i < 256; i++)
    {
        // reset pressed and released (as one-off events)
        keyArray[i].bPressed = false;
        keyArray[i].bReleased = false;

        // get all key state for virtual key codes 0 - 255
        newKeyStateArray[i] = GetAsyncKeyState(i);

        // store a reference to speed access as needed multiple times
        KeyState& key = keyArray[i];
        short& newKeyState = newKeyStateArray[i];

        // if new state is different from old
        if (newKeyState != oldKeyStateArray[i])
        {
            // most significant bit set means key currently 'down'
            if (newKeyState & 0x8000)
            {
                // only flag pressed if not being already held
                key.bPressed = !key.bHeld;
                // now being held
                key.bHeld = true;
            }
            else // otherwise change in state means key released and not held any more
            {
                key.bReleased = true;
                key.bHeld = false;
            }
        }
        oldKeyStateArray[i] = newKeyStateArray[i];
    }

    // Handle Mouse Input - Check for window events
    INPUT_RECORD inputBuffer[INPUT_BUFFER_SIZE];
    DWORD events = 0;
    GetNumberOfConsoleInputEvents(hConsoleInput, &events);
       

    // check if any events
    if (events > 0) {
        // ensure the buffer size is respected
        ReadConsoleInput(hConsoleInput, inputBuffer, INPUT_BUFFER_SIZE, &events); //ensure the buffer size is respected

        for (DWORD i = 0; i < events; ++i) {
            switch (inputBuffer[i].EventType) {

            case FOCUS_EVENT: {
                // Update console focus state
                bConsoleFocus = inputBuffer[i].Event.FocusEvent.bSetFocus;

                // Log focus state change
                std::wstring logMessage = bConsoleFocus ? L"Console window in focus." : L"Console window out of focus.";
                AddToLog(logMessage);

                // Handle game pause/resume based on focus state and bFocusPause setting
                if (bFocusPause) {
                    if (bConsoleFocus) {
                        UnPauseGameUpdate();
                    }
                    else {
                        PauseGameUpdate();
                    }
                }
                break;
            }

            case MOUSE_EVENT: {
                // Update mouse coordinates
                mouseCoords = inputBuffer[i].Event.MouseEvent.dwMousePosition;
                break;
            }

            default: {
                break;
            }
                   // more event handling if needed...
            }
        }
    }
}

// Set if window pauses when out of focus
void KablamEngine::SetConsoleFocusPause(bool state)
{
    bFocusPause = state;
    std::wstring logMessage = state ? L"Console window set TO PAUSE on loss of focus." : L"Console window set TO NOT PAUSE on loss of focus.";
    AddToLog(logMessage);
}

bool KablamEngine::GetConsoleFocus()
{
    return bConsoleFocus;
}

// Handle pause and unpause of GameUpdate (not GameThread)
void KablamEngine::PauseGameUpdate()
{
    bGameUpdatePaused = true;
    AddToLog(L"GameUpdate paused.");
}

void KablamEngine::UnPauseGameUpdate()
{
    bGameUpdatePaused = false;
    // reset gameloop timer
    tp1 = std::chrono::system_clock::now();
    AddToLog(L"GameUpdate unpaused.");
}

KablamEngine::KeyState KablamEngine::GetKeyState(short key)
{
    return keyArray[key];
}

COORD KablamEngine::GetMousePosition()
{
    return mouseCoords;
}

// stops user being able to resize window
int KablamEngine::SetResizeWindowLock(bool state)
{
    // Get the current window style
    LONG style = GetWindowLong(hConsoleWindow, GWL_STYLE);
    if (style == 0)
        return Error(L"Unable to set window lock; GetWindowLong failed.");

    // Enable resizing
    if (state == false)
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    // Disable resizing
    else
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    AddToLog(L"SetResizeWindowLock set to: " + std::to_wstring(state));

    // Set the updated window style
    SetLastError(0); // Clear any previous errors
    LONG prevStyle = SetWindowLong(hConsoleWindow, GWL_STYLE, style);

    // Handle error if SetWindowLong failed
    if (prevStyle == 0 && GetLastError() != 0)
        return Error(L"Unable to set window lock; SetWindowLong failed.");

    // Success
    return 0;
}

int KablamEngine::SetWindowPosition(int x, int y)
{
    if (!SetWindowPos(hConsoleWindow, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
        return Error(L"Unable to set console window position.");

    AddToLog(L"Console window position set to x: " + std::to_wstring(x) + L", y: " + std::to_wstring(x) + L'.');

    return 0;
}

int KablamEngine::SetFullScreen(bool state)
{
    int newFontWidth;
    int newFontHeight;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (state)
    {
        // Calculate the new font size to fit the screen width
        newFontWidth = screenWidth / GetConsoleWidth();
        newFontHeight = newFontWidth; // Maintain the same aspect ratio by setting height equal to width

        // Adjust the console height accordingly
        int consoleHeight = screenHeight / newFontHeight;

        if (!SetWindowPos(hConsoleWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
            return Error(L"Failed to set fullscreen mode: Unable to set console window position.");
    }
    else
    {
        newFontWidth = nFontWidth;
        newFontHeight = nFontHeight;

        if (!SetWindowPos(hConsoleWindow, NULL, 100, 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
            return Error(L"Failed to set windowed mode: Unable to set console window position.");
    }

    // Initialize the CONSOLE_FONT_INFOEX structure
    CONSOLE_FONT_INFOEX cfi{};
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0; // Use the default font
    cfi.dwFontSize.X = newFontWidth; // Width of each character
    cfi.dwFontSize.Y = newFontHeight; // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Lucida Console");

    // Set the font size
    if (!SetCurrentConsoleFontEx(hNewBuffer, FALSE, &cfi)) {
        CleanUp();
        return Error(L"Failed to set fullscreen mode: Unable to SetCurrentConsoleFontEx.");
    }

    AddToLog(L"Fullscreen mode toggled: Font width: " + std::to_wstring(newFontWidth) + L", font height: " + std::to_wstring(newFontHeight) + L'.');

    return 0;
}

void KablamEngine::FillScreenBuffer(short colour, short glyph)
{
    for (size_t i{ 0 }; i < nScreenWidth * nScreenHeight; i++)
    {
        screen[i].Attributes = colour;
        screen[i].Char.UnicodeChar = glyph;
    }
}


std::wstring KablamEngine::GetFormattedDateTime() {

    // Get current time as time_point
    auto now = std::chrono::system_clock::now();
    // Convert to time_t for compatibility with traditional C time functions
    auto now_c = std::chrono::system_clock::to_time_t(now);
    // Convert to tm struct for formatting
    std::tm now_tm{};
    localtime_s(&now_tm, &now_c);  // Use localtime_s for a safer conversion
    // Buffer to hold the formatted date and time
    wchar_t buffer[60];

    // Format
    std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &now_tm);

    // Convert & return
    return std::wstring(buffer);
}

int KablamEngine::Error(const wchar_t* message, short errorCode)
{
    // message no more than 256 chars
    wchar_t messageBuffer[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, 256, NULL);
    SetConsoleActiveScreenBuffer(hOGBuffer);
    wprintf(L"ERROR(code %i): %s\n\tOS message: %s\n", errorCode, message, messageBuffer);
    AddToLog(message);
    return errorCode;
}

// currently needs to delete screen buffer and close handle of new buffer
int KablamEngine::CleanUp()
{
    delete[] screen;  // Clean up allocated memory
    screen = nullptr; // ptr is now safe to check against nullptr

    delete[] screenBilinear;  // Clean up allocated memory
    screenBilinear = nullptr; // ptr is now safe to check against nullptr

    if (!CloseHandle(hNewBuffer)) {
        return Error(L"Unable to CloseHandle of new buffer when cleaning up.");
    }
    hNewBuffer = INVALID_HANDLE_VALUE;

    // set console mode back to OG
    if (!SetConsoleMode(hConsoleInput, dwPreviousConsoleMode)) {
        return Error(L"Unable to SetConsoleMode back to original when cleaning up.");
    }

    AddToLog(L"CleanUp() performed.");
    return 0; // if successful 
}

void KablamEngine::DrawMessageBox(const std::wstring& message, int messageLines, bool inputMode) {
    
    short textColour{};
    short bgColour{};

    if (inputMode) // select appropriate colour scheme
    {
        textColour = INPUT_MESSAGE_TEXT_COLOUR;
        bgColour = INPUT_MESSAGE_RECT_COLOUR;
    }
    else
    {
        textColour = ALERT_MESSAGE_TEXT_COLOUR;
        bgColour = ALERT_MESSAGE_RECT_COLOUR;
    }

    short colourScheme{ textColour | bgColour << 4 }; // create the text and bg colour appropriate for message type

    int maxMessageLength = nScreenWidth - MESSAGE_BORDER_SIZE * 2;
    std::wstring truncatedMessage = message.substr(0, maxMessageLength);
    int messageLength = std::max(static_cast<int>(truncatedMessage.length()), MIN_MESSAGE_LENGTH);
    int x = nScreenWidth / 2 - messageLength / 2;
    int y = nScreenHeight / 2 - messageLines / 2;

    DrawRectangleEdgeLength(x - MESSAGE_BORDER_SIZE, y - MESSAGE_BORDER_SIZE, messageLength + MESSAGE_BORDER_SIZE * 2, messageLines + MESSAGE_BORDER_SIZE * 2, bgColour, true, PIXEL_SOLID);
    DrawRectangleEdgeLength(x - MESSAGE_BORDER_SIZE, y - MESSAGE_BORDER_SIZE, messageLength + MESSAGE_BORDER_SIZE * 2, messageLines + MESSAGE_BORDER_SIZE * 2, bgColour, false, L'*');
    WriteStringToBuffer(x, y, truncatedMessage, colourScheme);

    if (inputMode) {
        WriteStringToBuffer(x, y + 2, INPUT_BASE_MESSAGE, colourScheme);
    }
    else {
        WriteStringToBuffer(x, y + 2, ALERT_BASE_MESSAGE, colourScheme);
    }
    WriteConsoleOutput(hNewBuffer, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &windowSize);
}

void KablamEngine::DisplayAlertMessage(const std::wstring& message) {
    PauseGameUpdate();
    DrawMessageBox(message, MESSAGE_LINES, false);
    WaitForKeyPress();
    UnPauseGameUpdate();
}

void KablamEngine::DisplayAlertMessageWithInput(const std::wstring& message, std::wstring& userInput) {
    DrawMessageBox(message, MESSAGE_LINES, true);  // Draw the initial message box for input

    std::wstring inputBuffer;
    bool enterPressed = false;
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

    SetConsoleInputMode(~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    INPUT_RECORD inputRecords[128];
    DWORD numRead;

    // calculate position of input line
    int xPos{ nScreenWidth / 2 - (int)message.size() / 2 };
    int yPos{ nScreenHeight / 2 - MESSAGE_LINES / 2 + 2 };

    // update and display the input buffer with untul enter pressed
    while (!enterPressed) {
        if (ReadConsoleInput(hInput, inputRecords, 128, &numRead)) {
            for (DWORD i = 0; i < numRead; ++i) {
                if (inputRecords[i].EventType == KEY_EVENT && inputRecords[i].Event.KeyEvent.bKeyDown) {
                    wchar_t ch = inputRecords[i].Event.KeyEvent.uChar.UnicodeChar;
                    if (ch >= L' ' && ch <= L'~') { // printable characters
                        inputBuffer.push_back(ch);
                    }
                    else if (ch == '\r') {
                        enterPressed = true;
                    }
                    else if (ch == '\b' && !inputBuffer.empty()) {
                        inputBuffer.pop_back();
                    }

                    std::wstring displayBuffer = INPUT_BASE_MESSAGE + inputBuffer + L" "; // L" " to delete previous chars
                    WriteStringToBuffer(xPos, yPos, displayBuffer, INPUT_MESSAGE_COLOUR_SCHEME);
                    WriteConsoleOutput(hNewBuffer, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &windowSize);
                }
            }
        }
    }

    RestoreConsoleInputMode();
    userInput = inputBuffer;
}

void KablamEngine::SetConsoleInputMode(DWORD modeFlags) {
    DWORD prevConsoleMode;
    GetConsoleMode(hConsoleInput, &prevConsoleMode);
    SetConsoleMode(hConsoleInput, prevConsoleMode & modeFlags);
}

void KablamEngine::RestoreConsoleInputMode() {
    DWORD prevConsoleMode;
    GetConsoleMode(hConsoleInput, &prevConsoleMode);
    SetConsoleMode(hConsoleInput, prevConsoleMode);
}

void KablamEngine::WaitForKeyPress()
{
    // Flush the console input buffer to clear out any prior key presses
    FlushConsoleInputBuffer(hConsoleInput);

    // set input mode
    SetConsoleInputMode(~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    INPUT_RECORD ir;
    DWORD numRead;
    while (bGameThreadRunning) {
        // Read the next input event
        if (ReadConsoleInput(hConsoleInput, &ir, 1, &numRead) && numRead > 0) {
            // Check if the event is a key event
            if (ir.EventType == KEY_EVENT) {
                // Check if the key is pressed down, ignoring key up events
                if (ir.Event.KeyEvent.bKeyDown) {
                    break; // Exit the loop on a new key press
                }
            }
        }
        Sleep(100);
    }

    // restore input
    RestoreConsoleInputMode();
}

float KablamEngine::GetAverageFPS(float anotherTimeValue)
{
    float total{ 0.0f };

    // add new value
    elapsedTimeDeque.push_back(anotherTimeValue);

    // If the deque exceeds the maximum size, remove the oldest element
    if (elapsedTimeDeque.size() > MAX_SIZE_DEQUE) {
        elapsedTimeDeque.pop_front();
    }

    // get total
    for (float& time : elapsedTimeDeque)
        total += time;

    // return average FPS
    return 1.0f / (total / elapsedTimeDeque.size());
}

// STATIC MEMBERS

// initialise ptr to instance of class
KablamEngine* KablamEngine::instance{ nullptr };

std::mutex KablamEngine::shutdownMutex;
std::condition_variable KablamEngine::shutdownCv;
std::vector<std::wstring> KablamEngine::eventLog;
std::atomic<bool> KablamEngine::bGameThreadRunning{ false };
std::atomic<bool> KablamEngine::bGracefulExitCompleted{ false };


void KablamEngine::AddToLog(std::wstring message)
{
    eventLog.push_back(message);
}

// handle closing console/log-out/shutdown behaviour
BOOL WINAPI KablamEngine::ConsoleControlHandler(DWORD dwCtrlType)
{
    AddToLog(L"ConsoleControlHandler() activated.");

    switch (dwCtrlType) {    // Clean up resources, save state, etc.

    case CTRL_CLOSE_EVENT: {
        KablamEngine::AddToLog(L"CTRL_CLOSE_EVENT occured (window closed by user).");

        // stop gameloop and wait for it to finish it's tidying up...
        KablamEngine::bGameThreadRunning = false;

        std::unique_lock<std::mutex> ul(KablamEngine::shutdownMutex);
        KablamEngine::shutdownCv.wait(ul, [] { return KablamEngine::bGracefulExitCompleted.load(); });

        return TRUE;
    }

    case CTRL_LOGOFF_EVENT: {
        // stop gameloop and wait for it to finish it's tidying up...
        KablamEngine::bGameThreadRunning = false;

        std::unique_lock<std::mutex> ul(KablamEngine::shutdownMutex);
        KablamEngine::shutdownCv.wait(ul, [] { return KablamEngine::bGracefulExitCompleted.load(); });

        return TRUE;
    }

    case CTRL_SHUTDOWN_EVENT: {
        // stop gameloop and wait for it to finish it's tidying up...
        KablamEngine::bGameThreadRunning = false;

        std::unique_lock<std::mutex> ul(KablamEngine::shutdownMutex);
        KablamEngine::shutdownCv.wait(ul, [] { return KablamEngine::bGracefulExitCompleted.load(); });

        return TRUE;
    }

    default:
        KablamEngine::AddToLog(L"Unknown EVENT called the ConsoleControlHandler() and it was not handled. Check outputLog.txt.");

        // stop gameloop and wait for it to finish it's tidying up...
        KablamEngine::bGameThreadRunning = false;

        std::unique_lock<std::mutex> ul(KablamEngine::shutdownMutex);
        KablamEngine::shutdownCv.wait(ul, [] { return KablamEngine::bGracefulExitCompleted.load(); });

        return FALSE; // handler didn't handle the event
    }
}

bool KablamEngine::OutputLogFile()
{
    // Open the file for writing
    FILE* file = nullptr;
    errno_t err = _wfopen_s(&file, L"events_log.txt", L"wb"); // Use wide string literal for filename
    if (err != 0 || file == nullptr)
        return false;

    // Write a Byte Order Mark(BOM) to let text editor know 16-bit wchar
    wchar_t bom = 0xFEFF;
    fwrite(&bom, sizeof(wchar_t), 1, file);

    // write error log to events_log.txt
    for (const auto& str : eventLog) {
        // Write the wide string to the file
        fwrite(str.c_str(), sizeof(wchar_t), str.size(), file);

        // Write a newline character as wide char
        const wchar_t newline[] = L"\n";
        fwrite(newline, sizeof(wchar_t), 1, file);
    }

    // flush
    fflush(file);
    // Close the file
    fclose(file);

    return true;
}






