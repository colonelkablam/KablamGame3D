#pragma once

#include <vector>
#include <string>

bool GetValidFileName(const std::wstring& filename, std::wstring& userInput);

int WStringToInteger(const std::wstring& input);

std::vector<std::wstring> PrintAndGetFileList(const std::wstring& folderName, const std::wstring& extensionName);

bool FileExistInDir(const std::vector <std::wstring>& fileList, const std::wstring& fileName);

void GetDimensionInput(const std::wstring& prompt, int& userInput, int min, int max);

bool GetYesNoInput(const std::wstring& prompt);

bool CheckFolderExists(const std::wstring& folderPath);

bool CreateFolder(const std::wstring& folderPath);

