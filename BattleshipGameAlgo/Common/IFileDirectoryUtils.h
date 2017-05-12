#pragma once
#include "IOLib.h"
#include <vector>

using namespace std;

extern Logger MainLogger;

class IFileDirectoryUtils
{
public:
	static string GetCurrentWorkingDirectory();
	static string GetFullPath(const string& relativePath);
	static bool DirExists(const std::string& dirName_in);
	static int GetAllFiles(const string& basePath, const string& filePattern, vector<string>& collection);
};
