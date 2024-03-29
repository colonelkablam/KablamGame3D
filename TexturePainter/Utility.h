#pragma once

#include <vector>
#include <string>

bool GetValidFileName(const std::wstring& filename, std::wstring& userInput);

std::vector<std::wstring> GetFileList(const std::wstring& folderName, const std::wstring& extensionName);

void PrintFileList(const std::wstring& header, const std::vector<std::wstring>& fileList);

bool FileExistInDir(const std::vector <std::wstring>& fileList, const std::wstring& fileName);

void GetDimensionInput(const std::wstring& prompt, int& userInput, int min, int max);

bool GetYesNoInput(const std::wstring& prompt);

bool LetterListener(const std::wstring& userInput, const wchar_t checkLetter);

int WStringToInteger(const std::wstring & input);

bool CheckFolderExists(const std::wstring& folderPath);

bool CreateFolder(const std::wstring& folderPath);

