#include "AttackFileUtilis.h"
#include "../Common/IFileDirectoryUtils.h"

string AttackFileUtils::GetAttackFile(const string & path, int playerID)
{
	vector<string> collection;
	int cnt = IFileDirectoryUtils::GetAllFiles(path, "*.attack", collection);
	switch (cnt)
	{
	case 0: return string();
	case 1: return collection[0];
	default:
		return collection[playerID];
	}
}
