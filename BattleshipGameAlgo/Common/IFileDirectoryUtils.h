#pragma once
#include "IOLib.h"


using namespace std;

extern Logger MainLogger;

class IFileDirectoryUtils
{
public:
	static string GetCurrentWorkingDirectory();
	static string GetFullPath(char const* relativePath);
	static bool DirExists(const std::string& dirName_in);
};
