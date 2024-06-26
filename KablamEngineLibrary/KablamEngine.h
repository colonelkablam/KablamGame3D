
/*      -   The Kablam Engine    -

    Nick Harding 10/2/2024 nickhfharding@gmail.com
    A slow and underpowered C++ Engine to display Graphics in the Windows console

    Plan to switch this to OpenGL when I understand the fundamentals

    Born from an odd desire too see what I could do with the Windows console when learning to code (as
    using it so much on various courses). I also wanted to try and strip away some of the abstraction around
    programming graphics (Unreal/Unity etc) to further my understanding of the fundamentals.

    Many solutions have been found on-line and inspired further by various people on
    youTube/reddit/discord/stackoverflow who seemed to have felt the same or encountered similar challenges.
    Weird bunch.

    There were many resources used to make this project,
    of particular use/reuse were (in no particular order):

    1.https://github.com/3DSage
    2.https://www.github.com/onelonecoder
    3.http://thecodingtrain.com/

*/

// wchar used in screenbuffer
#ifndef UNICODE
#error Enable UNICODE for your compiler - NH.
#endif

#pragma once

#include <Windows.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <deque>

#include "Texture.h"

class KablamEngine
{
    // member attributes

    // singleton pattern where only one instance of the class is expected/allowed
    // need access to member elements when handling closing of console
protected:

    // STATIC variables
    // constants
    constexpr static short INPUT_BUFFER_SIZE{ 32 };

    // pop-up message constants
    const short ALERT_MESSAGE_TEXT_COLOUR = FG_WHITE;
    const short ALERT_MESSAGE_RECT_COLOUR = FG_DARK_MAGENTA;
    const short ALERT_MESSAGE_COLOUR_SCHEME = ALERT_MESSAGE_TEXT_COLOUR | FG_DARK_MAGENTA << 4;
    const std::wstring ALERT_BASE_MESSAGE = L"Press any key to continue...";

    const short INPUT_MESSAGE_TEXT_COLOUR = FG_WHITE;
    const short INPUT_MESSAGE_RECT_COLOUR = FG_DARK_BLUE;
    const short INPUT_MESSAGE_COLOUR_SCHEME = INPUT_MESSAGE_TEXT_COLOUR | FG_DARK_BLUE << 4;
    const std::wstring INPUT_BASE_MESSAGE = L"Input > ";

    const int MESSAGE_BORDER_SIZE = 3;
    const int MESSAGE_LINES = 3;
    const int MIN_MESSAGE_LENGTH = 28; // accommodate base message/input area

    // pointer to instance
    static KablamEngine* instance;

    // static variables to handle shutting threads down in correct order
    static std::mutex shutdownMutex;
    static std::condition_variable shutdownCv;
    static std::vector<std::wstring> eventLog;
    static std::atomic<bool> bGameThreadRunning;
    static std::atomic<bool> bGracefulExitCompleted;

    // instance attributes
    short nScreenWidth;  // Console screen width
    short nScreenHeight;  // Console screen height
    short nFontWidth;      // Font width in pixels
    short nFontHeight;    // Font height in pixels

    // timing points
    std::chrono::system_clock::time_point tp1;
    std::chrono::system_clock::time_point tp2;
    std::deque<float> elapsedTimeDeque;
    const size_t MAX_SIZE_DEQUE = 50; // Maximum size of the deque list

    std::wstring sConsoleTitle;
    std::wstring sCurrentFileName;
    std::wstring sCurrentFilePath;

    CHAR_INFO* screen;      // pointer to screen buffer
    CHAR_INFO* screenBilinear;      // if applying filtering

    bool bBilinearApplied;

    SMALL_RECT windowSize;  // console window physical size
    int nScreenArrayLength;

    // grab handle to the console window (Windows manages this);
    // hiding mouse / system stuff etc...
    HWND hConsoleWindow;
    DWORD dwPreviousConsoleMode;

    // grab OG buffer - will revert to on clean-up
    HANDLE hOGBuffer;
    // for new console buffer
    HANDLE hNewBuffer;
    // grab input buffer
    HANDLE hConsoleInput;

    // storage of input events

    // KEYS
    // struct for each key
    struct KeyState {
        bool bPressed;
        bool bReleased;
        bool bHeld;
    };

    KeyState keyArray[256];
    short newKeyStateArray[256];
    short oldKeyStateArray[256];

