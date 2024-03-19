#include <windows.h>
#include <iostream>
#include <sstream> // For std::wstringstream

#include "Utility.h"

bool GetValidFileName(const std::wstring& prompt, std::wstring& userInput) {
    // illegal characters
    const std::wstring illegalChars{ L".*<>:\"/\\|?" };

    while (true)
    {
        std::wcout << prompt;
        std::getline(std::wcin, userInput);

        if (userInput == L"Q" || userInput == L"q") {
            return false; // User chose to quit
        }

        // Check for illegal characters
        bool hasIllegalChars = false;
        for (wchar_t ch : userInput) {
            if (ch < 32 || illegalChars.find(ch) != std::wstring::npos) {
                hasIllegalChars = true;
                std::wcout << L"\nInput must not contain illegal characters...\n";
                break; // No need to check further characters
            }
        }

        if (hasIllegalChars) {
            std::wcout << L"Press Enter to continue...\n";
            std::wcin.get();
            continue; // Prompt the user again
        }
        break; // Valid input
    }

    return true; // Filename is valid
}

bool FileExistInDir(const std::vector <std::wstring>* fileList, const std::wstring fileName) {
    for (const auto& file : *fileList) {
        if (file == fileName) {
            return true; // The file was found in the list
        }
    }
    return false; // The file was not found in the list
}


int WStringToInteger(const std::wstring& input)
{
    int number = 0;

    // Convert std::wstring to int using std::wstringstream
    std::wstringstream wss(input);
    wss >> number;

    // Check if the conversion was successful
    if (!wss.fail() && !wss.bad() && wss.eof()) {
        return number;
    }
    // else return -1
    return -1;
}

// makes a list of files in a given subdirectory
std::vector<std::wstring>* GetFileList(const std::wstring& folderName, const std::wstring& extensionName = L'\0')
{
    // create ptr to empty list
    std::vector<std::wstring>* fileList{ new std::vector<std::wstring>{} };

    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // Construct the search pattern, e.g., "C:\\path\\*.spr"
    std::wstring searchPattern = std::wstring(folderName) + L"*" + extensionName;

    std::wcout << L"\n--------- Searching for " << extensionName << " Files in " << folderName << " ------------\n\n";

    // Start the file search
    hFind = FindFirstFile(searchPattern.c_str(), &findFileData);

    // return an empty list if error
    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcout << L"No files found\n";
        return fileList; // list will be empty
    }
    else {
        short i = 1;
        do {
            // push name onto list
            fileList->push_back(findFileData.cFileName);
            std::wcout << i << L". " << findFileData.cFileName << std::endl;
            i++;
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    return fileList;
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