#pragma once

#include "IBattleshipGameAlgo.h"

/*
* a class that represents a player at the game
* holds the current information of the player while playing the game
*/
class NaiveBattleshipGameAlgo : public IBattleshipGameAlgo 
{
public:
	void setBoard(int player,const char** board, int numRows, int numCols) override; // called once to notify player on his board
	std::pair<int, int> attack() override; // ask player for his move
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	~NaiveBattleshipGameAlgo();
	bool init(const std::string& path) override;
private:

	// Number rows - set in SetBoard func
	int m_NumRow; 

	// Number cols - set in SetBoard func
	int m_NumCol;

	int m_PlayerNum; //My player number

	//boolean value setting if i finished my attacks
	bool m_AttacksDone;
	
	//my board game - should contain only my ships
	char** m_Board;
	
	//a board for marking where cant we attack
	char** m_CannotAttackBoard;
	
	//holder of current attack spot
	std::pair<int, int> m_CurrentAttack;
};