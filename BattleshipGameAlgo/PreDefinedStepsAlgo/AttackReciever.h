#pragma once

#include <fstream>
#include <queue>
using namespace std;

/*
* a utility class for handeling all attack functionalities
*/
class AttackReciever {
	string path;
	ifstream _file;
	queue<string> fileRowsQueue;

	void Dispose();
public:
	AttackReciever(const string& attackPath);
	bool Init();
	int ReadNextAttack(std::pair<int, int>& pair);
	pair<int, int> GetNextLegalAttack();
	int ConvertStringToIntSafely(string& line, int& number) const;
};