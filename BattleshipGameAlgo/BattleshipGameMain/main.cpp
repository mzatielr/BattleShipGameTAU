#include "../Common/GameBoardUtils.h"
#include "ShipDetailsBoard.h"
#include "../Common/Bonus.h"
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "Windows.h"

Logger MainLogger;

typedef IBattleshipGameAlgo *(*GetAlgorithmFuncType)();

using namespace std;

// Dll Loading global variables
vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> dll_vec; // vector of <Algo Name, dll handle, GetAlgorithm function ptr>
vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>::iterator vitr;

void FreeLoadedResources ()
{
	// close all the dynamic libs we opened
	for (vitr = dll_vec.begin(); vitr != dll_vec.end(); ++vitr)
	{
		FreeLibrary(get<1>(*vitr));
	}
}

int LoadDllFilesByOrder(string dirPath, GetAlgorithmFuncType& playerA, GetAlgorithmFuncType& playerB, bool dirExists) 
{
	char currentdirectory[_MAX_PATH];
	_fullpath(currentdirectory, ".\\", _MAX_PATH); // obtain current directory

	WIN32_FIND_DATAA fileData; //data struct for file
	vector<HINSTANCE>::iterator itr;

	// vector of shape objects we create
	vector<IBattleshipGameAlgo *> shape_vec;
	vector<IBattleshipGameAlgo *>::iterator aitr;

	// define function of the type we expect
	vector<string> algoNames;

	// iterate over *.dll files in path
	string s = "\\*.dll"; // only .dll endings

	if (!dirExists) 
	{
		dirPath.assign(currentdirectory + (string)"Debug");
	}

	HANDLE dir = FindFirstFileA((dirPath + s).c_str(), &fileData); 
	if (dir != INVALID_HANDLE_VALUE) //check if the dir opened successfully
	{
		// test each file suffix and set variables as needed
		do
		{
			string fileName = fileData.cFileName;
			string fullFileName = dirPath + "\\" + fileName;
			string AlgorithmName = fileName.substr(0, fileName.find("."));

			// Load dynamic library
			HINSTANCE hDll = LoadLibraryA(fullFileName.c_str()); // Notice: Unicode compatible version of LoadLibrary
			if (!hDll)
			{
				cout << "could not load the dynamic library" << endl;
				return EXIT_FAILURE;
			}

			// Get function pointer
			GetAlgorithmFuncType GetAlgorithmFunc = (GetAlgorithmFuncType)GetProcAddress(hDll, "GetAlgorithm");
			if (!GetAlgorithmFunc)
			{
				cout << "could not load function GetAlgorithm()" << endl;
				return EXIT_FAILURE;
			}

			//insert to the vector tuple (algoName,handler,pointer to func)
			dll_vec.push_back(make_tuple(AlgorithmName, hDll, GetAlgorithmFunc));
			algoNames.push_back(AlgorithmName);

		} while (FindNextFileA(dir, &fileData)); // Notice: Unicode compatible version of FindNextFile
	}
	else {
		cout << "ERROR: could not load dll files" << endl;
		return EXIT_FAILURE; 
	}
	if (dll_vec.size() < 2) {
		cout << "ERROR: not enough dll files" << endl;
		return EXIT_FAILURE; //TODO: search working directory
	}

	//sort vector of algorithm names
	sort(algoNames.begin(), algoNames.end());
	string firstAlgoName = algoNames[0]; 
	string secondAlgoName = algoNames[1];
	
	//find first algo implemantation in dll_vec
	for (vitr=dll_vec.begin(); vitr != dll_vec.end(); ++vitr) {
		if (!firstAlgoName.compare(get<0>(*vitr))) {
			playerA = get<2>(*vitr);
		}
		if (!secondAlgoName.compare(get<0>(*vitr))) {
			playerB = get<2>(*vitr);
		}
	}
	return 0;
}

void PrintPoints(ShipDetailsBoard& playerA, ShipDetailsBoard& playerB)
{
	cout << "Points:" << endl;
	cout << "Player A: " << playerB.negativeScore << endl;
	cout << "Player B: " << playerA.negativeScore << endl;
}

void PrintSinkCharRec(char** maingameboard, Bonus* b, int i, int j, int player)
{
	if (i < 0 || i >= ROWS || j < 0 || j >= COLS) // Stop recursion condition
	{
		return;
	}

	char currentCell = maingameboard[i][j];
	if (currentCell != HIT_CHAR)
		return;

	maingameboard[i][j] = SINK_CHAR;
	b->PrintPlayerChar(maingameboard[i][j], j, i, player);
	PrintSinkCharRec(maingameboard, b, i, j - 1, player);
	PrintSinkCharRec(maingameboard, b, i, j + 1, player);
	PrintSinkCharRec(maingameboard, b, i - 1, j, player);
	PrintSinkCharRec(maingameboard, b, i + 1, j, player);
}

