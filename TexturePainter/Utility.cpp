#include <iostream>
#include <sstream> // For std::wstringstream
#include <cwctype> // For std::towlower & std::iswalpha
#include <limits> // for max()
#include <windows.h>



// Undefine the macros
#undef max
#undef min



#include "Utility.h"

void SetConsoleWindowSize(int width, int height, int extraBufferLines)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set the screen buffer size - needs to be done first
    COORD bufferSize = { width, height + extraBufferLines };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // Set the console window size
    SMALL_RECT windowSize = { 0, 0, width - 1, height - 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void SetConsoleFont(int fontSize, const wchar_t* fontName)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
    wcscpy_s(cfi.FaceName, fontName); // Set font
    cfi.dwFontSize.Y = fontSize; // font height
    cfi.dwFontSize.X = 0; // system choose the font width
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
}

void CenterConsoleWindow()
{
    HWND consoleWindow = GetConsoleWindow();
    RECT rect;
    GetWindowRect(consoleWindow, &rect);
    int consoleWidth = rect.right - rect.left;
    int consoleHeight = rect.bottom - rect.top;

    // Get the screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the new position to center the console window
    int x = (screenWidth - consoleWidth) / 2;
    int y = (screenHeight - consoleHeight) / 2;

    // Set the new position
    MoveWindow(consoleWindow, x, y, consoleWidth, consoleHeight, TRUE);
}

bool GetValidFileName(const std::wstring& prompt, std::wstring& userInput, const std::wstring& illegalChars, const int maxLength) {

    std::wcout << prompt;
    std::getline(std::wcin, userInput);

    if (userInput.size() > maxLength)
    {
        std::wcout << L"\nFile name must be less than " << maxLength << " characters in length...\n\n";
        return false; // No need to check further characters
    }

    // Check for illegal characters
    bool hasIllegalChars = false;
    for (wchar_t ch : userInput) {
        if (ch < 32 || illegalChars.find(ch) != std::wstring::npos) {
            hasIllegalChars = true;
            std::wcout << L"\nInput must not contain illegal characters (" << illegalChars << ")...\n\n";
            return false; // No need to check further characters
        }
    }
    return true; // Filename is valid
}

bool IsValidFileName(const std::wstring& fileName, const std::wstring& illegalChars, const int maxLength) {

    // check input length
    if (fileName.size() > maxLength)
    {
        return false; // No need to check further characters
    }

    // Check for illegal characters
    for (wchar_t ch : fileName) {
        if (ch < 32 || illegalChars.find(ch) != std::wstring::npos) {
            return false; // Illegal character found, return immediately
        }
    }
    return true; // Filename is valid
}

bool FileExistInDir(const std::vector <std::wstring>& fileList, const std::wstring& fileName) {
    for (const auto& file : fileList) {
        if (file == fileName) {
            return true; // The file was found in the list
        }
    }
    return false; // The file was not found in the list
}

