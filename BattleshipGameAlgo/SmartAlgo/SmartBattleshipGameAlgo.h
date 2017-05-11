#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <tuple>

using namespace std;

enum class AttackMode {RandomMode , TargetMode };
enum class AttackDir {Vertical, Horizontal, Unknown};

/*
* a class that represents a player at the game
* holds the current information of the player while playing the game
*/
class SmartBattleshipGameAlgo : public IBattleshipGameAlgo
{
public:
	//IBattleshipGameAlgo	
	void setBoard(int player, const char** board, int numRows, int numCols) override; // called once to notify player on his board
	pair<int, int> attack() override; // ask player for his move
	void AddSqureCellsToQueue(int row, int col);
	void AddPotentialAttckIfLegal(int row, int col, AttackDir dir);
	void HandleMyRandomMode(int row, int col, AttackResult result);
	tuple<int, int, AttackDir> Dequeue(int row, int col);
	void AddToQueueAfterHit(int row, int col);
	void HandleMyTargetMode(int row, int col, AttackResult result);
	void HandleMyAttackResult(int row, int col, AttackResult result);
	void HandleRivalAttackResult(int row, int col, AttackResult result) const;
	void MarkInvalidCell(int row, int col, AttackResult result) const;
	void MarkSinkBattleAroundAsInvlid(int row, int col) const;
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
	~SmartBattleshipGameAlgo();
	bool init(const std::string& path) override;
private:
	// Number rows - set in SetBoard func
	int m_NumRow;

	// Number cols - set in SetBoard func
	int m_NumCol;
	
	// Attack Mode
	AttackMode m_mode;

	// Current Direction attack
	AttackDir m_CurrentDir;

	//My player number
	int m_myPlayerNum;
	
	//my board game
	char** m_board;
	
	//a board for marking where cant we attack
	char** m_cannotAttackBoard;
	
	//Holds the valid attack for generating a random attack
	vector<pair<int, int>> m_attacksRemain;

	// Holds potential attacks after mode is Target <row,col,dir(Vertical/Horizontal)>
	vector<tuple<int, int, AttackDir>> m_PotentialAttacks;

	static int GetRandom(size_t maxNumber);

	void StartRandomAttackMode();

	bool IsAttackValid(int row, int col) const;
};