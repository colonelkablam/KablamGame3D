// KablamEngine.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include "pch.h"
#include "KablamEngine.h"

// constructor

KablamEngine::KablamEngine()
    :nScreenWidth{ 30 }, nScreenHeight{ 10 }, nFontWidth{ 5 }, nFontHeight{ 10 }, hConsoleWindow{ GetConsoleWindow() },
    hOGBuffer{ GetStdHandle(STD_OUTPUT_HANDLE) }, hConsoleInput{ GetStdHandle(STD_INPUT_HANDLE) }, hNewBuffer{ INVALID_HANDLE_VALUE },
    dwPreviousConsoleMode{ 0 }, screen{ nullptr }, windowSize{ 0, 0, 1, 1 }, mouseCoords{ 0,0 }, sConsoleTitle{ L"no_name_given" },
    bConsoleFocus{ true }, bMouseShowing{ true } {

    // initialise storage of input events - key and mouse
    // use standard library fill
    std::fill(std::begin(newKeyStateArray), std::end(newKeyStateArray), false);
    std::fill(std::begin(oldKeyStateArray), std::end(oldKeyStateArray), false);

    for (keyState& key : keyArray) {
        key.bPressed = false;
        key.bHeld = false;
        key.bReleased = false;
    }
    
    // grab the ptr to this instance into static instance (needed for handling closing console)
    instance = this;
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
    screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
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

    // set how to handle a close console event
    if (!SetConsoleCtrlHandler(KablamEngine::ConsoleControlHandler, TRUE))
        return Error(L"Failed to SetConsoleCtrlHandler");


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
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();


    /////       MAIN GAME LOOP      /////

    while (bGameThreadRunning == true)
    {

        // Start parallel tasks for handling input
        std::thread UpdateMouseThread(&KablamEngine::UpdateInputStates, this);

        // Handle Timing
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        // Wait for parallel tasks to complete
        UpdateMouseThread.join();

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
    /////   END OF MAIN GAME LOOP   /////

    AddToLog(L"Exiting GameThread().");

    return 0;
}

int KablamEngine::GetScreenWidth()
{
    return nScreenWidth;
}

int KablamEngine::GetScreenHeight()
{
    return nScreenHeight;
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

void KablamEngine::DrawSquare(int x, int y, int sideLength, short colour, short glyph)
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

void KablamEngine::DrawRectangle(int x0, int y0, int x1, int y1, short colour, short glyph)
{
    // Top side
    DrawLine(x0, y0, x1, y0, colour, glyph);
    // Right side
    DrawLine(x1, y0, x1, y1, colour, glyph);
    // Bottom side
    DrawLine(x0, y1, x1, y1, colour, glyph);
    // Left side
    DrawLine(x0, y0, x0, y1, colour, glyph);
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
                bConsoleFocus = inputBuffer[i].Event.FocusEvent.bSetFocus;
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

KablamEngine::keyState KablamEngine::GetKeyState(short key)
{
    return keyArray[key];
}

COORD KablamEngine::GetMousePosition()
{
    return mouseCoords;
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

// stops user being able to resize window (currently will cause issues)
void KablamEngine::SetResizeWinowLock(bool state)
{
    // Get the current window style
    LONG style = GetWindowLong(hConsoleWindow, GWL_STYLE);

    // Add WS_THICKFRAME and WS_MAXIMIZEBOX to the style to enable resizing and maximizing
    if (state == false)
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    else
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    // Remove WS_THICKFRAME and WS_MAXIMIZEBOX from the style to disable resizing and maximizing

    // Set the updated window style
    SetWindowLong(hConsoleWindow, GWL_STYLE, style);
}

void KablamEngine::ToggleMouse()
{
    bMouseShowing = !bMouseShowing;
    ShowCursor(bMouseShowing);
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
        KablamEngine::AddToLog(L"CTRL_LOGOFF_EVENT occured.");
        OutputLogFile();
        // stop gameloop and wait for it to finish it's tidying up...
        KablamEngine::bGameThreadRunning = false;
        std::unique_lock<std::mutex> ul(KablamEngine::shutdownMutex);
        KablamEngine::shutdownCv.wait(ul, [] { return KablamEngine::bGracefulExitCompleted.load(); });

        return TRUE;
    }

    case CTRL_SHUTDOWN_EVENT: {
        KablamEngine::AddToLog(L"CTRL_SHUTDOWN_EVENT occured.");
        OutputLogFile();

        // stop gameloop and wait for it to finish it's tidying up...
        KablamEngine::bGameThreadRunning = false;
        std::unique_lock<std::mutex> ul(KablamEngine::shutdownMutex);
        KablamEngine::shutdownCv.wait(ul, [] { return KablamEngine::bGracefulExitCompleted.load(); });

        return TRUE;
    }

    default:
        KablamEngine::AddToLog(L"Unknown EVENT called the ConsoleControlHandler(), not handled. Check outputLog.txt.");

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






