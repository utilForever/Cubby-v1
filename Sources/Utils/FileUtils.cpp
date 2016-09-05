/*************************************************************************
> File Name: FiltUtils.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Some general file and folder utility functions.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#endif

#include "FileUtils.h"

std::string wchar_tTostring(const wchar_t* wchar)
{
	std::string str = "";
	int index = 0;
	while (wchar[index] != 0)
	{
		str += static_cast<char>(wchar[index]);
		++index;
	}
	return str;
}

wchar_t* stringTowchar_t(const std::string& str)
{
	static wchar_t wchar[260];
	unsigned int index = 0;
	while (index < str.size())
	{
		wchar[index] = static_cast<wchar_t>(str[index]);
		++index;
	}
	wchar[index] = 0;
	return wchar;
}

std::vector<std::string> ListFilesInDirectory(std::string directoryName)
{
#ifdef _WIN32
	WIN32_FIND_DATA findFileData;
	std::wstring wideStr = std::wstring(directoryName.begin(), directoryName.end());
	const wchar_t* fileName = wideStr.c_str();
	HANDLE hFind = FindFirstFile(fileName, &findFileData);
	std::vector<std::string> listFileNames;

	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
		{
			listFileNames.push_back(wchar_tTostring(findFileData.cFileName));
		}

		while (FindNextFile(hFind, &findFileData))
		{
			listFileNames.push_back(wchar_tTostring(findFileData.cFileName));
		}

		FindClose(hFind);
	}
	else
	{
		DWORD errorCode = GetLastError();
		std::cout << "ListFilesInDirectory() failed with error: " << errorCode << std::endl;
	}

	return listFileNames;
#elif __linux__
	directoryName = directoryName.substr(0, directoryName.length() - 3);
	std::vector<std::string> listFileNames;
	DIR* dp;
	struct dirent* dirp;
	if ((dp = opendir(directoryName.c_str())) == nullptr)
	{
		std::cout << "Error(" << errno << ") opening " << directoryName << std::endl;
		return listFileNames;
	}

	while ((dirp = readdir(dp)) != nullptr)
	{
		listFileNames.push_back(std::string(dirp->d_name));
	}

	closedir(dp);

	return listFileNames;
#endif //_WIN32
}
