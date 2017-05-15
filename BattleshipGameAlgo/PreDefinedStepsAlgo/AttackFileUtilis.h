#pragma once
#include "IOLib.h"

using namespace std;
extern Logger MainLogger;

class AttackFileUtils
{
private:
public:
	static string GetAttackFile(const string& path, int playerID);
};
