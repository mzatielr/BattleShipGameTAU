// BattleshipGameAlgo.cpp : Defines the entry point for the console application.
//

#include "BattleshipGameAlgo.h"
#include <utility>
#include "../Common/GameBoardUtils.h"
#include "AttackReciever.h"


PreDefinedBattleshipGameAlgo::PreDefinedBattleshipGameAlgo()
	: m_NumRow(0), m_NumCol(0), m_myPlayerNum(0), m_attacksDone(false), m_board(nullptr), m_attackReceiver(nullptr)
{
}


/*
* \brief Find next cell to attck 
* \return (-1,-1) in case of EOF. (-2, -2) in case of any failure - should return values between 1-10 and not 0-9 (EX2 specification)
*/
std::pair<int,int> PreDefinedBattleshipGameAlgo::attack()
{
	if(m_attacksDone)
	{
		return pair<int, int>{AttckDoneIndex, AttckDoneIndex};
	}

	pair<int, int> attack = m_attackReceiver->GetNextLegalAttack();
	if(attack.first == AttckDoneIndex || attack.first == ErrorDuringGetAttackIndex)
	{
		m_attacksDone = true;
		m_attackReceiver->Dispose();
	}
	return attack;
}

/*
this function is called at startup to update each players board game
*/
void PreDefinedBattleshipGameAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	m_NumRow = numRows;
	m_NumCol = numCols;
	m_myPlayerNum = player;

	m_board = GameBoardUtils::InitializeNewEmptyBoard();
	GameBoardUtils::CloneBoardToPlayer(board, m_myPlayerNum, m_board);
}

/*
this function only updates board of player & adds score if needed.
checking of if the game is ended will be at main function
*/
void PreDefinedBattleshipGameAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}

PreDefinedBattleshipGameAlgo::~PreDefinedBattleshipGameAlgo()
{
	GameBoardUtils::DeleteBoard(m_board);
	delete m_attackReceiver;
}

bool PreDefinedBattleshipGameAlgo::init(const std::string& path)
{
	m_attackReceiver = new AttackReciever(path); //TODO: need to init the attack file here
	
	//Todo -  Change Return value - can be also false, after changing the above remark
	return true;
}


/**
* \brief Return instance of PreDefinedBattleshipGameAlgo
* \return PreDefinedBattleshipGameAlgo initalized object - used for loading DLL
*/
IBattleshipGameAlgo* GetAlgorithm()
{
	return new PreDefinedBattleshipGameAlgo();
}

