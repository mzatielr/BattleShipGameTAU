#pragma once
#include "IBattleshipGameAlgo.h"
#include "IOLib.h"
#include "Windows.h"

using namespace std;

extern Logger MainLogger;

typedef IBattleshipGameAlgo *(*GetAlgorithmFuncType)();
extern GetAlgorithmFuncType getPlayerAlgo;

class DllAlgo
{
public:
	DllAlgo();
	IBattleshipGameAlgo* algo;
	bool LoadDll(const string& dllPath);
	void Dispose() const;
private:
	string path;
	pair <HINSTANCE, GetAlgorithmFuncType> handle;
};
