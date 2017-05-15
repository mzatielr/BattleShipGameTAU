#pragma once
#include "Bonus.h"

using namespace std;
class Configuration
{
public:
	string path;
	BonusParams bonusParam;
};

class IArgumentParser
{
public :
	static void ParseArguments(Configuration& config, int argc, char* argv[]);
};