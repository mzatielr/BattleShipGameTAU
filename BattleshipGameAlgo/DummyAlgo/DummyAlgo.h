#pragma once
#include "IBattleshipGameAlgo.h"

class DummyAlgo : public IBattleshipGameAlgo
{
	void setBoard(int player, const char** board, int numRows, int numCols) override; // called once to notify player on his board
	std::pair<int, int> attack() override; // ask player for his move
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	~DummyAlgo();
	bool init(const std::string& path) override;
};