void SetPlayerBoards(char** board, IBattleshipGameAlgo*& playerA, IBattleshipGameAlgo*& playerB)
{
	char** playerAboard = GameBoardUtils::ClonePlayerBoard(const_cast<const char**>(board), PlayerAID);
	MainLogger.logFile << "CloneBoardForA" << endl;
	GameBoardUtils::PrintBoard(MainLogger.logFile, playerAboard, ROWS, COLS);

	char** playerBboard = GameBoardUtils::ClonePlayerBoard(const_cast<const char**>(board), PlayerBID);
	MainLogger.logFile << "CloneBoardForB" << endl;
	GameBoardUtils::PrintBoard(MainLogger.logFile, playerBboard, ROWS, COLS);
	playerA->setBoard(0,const_cast<const char**>(playerAboard), ROWS, COLS);
	playerB->setBoard(1,const_cast<const char**>(playerBboard), ROWS, COLS);

	GameBoardUtils::DeleteBoard(playerAboard);
	GameBoardUtils::DeleteBoard(playerBboard);
}

pair<int, int> GetNextPlayerAttack(int player_id,  IBattleshipGameAlgo*  player_a, IBattleshipGameAlgo* player_b)
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
	return{ ErrorDuringGetAttackIndex,ErrorDuringGetAttackIndex };
}

AttackResult GetAttackResult(const pair<int, int>& pair, char** board, ShipDetailsBoard& detailsA, ShipDetailsBoard& detailsB)
{
	return GameBoardUtils::IsPlayerIdChar(PlayerAID,board[pair.first][pair.second]) ? detailsA.GetAttackResult(pair) : detailsB.GetAttackResult(pair);
}

bool IsPlayerWon(int currentPlayer, ShipDetailsBoard& detailsA, ShipDetailsBoard& detailsB)
{
	return currentPlayer == PlayerAID ? detailsB.IsLoose() : detailsA.IsLoose();
}

// Free all global variables
void FreeGlobalVariable()
{
	MainLogger.LoggerDispose();
	FreeLoadedResources();
}

