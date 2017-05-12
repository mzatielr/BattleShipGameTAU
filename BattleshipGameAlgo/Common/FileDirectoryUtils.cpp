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

string IFileDirectoryUtils::GetFullPath(const string& relativePath)
{
	char path[_MAX_PATH];
	char* result = _fullpath(path, relativePath.c_str(), _MAX_PATH);
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

int IFileDirectoryUtils::GetAllFiles(const string& basePath, const string& filePattern, vector<string>& collection)
{
	int counter = 0;
	WIN32_FIND_DATAA fileData; //data struct for file

	string fullPattern = basePath + "\\" + filePattern;
	HANDLE handle = FindFirstFileA((fullPattern).c_str(), &fileData);
	if(handle == INVALID_HANDLE_VALUE)
	{
		MainLogger.logFile << "[GetAllFiles] Invalid Handle Value detected for " << fullPattern << endl;
		if(GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			MainLogger.logFile << "[GetAllFiles] File not found for pattern: " << fullPattern << endl;
		}
		return 0;
	}
	do
	{
		string path = basePath + "\\" + fileData.cFileName;
		MainLogger.logFile << "[GetAllFiles] File was found: " << path << endl;
		collection.push_back(path);
		counter++;
	} while (FindNextFileA(handle, &fileData)); // Notice: Unicode compatible version of FindNextFile

	if(GetLastError() != ERROR_NO_MORE_FILES)
	{
		MainLogger.logFile << "[GetAllFiles] Error accured in FindNextFileA" << endl;
	}
	
	FindClose(handle);

	sort(collection.begin(), collection.end());
	return counter;
}
