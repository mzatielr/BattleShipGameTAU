#pragma once

#include <fstream>
#include <queue>
#include "IOLib.h"

using namespace std;

extern Logger MainLogger;

/*
* a utility class for handeling all attack functionalities
*/
class AttackReciever {
	int m_Rows;
	int m_Cols;
	string path;
	ifstream _file;
	queue<string> fileRowsQueue;

	void Dispose();
public:
	AttackReciever();
	bool Init(const string& attackPath, int rows, int cols);
	int ReadNextAttack(std::pair<int, int>& pair);
	pair<int, int> GetNextLegalAttack();
	int ConvertStringToIntSafely(string& line, int& number) const;
};