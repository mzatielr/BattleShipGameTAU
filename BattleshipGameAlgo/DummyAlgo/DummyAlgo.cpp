
#include "DummyAlgo.h"



/**
* \brief Return instance of NaiveBattleshipGameAlgo
* \return NaiveBattleshipGameAlgo initalized object - used for loading DLL
*/

IBattleshipGameAlgo* GetAlgorithm()
{
	return new DummyAlgo();
}

void DummyAlgo::setBoard(int player, const char ** board, int numRows, int numCols)
{
}

std::pair<int, int> DummyAlgo::attack()
{
	return{ -1, -1 };
}

void DummyAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}

DummyAlgo::~DummyAlgo()
{
}

bool DummyAlgo::init(const std::string& path)
{
	return true;
}
