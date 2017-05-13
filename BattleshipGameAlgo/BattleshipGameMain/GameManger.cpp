#include "GameManager.h"
#include "../Common/IFileDirectoryUtils.h"
#include "../Common/Contants.h"
#include "../Common/GameBoardUtils.h"
#include "ShipDetailsBoard.h"

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

	mainGameBoard = GameBoardUtils::InitializeNewEmptyBoard(ROWS,COLS);
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
	char** playerboard = GameBoardUtils::ClonePlayerBoard(const_cast<const char**>(mainGameBoard), playerID,ROWS,COLS);
	algo.algo->setBoard(playerID, const_cast<const char**>(playerboard), ROWS, COLS);
	GameBoardUtils::DeleteBoard(playerboard, ROWS);

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

pair<int, int> GameManager::GetNextPlayerAttack(int player_id, IBattleshipGameAlgo*  player_a, IBattleshipGameAlgo* player_b)
{
	if (player_id == PlayerAID)
	{
		return player_a->attack();
	}
	if (player_id == PlayerBID)
	{
		return player_b->attack();
	}
	// Fatal Error
	MainLogger.logFile << "Fatal error occured. Attack move was asked for non exixting player id " << player_id << endl;
	return{ };
}


AttackResult GameManager::GetAttackResult(const pair<int, int>& pair, char** board, ShipDetailsBoard& detailsA, ShipDetailsBoard& detailsB)
{
	return GameBoardUtils::IsPlayerIdChar(PlayerAID, board[pair.first][pair.second]) ? detailsA.GetAttackResult(pair) : detailsB.GetAttackResult(pair);
}

void GameManager::PrintPoints(ShipDetailsBoard& playerA, ShipDetailsBoard& playerB)
{
	cout << "Points:" << endl;
	cout << "Player A: " << playerB.negativeScore << endl;
	cout << "Player B: " << playerA.negativeScore << endl;
}

void GameManager::PrintSinkCharRec(char** maingameboard, Bonus& b, int i, int j, int player)
{
	if (i < 0 || i >= ROWS || j < 0 || j >= COLS) // Stop recursion condition
	{
		return;
	}

	char currentCell = maingameboard[i][j];
	if (currentCell != HIT_CHAR)
		return;

	maingameboard[i][j] = SINK_CHAR;
	b.PrintPlayerChar(maingameboard[i][j], j, i, player);
	PrintSinkCharRec(maingameboard, b, i, j - 1, player);
	PrintSinkCharRec(maingameboard, b, i, j + 1, player);
	PrintSinkCharRec(maingameboard, b, i - 1, j, player);
	PrintSinkCharRec(maingameboard, b, i + 1, j, player);
}

bool GameManager::IsPlayerWon(int currentPlayer, ShipDetailsBoard& detailsA, ShipDetailsBoard& detailsB)
{
	return currentPlayer == PlayerAID ? detailsB.IsLoose() : detailsA.IsLoose();
}

// Attack pair 1-10
bool GameManager::ValidAttackCor(const pair<int, int>& pair)
{
	return pair.first > 0 && pair.first <= ROWS && pair.second > 0 && pair.second <= COLS;
}


