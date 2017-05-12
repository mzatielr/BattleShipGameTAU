#pragma once
#include "../Common/GameBoardUtils.h"
#include "IBattleshipGameAlgo.h"

enum class Direction { Right, Left, Up, Down, All };

/*
* a utility class for keeping track of game state
*/
class ShipDetailsBoard
{
	GameBoardUtils _utils;
public:
	int playerID;
	char** mainboard;

	int RubberBoatCells;
	int RocketShipCells;
	int SubmarineCells;
	int DestroyeCells;

	int negativeScore; // Save the score of the other player
	ShipDetailsBoard(char** board, int playerID);
	AttackResult GetAttackResult(pair<int, int> attack);
	bool IsLoose() const;
	bool IsSinkRecursiveChecker(int row, int col, Direction dir = Direction::All) const;
 n};