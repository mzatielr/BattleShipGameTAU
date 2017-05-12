#include "IFileDirectoryUtils.h"
#include "Windows.h"

string IFileDirectoryUtils::GetCurrentWorkingDirectory()
{
	char currentdirectory[_MAX_PATH];
	DWORD value = GetCurrentDirectoryA(_MAX_PATH, currentdirectory);
	if(value == 0)
	{
		// Error eccured
		DWORD error = GetLastError();
		MainLogger.logFile << "[GetCurrentWorkingDirectory] Error ecurred: " << error << endl;
		return string();
	}

	string dir = string(currentdirectory);
	MainLogger.logFile << "[GetCurrentWorkingDirectory] Current Working dir is " << dir << endl;
	return dir;
}

string IFileDirectoryUtils::GetFullPath(char const* relativePath)
{
	char path[_MAX_PATH];
	char* result = _fullpath(path, relativePath, _MAX_PATH);
	if(result == nullptr)
	{
		MainLogger.logFile << "[GetFullPath] Failed to get full path for " << relativePath << endl;
		return string();
	}
	
	MainLogger.logFile << "[GetFullPath] Full path of " << relativePath <<  " is " << path << endl;
	return string(path);
}

//taken from: http://stackoverflow.com/questions/8233842/how-to-check-if-directory-exist-using-c-and-winapi
bool IFileDirectoryUtils::DirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}