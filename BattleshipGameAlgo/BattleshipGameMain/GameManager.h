#pragma once
#include "IArgumentParser.h"
#include "DllAlgo.h"
#include "ShipDetailsBoard.h"

extern Logger MainLogger;

class GameManager
{
	Configuration& config;
	pair<string, string> dllPaths;
	DllAlgo algo1;
	DllAlgo algo2;

	// board - will save updated and full board of two players
	char** mainGameBoard;

	bool ConfigureDll();
	bool ConfigurePath() const;
	bool ConfigureBoard() ;
	bool ConfigureFiles();
	bool InitPlayers();
	bool InitDllAlgo(DllAlgo& algo, const string& path, int playerID) const;

	int GameInitializer();
	static pair<int, int> GetNextPlayerAttack(int player_id, IBattleshipGameAlgo* player_a, IBattleshipGameAlgo* player_b);
	static AttackResult GetAttackResult(const pair<int, int>& pair, char** board, ShipDetailsBoard& detailsA, ShipDetailsBoard& detailsB);
	static void PrintPoints(ShipDetailsBoard& playerA, ShipDetailsBoard& playerB);
	static void PrintSinkCharRec(char** maingameboard, Bonus& b, int i, int j, int player);
	static bool IsPlayerWon(int currentPlayer, ShipDetailsBoard& detailsA, ShipDetailsBoard& detailsB);
	static bool ValidAttackCor(const pair<int, int>& pair);
	int PlayGame();

	void GameManagerCleanup() const;
public:
	GameManager(Configuration& config);

	int RunGame();
	void Test_GetAllAttacks() const;
};