int GameManager::PlayGame() const
{
	ShipDetailsBoard playerAboardDetails(mainGameBoard, PlayerAID);
	ShipDetailsBoard playerBboardDetails(mainGameBoard, PlayerBID);

	int playerIdToPlayNext = PlayerAID;

	// Configure Bonus start point and color
	Bonus bonus(!config.bonusParam.isQuiet, config.bonusParam.delayInMiliseconds);
	bonus.Init(mainGameBoard, ROWS, COLS);
	GameBoardUtils::ChangeFontSize();

	//main game play
	bool AattacksDone = false;
	bool BattacksDone = false;

	// While not both of players ended their attacks
	while (!AattacksDone || !BattacksDone)
	{
		pair<int, int> tempPair = GetNextPlayerAttack(playerIdToPlayNext, algo1.algo, algo2.algo); 
		if (tempPair.first == AttckDoneIndex && (tempPair.second == AttckDoneIndex))
		{
			switch (playerIdToPlayNext)
			{
			case PlayerAID:
				AattacksDone = true;
				break;
			case PlayerBID:
				BattacksDone = true;
				break;
			default:;
			}

			// Flip players
			playerIdToPlayNext = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
		}
		else if (ValidAttackCor(tempPair))
		{
			//aligned both axis -1 because main board starts from (0,0)
			tempPair = { tempPair.first - 1,tempPair.second - 1 };

			char attckCell = mainGameBoard[tempPair.first][tempPair.second];
			bool isSelfAttack = GameBoardUtils::IsPlayerIdChar(playerIdToPlayNext, attckCell);

			//calculate attack and update mainboard
			AttackResult tempattackresult = GetAttackResult(tempPair, mainGameBoard, playerAboardDetails, playerBboardDetails);

			string resultDesc;
			switch (tempattackresult)
			{
			case AttackResult::Miss: resultDesc = "Miss";  break;
			case AttackResult::Hit: resultDesc = "Hit"; break;
			case AttackResult::Sink:resultDesc = "Sink"; break;
			default: ;
			}

			MainLogger.logFile << "Player " << playerIdToPlayNext << " attack in (" << tempPair.first << "," << tempPair.second << ") result: " << resultDesc << endl;

			//update players - Notify with values 1-10 and not 0-9
			algo1.algo->notifyOnAttackResult(playerIdToPlayNext, tempPair.first + 1, tempPair.second + 1, tempattackresult);
			algo2.algo->notifyOnAttackResult(playerIdToPlayNext, tempPair.first + 1, tempPair.second + 1, tempattackresult);

			if (tempattackresult != AttackResult::Miss)
			{
				int playerTosetColor;
				if (isSelfAttack)
				{
					playerTosetColor = playerIdToPlayNext;
				}
				else
				{
					playerTosetColor = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
				}

				if (tempattackresult == AttackResult::Sink)
				{
					// In case sink update all the cell to SINK_CHAR
					PrintSinkCharRec(mainGameBoard, bonus, tempPair.first, tempPair.second, playerTosetColor);
				}
				else // In case hit update only the target cell
					bonus.PrintPlayerChar(mainGameBoard[tempPair.first][tempPair.second], tempPair.second, tempPair.first, playerTosetColor);
			}

			if (tempattackresult == AttackResult::Miss || isSelfAttack)
			{
				// Flip Players
				playerIdToPlayNext = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
			}

			if (IsPlayerWon(PlayerAID, playerAboardDetails, playerBboardDetails))
			{
				bonus.Dispose(); // Important: Don't touch and don't change the order of statements [Mordehai]
				cout << "Player A won" << endl;
				PrintPoints(playerAboardDetails, playerBboardDetails);
				return 0;
			}
			if (IsPlayerWon(PlayerBID, playerAboardDetails, playerBboardDetails))
			{
				bonus.Dispose(); // Important: Don't touch and don't change the order of statements [Mordehai]
				cout << "Player B won" << endl;
				PrintPoints(playerAboardDetails, playerBboardDetails);
				return 0;
			}
		}
		else
		{
			MainLogger.logFile << "Invlaid attack <" << tempPair.first << "," << tempPair.second
								<< "> for player " << playerIdToPlayNext << ". Flipping players" << endl;
			// Flip players
			playerIdToPlayNext = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
		}
	}

	bonus.Dispose(); // Important: Don't touch and don't change the order of statements [Mordehai]
	PrintPoints(playerAboardDetails, playerBboardDetails);
	return 0;
}

void GameManager::GameManagerCleanup() const
{
	GameBoardUtils::DeleteBoard(mainGameBoard, ROWS);
	algo1.Dispose();
	algo2.Dispose();

	MainLogger.LoggerDispose();
}

int GameManager::RunGame()
{
	GameBoardUtils::InitLogger(MainLogger, "GameManager.log");

	int code = GameInitializer();
	if(code == ErrorExitCode)
	{
		return code;
	}
	MainLogger.logFile << "===== Game Initilized =======" << endl;

	
	code = PlayGame();
	
	MainLogger.logFile << "Game exit code is " << code << endl;

	GameManagerCleanup();
	return code;
}


#pragma region Test

void GameManager::Test_GetAllAttacks() const
{
	pair<int, int> attack = algo1.algo->attack();
	while (attack.first != AttckDoneIndex)
	{
		MainLogger.logFile << attack.first << "," << attack.second << endl;
		attack = algo1.algo->attack();
	}
}

#pragma endregion 