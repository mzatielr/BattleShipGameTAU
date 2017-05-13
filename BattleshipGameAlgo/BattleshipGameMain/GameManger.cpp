#include "GameManager.h"
#include "../Common/IFileDirectoryUtils.h"
#include "../Common/Contants.h"
#include "../Common/GameBoardUtils.h"

GameManager::GameManager(Configuration& config): config(config)
{
}

bool GameManager::ConfigureDll() 
{
	vector<string> collection;
	int cnt = IFileDirectoryUtils::GetAllFiles(config.path, "*.dll", collection);
	if (cnt < 2)
	{
		MainLogger.logFile << "Missing *.dll file. Found " << cnt << endl;
		cout << "Missing an algorithm (dll) file looking in path: " << config.path << endl;
		return false;
	}

	dllPaths.first = collection[0];
	dllPaths.second = collection[1];

	MainLogger.logFile << "First Algo path: " << dllPaths.first << endl;
	MainLogger.logFile << "Second Algo path: " << dllPaths.second << endl;
	return true;
}

bool GameManager::ConfigurePath() const
{
	if(!config.path.empty())
	{
		MainLogger.logFile << "Path provided in the argument is " << config.path << endl;
		config.path = IFileDirectoryUtils::GetFullPath(config.path);
		if(IFileDirectoryUtils::DirExists(config.path))
		{
			MainLogger.logFile << "Full path directory exist: " << config.path << endl;
			return true;
		}
		MainLogger.logFile << "Path is not exist " << config.path << endl;
		cout << "Wrong Path: " << config.path << endl;
		return false;
	}
	MainLogger.logFile << "Path is not specified in the argument, getting current working directory" << endl;
	config.path = IFileDirectoryUtils::GetCurrentWorkingDirectory();
	
	if(IFileDirectoryUtils::DirExists(config.path))
	{
		MainLogger.logFile << "Current working directory is " << config.path << endl;
		return true;
	}
	cout << "Wrong Path: " << config.path << endl;
	return false;
}

bool GameManager::ConfigureBoard()
{
	vector<string> collection;
	int cnt = IFileDirectoryUtils::GetAllFiles(config.path, "*.sboard", collection);
	if(cnt <= 0)
	{
		MainLogger.logFile << "Missing *.sboard file" << endl;
		cout << "Missing board file (*.sboard) looking in path: " << config.path << endl;
		return false;
	}
	
	string boardPath = collection[0];
	MainLogger.logFile << "Board path in use is " << boardPath << endl;

	mainGameBoard = GameBoardUtils::InitializeNewEmptyBoard();
	//load main game board from file & validate the board
	if (GameBoardUtils::LoadBoardFromFile(mainGameBoard, ROWS, COLS, boardPath) != BoardFileErrorCode::Success)
	{
		MainLogger.logFile << "Failed to validate successfuly board" << endl;
		return false;
	}

	MainLogger.logFile << "Board was validated successfuly" << endl;
	GameBoardUtils::PrintBoard(MainLogger.logFile, mainGameBoard, ROWS, COLS);
	return true;
}

bool GameManager::ConfigureFiles()
{
	bool res1 = ConfigureBoard();
	bool res2 = ConfigureDll();

	return res1 && res2;
}

bool GameManager::InitPlayers()
{
	return InitDllAlgo(algo1, dllPaths.first, PlayerAID) && InitDllAlgo(algo2, dllPaths.second, PlayerBID);
}

bool GameManager::InitDllAlgo(DllAlgo& algo, const string & path, int playerID) const
{
	bool result  = algo.LoadDll(path);
	if(!result)
	{
		cout << "Cannot load dll: " << path;
		return false;
	}

	// Set Player Board
	char** playerboard = GameBoardUtils::ClonePlayerBoard(const_cast<const char**>(mainGameBoard), playerID);
	algo.algo->setBoard(playerID, const_cast<const char**>(playerboard), ROWS, COLS);
	MainLogger.logFile << "Set board for player " << playerID << " finished" << endl;

	// Init
	result = algo.algo->init(config.path);
	if(!result)
	{
		MainLogger.logFile << "Failed to init for player " << playerID << endl;
		cout << "Algorithm initialization failed for dll: " << path << endl;
		return false;
	}
	
	MainLogger.logFile << "Init successfuly for player " << playerID << endl;
	return true;
}

int GameManager::GameInitializer()
{
	bool result = ConfigurePath();
	if (!result)
	{
		return ErrorExitCode;
	}

	result = ConfigureFiles();
	if (!result)
	{
		return ErrorExitCode;
	}

	result = InitPlayers();
	if (!result)
	{
		return ErrorExitCode;
	}

	return 0;
}

int GameManager::RunGame()
{
	int code = GameInitializer();
	if(code == ErrorExitCode)
	{
		return code;
	}
	MainLogger.logFile << "===== Game Initilized =======" << endl;

}


