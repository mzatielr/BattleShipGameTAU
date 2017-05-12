#pragma once
#include "IArgumentParser.h"
#include "DllAlgo.h"

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
public:
	GameManager(Configuration& config);

	int RunGame();
};