int main(int argc, char* argv[]) 
{
	GetAlgorithmFuncType getPlayerAAlgo, getPlayerBAlgo;

	bool dirExists = false; 
	GameBoardUtils::InitLogger(MainLogger, "Battle.log");
	
	// Configure Bonus start point and color
	GameBoardUtils::ChangeFontSize();
	BonusParams p; 

	//Todo: Move all this if to function [Optional]
	//Todo: Ozeri refacrtor and improve implementation - something better than "NS" [Optional]
	string dirPath("NS");
	
	//aquiring arguments and checking if dir exists
	if (argc > 1) 
	{
		for (int i = 1; i < argc; i++) 
		{
			string s = argv[i];
			if (!s.compare("-quiet")) {
				p.isQuiet = true;
			}
			else if (!s.compare("-delay")) 
			{
				int del = atoi(argv[i + 1]);
				p.delayInMiliseconds = del;
				i++;
			}
			else
			{
				dirPath = argv[i];
			}
		}
		if (dirPath.compare("NS"))
		{
			dirExists = GameBoardUtils::DirExists(dirPath);
			if (!dirExists) 
			{
				cout << "Wrong path:" << dirPath << endl;
			}
		}
	}

	//TODO: add search in working directory in case less then 2 were found + not working on command line with spaces
	if (LoadDllFilesByOrder(dirPath, getPlayerAAlgo, getPlayerBAlgo, dirExists))
	{
		cout << "Error loading dll files. exiting" << endl;
		MainLogger.LoggerDispose();
		return ErrorExitCode; //TODO: release all and check is ok
	}

	//get .sboard file path
	string mainboardpath = GameBoardUtils::GetFilePathBySuffix(argc, dirPath,".sboard",dirExists);
	if (mainboardpath == "ERR") {
		cout << "ERROR occured while getting board path" << endl;
		MainLogger.LoggerDispose();
		return ErrorExitCode;
	}

	// board - will save updated and full board of two players
	char** mainGameBoard = GameBoardUtils::InitializeNewEmptyBoard();
	//load main game board from file & validate the board
	if(GameBoardUtils::LoadBoardFromFile(mainGameBoard, ROWS, COLS, mainboardpath)!= BoardFileErrorCode::Success)
	{
		GameBoardUtils::DeleteBoard(mainGameBoard);
		MainLogger.LoggerDispose();
		return ErrorExitCode;
	}
	GameBoardUtils::PrintBoard(MainLogger.logFile, mainGameBoard, ROWS, COLS);

	//Todo: Move all this to PreDefinedStepsAlgo - we should pass a folder name and not a specific file [Must]
	//get attack files path
	string Aattackpath = GameBoardUtils::GetFilePathBySuffix(argc, dirPath, ".attack-a", dirExists);
	if (Aattackpath == "ERR") 
	{
		cout << "ERROR in retrieving attack file of player A" << endl;
		GameBoardUtils::DeleteBoard(mainGameBoard);
		FreeGlobalVariable();
		return ErrorExitCode;
	}
	string Battackpath = GameBoardUtils::GetFilePathBySuffix(argc, dirPath, ".attack-b", dirExists);
	if (Battackpath == "ERR") 
	{
		cout << "ERROR in retrieving attack file of player B" << endl;
		GameBoardUtils::DeleteBoard(mainGameBoard);
		FreeGlobalVariable();
		return ErrorExitCode;
	}

	// Init players Instances
	IBattleshipGameAlgo* algoA = getPlayerAAlgo();
	IBattleshipGameAlgo* algoB = getPlayerBAlgo();

	//setting player boards
	SetPlayerBoards(mainGameBoard, algoA, algoB);
	if (!algoA->init(Aattackpath) || !algoB->init(Battackpath)) 
	{
		cout << "ERROR: could not init attack files" << endl;
		GameBoardUtils::DeleteBoard(mainGameBoard);
		FreeGlobalVariable();
		return ErrorExitCode; //TODO: release all dynamic allocations
	}

	ShipDetailsBoard playerAboardDetails(mainGameBoard, PlayerAID);
	ShipDetailsBoard playerBboardDetails(mainGameBoard, PlayerBID);

	int playerIdToPlayNext = PlayerAID;

	Bonus* bonus = new Bonus(!p.isQuiet, p.delayInMiliseconds);
	bonus->Init(mainGameBoard, ROWS, COLS);
	
	//main game play

	bool AattacksDone = false;
	bool BattacksDone = false;

	// While not both of players ended their attacks - //TODO: make outsidebollean to set instead of attacks done.
	while (!AattacksDone || !BattacksDone)
	{
		pair<int, int> tempPair = GetNextPlayerAttack(playerIdToPlayNext, algoA, algoB); //TODO: move all func from main into GameBoardUtils [Optional]

		//Error occurred 
		if (tempPair.first == ErrorDuringGetAttackIndex && tempPair.second == ErrorDuringGetAttackIndex)
		{
			GameBoardUtils::DeleteBoard(mainGameBoard);
			MainLogger.LoggerDispose();
			return ErrorExitCode;
		}
		//end of attacks
		if ((tempPair.first == AttckDoneIndex) && (tempPair.second == AttckDoneIndex))
		{
			if(playerIdToPlayNext)
			{
				AattacksDone = true;
			}
			else
			{
				BattacksDone = true;
			}
			// Flip players
			playerIdToPlayNext = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
		}
		else
		{
			//aligned both axis -1 because main board starts from (0,0)
			tempPair = { tempPair.first - 1,tempPair.second - 1 };

			char attckCell = mainGameBoard[tempPair.first][tempPair.second];
			bool isSelfAttack = GameBoardUtils::IsPlayerIdChar(playerIdToPlayNext, attckCell);

			//calculate attack and update mainboard
			AttackResult tempattackresult = GetAttackResult(tempPair, mainGameBoard, playerAboardDetails, playerBboardDetails);

			//update players
			algoA->notifyOnAttackResult(playerIdToPlayNext, tempPair.first, tempPair.second, tempattackresult);
			algoB->notifyOnAttackResult(playerIdToPlayNext, tempPair.first, tempPair.second, tempattackresult);

			if (tempattackresult != AttackResult::Miss)
			{
				int playerTosetColor;
				if(isSelfAttack)
				{
					playerTosetColor = playerIdToPlayNext;
				}
				else
				{
					playerTosetColor = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
				}

				if(tempattackresult == AttackResult::Sink)
				{
					// In case sink update all the cell to SINK_CHAR
					PrintSinkCharRec(mainGameBoard,bonus, tempPair.first, tempPair.second, playerTosetColor);
				}
				else // In case hit update only the target cell
					bonus->PrintPlayerChar(mainGameBoard[tempPair.first][tempPair.second], tempPair.second, tempPair.first, playerTosetColor);
			}

			if (tempattackresult == AttackResult::Miss || isSelfAttack)
			{
				// Flip Players
				playerIdToPlayNext = (playerIdToPlayNext == PlayerAID) ? PlayerBID : PlayerAID;
			}

			if (IsPlayerWon(PlayerAID, playerAboardDetails, playerBboardDetails))
			{
				delete bonus; // Important: Don't touch and don't change the order of statements [Mordehai]
				cout << "Player A won" << endl;
				PrintPoints(playerAboardDetails, playerBboardDetails);

				GameBoardUtils::DeleteBoard(mainGameBoard);
				FreeGlobalVariable();
				return 0;
			}
			if (IsPlayerWon(PlayerBID, playerAboardDetails, playerBboardDetails))
			{
				delete bonus; // Important: Don't touch and don't change the order of statements [Mordehai]
				cout << "Player B won" << endl;
				PrintPoints(playerAboardDetails, playerBboardDetails);

				GameBoardUtils::DeleteBoard(mainGameBoard);
				FreeGlobalVariable();
				return 0;
			}
		}
	}

	delete bonus; // Important: Don't touch and don't change the order of statements [Mordehai]
	PrintPoints(playerAboardDetails, playerBboardDetails);

	FreeGlobalVariable();
	GameBoardUtils::DeleteBoard(mainGameBoard);

	return 0;
}
