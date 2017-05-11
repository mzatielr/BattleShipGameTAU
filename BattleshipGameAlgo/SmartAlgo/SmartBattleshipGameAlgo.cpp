#include "SmartBattleshipGameAlgo.h"
#include "../Common/GameBoardUtils.h"
#include "../Common/Contants.h"

pair<int, int> SmartBattleshipGameAlgo::attack()
{
	if (m_mode == AttackMode::RandomMode)
	{
		int randomLocation = GetRandom(m_attacksRemain.size());
		pair<int, int> currentAttack = m_attacksRemain[randomLocation];

		m_attacksRemain.erase(m_attacksRemain.begin() + randomLocation);
		return currentAttack;
	}


	return pair<int, int>{AttckDoneIndex, AttckDoneIndex};
}

/*
this function is called at startup to update each players board game
*/
void SmartBattleshipGameAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	m_NumRow = numRows;
	m_NumCol = numCols;
	m_myPlayerNum = player;
	m_mode = AttackMode::RandomMode; // Starting from random mode

	m_cannotAttackBoard = GameBoardUtils::InitializeNewEmptyBoard();
	GameBoardUtils::InitBoard(m_cannotAttackBoard, numRows, numCols, CanAttck);
	
	m_board = GameBoardUtils::InitializeNewEmptyBoard();
	GameBoardUtils::CloneBoardToPlayer(board, m_myPlayerNum, m_board);

	//prepering a matrix to know where not to attack
	GameBoardUtils::MarkCannotAttack(m_cannotAttackBoard, m_myPlayerNum, m_board);
	for (int i = 0; i < m_NumRow; i++)
	{
		for (int j = 0; j < m_NumCol; j++)
		{
			if (m_cannotAttackBoard[i][j] == CanAttck) //if we can attack at that spot
			{
				m_attacksRemain.push_back(pair<int, int>(i, j));
			}
		}
	}
}

void SmartBattleshipGameAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if(m_myPlayerNum == player)
	{
		if (m_mode == AttackMode::RandomMode)
		{
			if(result == AttackResult::Hit)
			{
				m_mode = AttackMode::RandomMode;
			}
			//else - sink || Miss - stay random
			else
			{
				
			}
		}
		else
		{
			if(result == AttackResult::Hit)
			{
				
			}
		}
	}
	else
	{
		
	}
}

SmartBattleshipGameAlgo::~SmartBattleshipGameAlgo()
{
	GameBoardUtils::DeleteBoard(m_board);
	GameBoardUtils::DeleteBoard(m_cannotAttackBoard);
}

// Don't do  nothing.
bool SmartBattleshipGameAlgo::init(const string& path)
{
	return true; 
}

// Returns random number between 0 to max number - 1
int SmartBattleshipGameAlgo::GetRandom(size_t maxNumber)
{
	return rand() % maxNumber;
}

/**
* \brief Return instance of SmartBattleshipGameAlgo
* \return SmartBattleshipGameAlgo initalized object - used for loading DLL
*/
IBattleshipGameAlgo* GetAlgorithm()
{
	return (new SmartBattleshipGameAlgo());
}
