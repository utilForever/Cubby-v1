/*************************************************************************
> File Name: FiltUtils.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Some general file and folder utility functions.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FILE_UTILS_H
#define CUBBY_FILE_UTILS_H

#include <string>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#define fopen_s(pFile, fileName, mode) ((*(pFile)) = fopen((fileName), (mode))) == NULL
#endif

std::string wchar_tTostring(const wchar_t* wchar);
wchar_t* stringTowchar_t(const std::string& str);
std::vector<std::string> ListFilesInDirectory(std::string directoryName);

#endif