// makes a list of files in a given subdirectory
std::vector<std::wstring> GetFileList(const std::wstring& folderName, const std::wstring& extensionName = L'\0')
{
    // create an empty list vector
    std::vector<std::wstring> fileList;

    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // Construct the search pattern, e.g., "C:\\path\\*.spr"
    std::wstring searchPattern = std::wstring(folderName) + L"*" + extensionName;

    // Start the file search
    hFind = FindFirstFile(searchPattern.c_str(), &findFileData);

    // return an empty list if error
    if (hFind == INVALID_HANDLE_VALUE) {
        return fileList; // list will be empty
    }
    else {
        do {
            // push name onto list
            fileList.push_back(findFileData.cFileName);
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    return fileList;
}

void PrintFileList(const std::wstring& header, const std::vector<std::wstring>& fileList)
{
    std::wcout << L"\n" << header << L"\n\n";

    if (fileList.size() != 0)
    {
        int count{ 1 };
        for (const auto& file : fileList)
        {
            std::wcout << count << L". " << file << std::endl;
            count++;
        }
    }
    else
        std::wcout << L"No files to display...\n";

    std::wcout << std::endl;
}

void GetDimensionInput(const std::wstring& prompt, int& userInput, int min, int max) {
    while (true) {
        std::wcout << prompt;
        if (std::wcin >> userInput) {
            // Clear the input buffer to remove the leftover newline character
            std::wcin.ignore(10000, L'\n');

            if (userInput >= min && userInput <= max) {
                // Valid input
                break;
            }
            else {
                std::wcout << L"Please enter a value between " << min << " and " << max << ".\n";
            }
        }
        else {
            // Invalid input, clear the error state and ignore the rest of the input
            std::wcin.clear();
            std::wcin.ignore(10000, L'\n');
            std::wcout << L"Invalid input, please enter an integer.\n";
        }
    }
}

bool GetYesNoInput(const std::wstring& prompt) {

    wchar_t userInput{ L' ' };
    while (true) {
        std::wcout << prompt;
        if (std::wcin >> userInput) {
            userInput = towlower(userInput);

            // Clear the input buffer to remove the leftover newline character
            std::wcin.ignore(10000, L'\n');

            if (userInput == L'y')
                return true;
            else if (userInput == L'n')
                return false;
            else {
                std::wcout << L"Please enter 'y' for yes or 'n' for no.\n";
            }
        }
        else {
            std::wcin.clear();
            std::wcin.ignore(10000, L'\n');
            std::wcout << L"Invalid input, please enter 'y' for yes or 'n' for no.\n";
        }
    }
}

bool LetterListener(const std::wstring& userInput, const wchar_t checkLetter) {
    // Check if the userInput length is 1 and if the first character, converted to lowercase, matches 'q'
    if (userInput.length() == 1 && std::towlower(userInput.at(0)) == std::towlower(checkLetter)) {
        return true;
    }
    return false;
}

int WStringToInteger(const std::wstring& input)
{
    int number{ 0 };
    std::wstringstream wss(input);

    // Attempt to convert std::wstring to int
    wss >> number;

    // Check if the conversion was successful and the whole string was consumed
    if (!wss.fail() && wss.eof()) {
        return number;
    }

    // Handle potential issues with the conversion process
    if (wss.fail()) {
        // Check if the input string was out of range for an int
        long long tempNumber{ 0 };
        std::wistringstream wiss(input);
        wiss >> tempNumber;

        if (tempNumber > std::numeric_limits<int>::max() || tempNumber < std::numeric_limits<int>::min()) {
            return -1; // Out of range
        }
    }

    // If we reached here, the conversion was not successful
    return -1;
}

bool CheckFolderExists(const std::wstring& folderPath) {
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

bool CreateFolder(const std::wstring& folderPath) {
    if (!CreateDirectory(folderPath.c_str(), NULL)) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            return true; // The folder already exists
        }
        std::wcerr << L"Failed to create directory: " << folderPath << std::endl;
        return false; // Failed to create the directory
    }

    return true; // Successfully created the directory
}

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

// Defines the 'pixels' for each character in a chunky font
std::unordered_map<char, std::vector<std::string>> charMap = {
    {'A', {"  #  ", " # # ", "#####", "#   #", "#   #"}},
    {'B', {"#### ", "#   #", "#####", "#   #", "#### "}},
    {'C', {" ### ", "#    ", "#    ", "#    ", " ### "}},
    {'D', {"#### ", "#   #", "#   #", "#   #", "#### "}},
    {'E', {"#####", "#    ", "#####", "#    ", "#####"}},
    {'F', {"#####", "#    ", "#####", "#    ", "#    "}},
    {'G', {" ### ", "#    ", "# ###", "#   #", " ### "}},
    {'H', {"#   #", "#   #", "#####", "#   #", "#   #"}},
    {'I', {"#####", "  #  ", "  #  ", "  #  ", "#####"}},
    {'J', {"    #", "    #", "    #", "#   #", " ### "}},
    {'K', {"#   #", "#  # ", "###  ", "#  # ", "#   #"}},
    {'L', {"#    ", "#    ", "#    ", "#    ", "#####"}},
    {'M', {"#   #", "## ##", "# # #", "#   #", "#   #"}},
    {'N', {"#   #", "##  #", "# # #", "#  ##", "#   #"}},
    {'O', {" ### ", "#   #", "#   #", "#   #", " ### "}},
    {'P', {"#### ", "#   #", "#### ", "#    ", "#    "}},
    {'Q', {" ### ", "#   #", "#   #", " ### ", "    #"}},
    {'R', {"#### ", "#   #", "#### ", "#  # ", "#   #"}},
    {'S', {" ####", "#    ", " ### ", "    #", "#### "}},
    {'T', {"#####", "  #  ", "  #  ", "  #  ", "  #  "}},
    {'U', {"#   #", "#   #", "#   #", "#   #", " ### "}},
    {'V', {"#   #", "#   #", "#   #", " # # ", "  #  "}},
    {'W', {"#   #", "#   #", "# # #", "## ##", "#   #"}},
    {'X', {"#   #", " # # ", "  #  ", " # # ", "#   #"}},
    {'Y', {"#   #", "#   #", " ### ", "  #  ", "  #  "}},
    {'Z', {"#####", "   # ", "  #  ", " #   ", "#####"}},
    {' ', {"     ", "     ", "     ", "     ", "     "}}
};

void printChunkyString(const std::string& text) {
    // Get the console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get current console attributes
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD saved_attributes = consoleInfo.wAttributes;  // Save current attributes

    for (int row = 0; row < 5; ++row) { // Assuming 5 rows per character
        for (char ch : text) {
            ch = std::toupper(ch); // Convert character to uppercase
            printCharacterRow(hConsole, charMap, ch, row, saved_attributes);
        }
        std::cout << std::endl; // Newline after each row
    }
};

void printCharacterRow(HANDLE hConsole, const std::unordered_map<char, std::vector<std::string>>& charMap, char ch, int row, WORD originalAttributes) {
    const int purple = FOREGROUND_BLUE | FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_RED;  // Purple color attribute

    // Check if the character is in the map
    if (charMap.find(ch) != charMap.end()) {
        for (char pixel : charMap.at(ch)[row]) {
            if (pixel != ' ') {
                SetConsoleTextAttribute(hConsole, purple);  // Set text color to purple
                std::cout << pixel;
                SetConsoleTextAttribute(hConsole, originalAttributes);  // Reset to default color
            }
            else {
                std::cout << ' ';
            }
        }
    }
    else {
        SetConsoleTextAttribute(hConsole, purple);  // Set text color to purple for unknown characters
        std::cout << "????";
        SetConsoleTextAttribute(hConsole, originalAttributes);  // Reset to default color
    }
    std::cout << ' ';  // Add a space after each character block
};