    // CONSOLE / WINDOW
    bool bConsoleFocus;
    bool bFocusPause;
    bool bGameUpdatePaused;
    bool bFullScreen;
    COORD mouseCoords;

    // sampling
    struct Colour5Sample {
        short c = 0;
        short c00 = 0;
        short c01 = 0;
        short c10 = 0;
        short c11 = 0;
    };

public:

    // constructor 
    KablamEngine();

    // destructor
    ~KablamEngine();

    // member methods
    int BuildConsole(int screenWidth, int screenHeight, int pixelWidth, int pixelHeight, int winPosX, int winPosY);

    void Start(); // starts engine


private:
    int GameThread();

protected:
    // User MUST OVERRIDE THESE!!
    virtual bool OnGameCreate() = 0;       // pure virtual
    virtual bool OnGameUpdate(float fElapsedTime) = 0;  // pure virtual

    // Optional for clean up 
    //virtual bool OnGameDestroy() { return true; }
public:
    int GetConsoleWidth();
    int GetConsoleHeight();
    void WriteStringToBuffer(int x, int y, const std::wstring& string, short colour = FG_WHITE);
    void DrawPoint(int x, int y, short colour = FG_WHITE, short glyph = PIXEL_SOLID);
    void DrawPoint(int x, int y, const CHAR_INFO& pixel);
    void DrawBlock(int x, int y, int size = 1, short colour = FG_WHITE, short glyph = PIXEL_SOLID);
    void DrawLine(int x0, int y0, int x1, int y1, short colour = FG_WHITE, short glyph = PIXEL_SOLID, int lineThickness = 1);
    void DrawRectangleCoords(int x0, int y0, int x1, int y1, short colour = FG_WHITE, bool filled = false, short glyph = PIXEL_SOLID, int lineWidth = 1);
    void DrawRectangleEdgeLength(int x, int y, int width, int height, short colour = FG_WHITE, bool filled = false, short glyph = PIXEL_SOLID, int lineWidth = 1);
    void DrawCircle(int x, int y, int radius, short colour = FG_WHITE, short glyph = PIXEL_SOLID, bool filled = false);
    int DrawTextureToScreen(const Texture& texture, int x, int y, float scale = 1, bool showEmptyPix = false, bool greyedOut = false);
    int DrawPartialTextureToScreen(const Texture& texture, int xScreen, int yScreen, float scale = 1);
    int DrawPartialTextureToScreen(const Texture& texture, COORD screenPos, float scale);
    int DrawSectionOfTextureToScreen(const Texture& texture, int xScreen, int yScreen, int x0, int y0, int x1, int y1, float scale = 1, bool showEmptyPix = false);
    int DrawSectionOfTextureToScreen(const Texture& texture, COORD screenPos, COORD topLeft, COORD bottomRight, float scale, bool showEmptyPix);


private:
    void SampleSurroundingTexels(int x, int y, Colour5Sample& sample);
    void TwoMainColourCounts(const std::map<short, int>& colourMap, std::pair<short, int>& firstColour, std::pair<short, int>& secondColour);

protected:
    void ApplyDominantColorGlyphBlend();
    void UpdateInputStates();
    void SetConsoleFocusPause(bool state);
    bool GetConsoleFocus();
    void PauseGameUpdate();
    void UnPauseGameUpdate();
    KeyState GetKeyState(short key);
    int SetResizeWindowLock(bool state);
    int SetWindowPosition(int x, int y);
    int SetFullScreen(bool state);
    void FillScreenBuffer(short colour = FG_BLACK, short glyph = PIXEL_SOLID);
    std::wstring GetFormattedDateTime();
    int Error(const wchar_t* msg, short errorCode = 1);
    int CleanUp();

    void DrawMessageBox(const std::wstring& message, int messageLines, bool inputMode);
    void DisplayAlertMessage(const std::wstring& message);
    void DisplayAlertMessageWithInput(const std::wstring& message, std::wstring& userInput);
    void WaitForKeyPress();
    void SetConsoleInputMode(DWORD modeFlags);
    void RestoreConsoleInputMode();


    float GetAverageFPS(float elapsedTimeValue);

public:
    COORD GetMousePosition();
    static void AddToLog(std::wstring message);

private:
    static BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType);
    static bool OutputLogFile();

};