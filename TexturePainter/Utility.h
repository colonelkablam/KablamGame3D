#pragma once

#include <unordered_map>
#include <Windows.h>
#include <vector>
#include <string>

void SetConsoleWindowSize(int width, int height, int extraBufferLines = 0);

void SetConsoleFont(int fontSize, const wchar_t* fontName = L"Lucida Console");

void CenterConsoleWindow();

bool GetValidFileName(const std::wstring& prompt, std::wstring& userInput, const std::wstring& illegalChars, const int maxLength);

bool IsValidFileName(const std::wstring& fileName, const std::wstring& illegalChars, const int maxLength);

std::vector<std::wstring> GetFileList(const std::wstring& folderName, const std::wstring& extensionName);

void PrintFileList(const std::wstring& header, const std::vector<std::wstring>& fileList);

bool FileExistInDir(const std::vector <std::wstring>& fileList, const std::wstring& fileName);

void GetDimensionInput(const std::wstring& prompt, int& userInput, int min, int max);

bool GetYesNoInput(const std::wstring& prompt);

bool LetterListener(const std::wstring& userInput, const wchar_t checkLetter);

int WStringToInteger(const std::wstring & input);

bool CheckFolderExists(const std::wstring& folderPath);

bool CreateFolder(const std::wstring& folderPath);

void printChunkyString(const std::string& text);

void printCharacterRow(HANDLE hConsole, const std::unordered_map<char, std::vector<std::string>>& charMap, char ch, int row, WORD originalAttributes);



