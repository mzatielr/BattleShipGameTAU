#include "../Common/GameBoardUtils.h"
#include "../Common/Bonus.h"
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "../Common/IFileDirectoryUtils.h"
#include "IArgumentParser.h"
#include "GameManager.h"

Logger MainLogger;

using namespace std;

int main(int argc, char* argv[]) 
{
	Configuration config;
	IArgumentParser::ParseArguments(config, argc, argv);
	GameManager game(config);

	int exitCode = game.RunGame();
	return exitCode;
}

#pragma region Test

void FileDirectoryUtils_GetCurrentWorkingDirectory_PositiveScenario()
{
	GameBoardUtils::InitLogger(MainLogger, "FileDirectoryTest.log");

	cout << "Current working directory is " << IFileDirectoryUtils::GetCurrentWorkingDirectory() << endl;

	MainLogger.LoggerDispose();
}

void FileDirectoryUtils_GetFullPath_PositiveScenario()
{
	GameBoardUtils::InitLogger(MainLogger, "GetFullPathTest.log");

	cout << "Full Path is " << IFileDirectoryUtils::GetFullPath("C:\\Users\\mzaitler\\Downloads\\DynamicLoading") << endl;

	MainLogger.LoggerDispose();
}

void FileDirectoryUtils_DirExists_PositiveScenario()
{
	GameBoardUtils::InitLogger(MainLogger, "GetFullPathTest.log");

	cout << "DirExists " << IFileDirectoryUtils::DirExists("C:\\Users\\mzaitler\\Downloads\\DynamicLoading") << endl;

	MainLogger.LoggerDispose();
}

void FileDirectoryUtils_GetAllFiles_PositiveScenario()
{
	GameBoardUtils::InitLogger(MainLogger, "GetAllFiles.log");
	vector<string> matchFile;
	cout << "Num of match file found " << IFileDirectoryUtils::GetAllFiles("C:\\temp\\New folder", "*.dll", matchFile) << endl;

	for (auto itr = matchFile.begin(); itr != matchFile.end(); ++itr)
		cout << *itr << endl;

	MainLogger.LoggerDispose();
}
#pragma endregion 
