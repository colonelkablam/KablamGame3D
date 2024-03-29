#include <windows.h>
#include <iostream>
#include <sstream> // For std::wstringstream
#include <cwctype> // For std::towlower & std::iswalpha

#include "Utility.h"

bool GetValidFileName(const std::wstring& prompt, std::wstring& userInput) {
    // illegal characters
    const std::wstring illegalChars{ L".*<>:\"/\\|?" };

    std::wcout << prompt;
    std::getline(std::wcin, userInput);

    // Check for illegal characters
    bool hasIllegalChars = false;
    for (wchar_t ch : userInput) {
        if (ch < 32 || illegalChars.find(ch) != std::wstring::npos) {
            hasIllegalChars = true;
            std::wcout << L"\nInput must not contain illegal characters (.*<>:\"/\\|?)...\n\n";
            return false; // No need to check further characters
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