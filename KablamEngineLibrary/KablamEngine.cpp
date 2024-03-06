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
    dwPreviousConsoleMode{ 0 }, screen{ nullptr }, screenBilinear{ nullptr }, nScreenArrayLength{ 0 }, windowSize { 0, 0, 1, 1 }, mouseCoords{ 0,0 },
    sConsoleTitle{ L"no_name_given" }, bConsoleFocus{ true }, bGameUpdatePaused{ false }, bFocusPause{ false }, bFullScreen{ false } {

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

int KablamEngine::BuildConsole(int screenWidth, int screenHeight, int fontWidth, int fontHeight)
{
    // set the users console size
    nScreenWidth = screenWidth;  // Console screen width
    nScreenHeight = screenHeight;  // Console screen height
    nFontWidth = fontWidth;      // Font width in pixels
    nFontHeight = fontHeight;    // Font height in pixels

    // right... this is painful - let's go!

    if (hOGBuffer == INVALID_HANDLE_VALUE)
        return Error(L"Bad STD_OUTPUT_HANDLE.");

    if (hConsoleInput == INVALID_HANDLE_VALUE)
        return Error(L"Bad INVALID_HANDLE_VALUE.");

    // Create a new screen buffer
    nScreenArrayLength = nScreenWidth * nScreenHeight;
    screen = new CHAR_INFO[nScreenArrayLength];
    // clear memory for screen buffer
    memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

    // create new console buffer
    hNewBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (hNewBuffer == INVALID_HANDLE_VALUE) {
        delete[] screen;  // Clean up allocated memory
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
    CONSOLE_FONT_INFOEX cfi;
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
            bGameUpdatePaused = false;
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
            swprintf_s(title, 256, L"%s - FPS: %3.2f", sConsoleTitle.c_str(), 1.0f / fElapsedTime);
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
}

// Bresenham's line algorithm
void KablamEngine::DrawLine(int x0, int y0, int x1, int y1, short colour, short glyph)
{
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

void KablamEngine::DrawSquare(int x, int y, int sideLength, short colour, short glyph, int lineWidth, bool filled)
{
    // Top side
    DrawLine(x, y, x + sideLength, y, colour, glyph);
    // Right side
    DrawLine(x + sideLength, y, x + sideLength, y + sideLength, colour, glyph);
    // Bottom side
    DrawLine(x, y + sideLength, x + sideLength, y + sideLength, colour, glyph);
    // Left side
    DrawLine(x, y, x, y + sideLength, colour, glyph);
}

// top left and bottom right coords
void KablamEngine::DrawRectangleCoords(int x0, int y0, int x1, int y1, short colour, bool filled, short glyph, int lineWidth)
{
    if (filled)
    {
        for (int i{ 0 }; i <= y1 - y0; i++)
            DrawLine(x0, y0 + i, x1, y0 + i, colour, glyph);
    }
    else
    {
        // Top side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x0, y0 + i, x1, y0 + i, colour, glyph);
       // Right side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x1 - i, y0, x1 - i, y1, colour, glyph);
        // Bottom side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x0, y1 - i, x1, y1 - i, colour, glyph);
        // Left side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x0 + i, y0, x0 + i, y1, colour, glyph);
    }
}

// top left coords and width and height
void KablamEngine::DrawRectangleEdgeLength(int x, int y, int width, int height, short colour, bool filled, short glyph, int lineWidth)
{
    if (filled)
    {
        for (int i{ 0 }; i < height; i++)
            DrawLine(x, y + i, x + width - 1, y + i, colour, glyph);
    }
    else
    {
        width -= 1;
        height -= 1;
        // Top side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x, y + i, x + width, y + i, colour, glyph);
        // Right side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x + width - i, y, x + width - i, y + height, colour, glyph);
        // Bottom side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x, y + height - i, x + width, y + height - i, colour, glyph);
        // Left side
        for (int i{ 0 }; i < lineWidth; i++)
            DrawLine(x + i, y, x + i, y + height, colour, glyph);
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


int KablamEngine::DrawTextureToScreen(const Texture* texture, int xScreen, int yScreen, float scale)
{
    // need to handle scale...TBC

    for (int x{ 0 }; x < texture->GetWidth(); x++)
    {
        for (int y{ 0 }; y < texture->GetHeight(); y++)
        {
            DrawPoint(xScreen + x, yScreen + y, texture->GetColour(x, y), texture->GetGlyph(x, y));
        }
    }
    return 0;
}


void KablamEngine::CountColorRatios(Colour4Sample& sample, std::map<short, int>& colourMap)
{
// Increment the count for each color
    colourMap[sample.c00]++;
    colourMap[sample.c01]++;
    colourMap[sample.c10]++;
    colourMap[sample.c11]++;
}


void KablamEngine::CalculateColorRatios(const std::map<short, int>& colorCounts, std::map<short, float>& ratioMap) {
    for (const auto& pair : colorCounts) {
        ratioMap[pair.first] = pair.second / 4.0f;  // Dividing by 4, as there are 4 texels
    }
}

void KablamEngine::ApplyBilinearProcess()
{
    Colour4Sample colourSample;
    std::map<short, int> colourMap;
    std::map<short, float> ratioMap;

    for (int x{ 0 }; x < nScreenWidth; x++)
    {
        for (int y{ 0 }; y < nScreenHeight; y++)
        {
            // Fetch the four texels nearest to (x, y)
            colourSample.c00 = screen[y * nScreenWidth + x].Attributes;
            colourSample.c10 = screen[y * nScreenWidth + std::min(x + 1, nScreenWidth - 1)].Attributes;
            colourSample.c01 = screen[std::min(y + 1, nScreenHeight - 1) * nScreenWidth + x].Attributes;
            colourSample.c11 = screen[std::min(y + 1, nScreenHeight - 1) * nScreenWidth + std::min(x + 1, nScreenWidth - 1)].Attributes;

            CountColorRatios(colourSample, colourMap);
            CalculateColorRatios(colourMap, ratioMap);

            // Sort colors by their ratio to find the most and second most dominant colors
            std::vector<std::pair<short, float>> sortedRatios(ratioMap.begin(), ratioMap.end());
            std::sort(sortedRatios.begin(), sortedRatios.end(), [](const auto& a, const auto& b) {
                return a.second > b.second; // Descending order
                });

            // Select the two most dominant colors for foreground and background
            short fgColour = sortedRatios.size() > 0 ? sortedRatios[0].first : FG_WHITE; // Most dominant
            short bgColour = sortedRatios.size() > 1 ? sortedRatios[1].first : BG_BLACK; // Second most dominant

            // Choose a glyph based on the ratio of the most dominant color
            short glyph;
            float ratio = sortedRatios[0].second; // Ratio of the most dominant color

            std::array<short, 4> glyphs = { PIXEL_QUARTER, PIXEL_HALF, PIXEL_THREEQUARTERS, PIXEL_SOLID };

            int ratioIndex = std::min(static_cast<int>(ratio * 4), 3); // Assuming ratio is normalized to [0, 1]
            glyph = glyphs[ratioIndex];

            screenBilinear[y * nScreenHeight + x].Attributes = fgColour | (bgColour << 4);
            screenBilinear[y * nScreenHeight + x].Char.UnicodeChar = glyph;
        }
    }

    screen = screenBilinear;
}

KablamEngine::Colour4Sample KablamEngine::Get4ColourSample(int x, int y)
{

}

void KablamEngine::Blend4ColourSample(const Colour4Sample& sample, Pixel& pix)
{

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
        keyState& key = keyArray[i];
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

    if (state)
    {
        // Get the screen width in pixels
        newFontWidth = GetSystemMetrics(SM_CXSCREEN) / GetConsoleWidth();
        // Get the screen height in pixels
        newFontHeight = GetSystemMetrics(SM_CYSCREEN) / GetConsoleHeight();
        if (!SetWindowPos(hConsoleWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
            return Error(L"Failed to set fullscreen mode: Unable to set console window position.");
    }
    else
    {
        newFontWidth = nFontWidth;
        newFontHeight = nFontWidth;
        if (!SetWindowPos(hConsoleWindow, NULL, 100, 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
            return Error(L"Failed to set fullscreen mode: Unable to set console window position.");
    }


    // Initialize the CONSOLE_FONT_INFOEX structure
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0; // Use the default font
    cfi.dwFontSize.X = newFontWidth; // Width of each character
    cfi.dwFontSize.Y = newFontHeight; // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    wcscpy_s(cfi.FaceName, L"Lucida Console");
    // set size
    if (!SetCurrentConsoleFontEx(hNewBuffer, FALSE, &cfi)) {
        CleanUp();
        return Error(L"Failed to set fullscreen mode: Unable to SetCurrentConsoleFontEx.");
    }

    AddToLog(L"Fullscreen mode toggled: Font width: " + std::to_wstring(newFontWidth) + L", font height: " + std::to_wstring(newFontHeight) + L'.');

    return 0;
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
    wchar_t buffer[100];

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

void KablamEngine::DisplayAlertMessage(const std::wstring& message)
{
    int border = 3;
    int maxMessageLength = nScreenWidth - border * 2; // Maximum message length considering the border
    std::wstring truncatedMessage = message.substr(0, maxMessageLength); // Truncate message accordingly
    int messageLength = std::max(static_cast<int>(truncatedMessage.length()), 28); // 28 is "Press any key" message length
    int messageLines = 3; // Number of lines for the message
    // Center the message
    int x = nScreenWidth / 2 - messageLength / 2;
    int y = nScreenHeight / 2 - messageLines / 2;

    // Draw background of message box
    DrawRectangleEdgeLength(x - border, y - border, messageLength + border * 2, messageLines + border * 2, FG_DARK_MAGENTA, true, PIXEL_SOLID);
    // Draw border
    DrawRectangleEdgeLength(x - border, y - border, messageLength + border * 2, messageLines + border * 2, FG_WHITE | FG_DARK_MAGENTA, false, L'*');

    // Write the truncated message and the "Press any key" instruction
    WriteStringToBuffer(x, y, truncatedMessage, FG_WHITE | BG_DARK_MAGENTA);
    WriteStringToBuffer(x, y + 2, L"Press any key to continue...", FG_WHITE | BG_DARK_MAGENTA);

    // Update Console Buffer with screen buffer
    WriteConsoleOutput(hNewBuffer, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &windowSize);

    PauseGameUpdate();
    WaitForKeyPress();
    UnPauseGameUpdate();
}

void KablamEngine::WaitForKeyPress()
{
    // Flush the console input buffer to clear out any prior key presses
    FlushConsoleInputBuffer(hConsoleInput);

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






