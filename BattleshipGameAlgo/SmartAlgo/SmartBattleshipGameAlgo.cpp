#include "SmartBattleshipGameAlgo.h"
#include "../Common/GameBoardUtils.h"
#include "../Common/Contants.h"
#include <cassert>

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

void SmartBattleshipGameAlgo::AddSqureCellsToQueue(int row, int col)
{
	// Add above me cell
	if(row > 0)
	{
		AddPotentialAttckIfLegal(row - 1, col, AttackDir::Vertical);
	}

	// Add below me cell
	if(row  < m_NumRow -1)
	{
		AddPotentialAttckIfLegal(row + 1, col, AttackDir::Vertical);
	}

	// Add left cell
	if(col > 0)
	{
		AddPotentialAttckIfLegal(row, col - 1, AttackDir::Horizontal);
	}

	// Add right cell
	if(col < m_NumCol -1 )
	{
		AddPotentialAttckIfLegal(row, col + 1, AttackDir::Horizontal);
	}
}

void SmartBattleshipGameAlgo::AddPotentialAttckIfLegal(int row, int col, AttackDir dir)
{
	if(IsAttackValid(row, col))
	{
		m_PotentialAttacks.push_back(make_tuple(row, col, dir));
	}
}

void SmartBattleshipGameAlgo::HandleMyRandomMode(int row, int col, AttackResult result)
{
	switch (result)
	{
	case AttackResult::Miss:
		// Continue as random mode
		StartRandomAttackMode(); 
		break;
	case AttackResult::Hit:
		// Switch to Target mode
		m_mode = AttackMode::TargetMode;

		// On first hit battleship direcion in unknown
		m_CurrentDir = AttackDir::Unknown;

		// Add all around legal cell to queue
		AddSqureCellsToQueue(row, col);
		break;
	case AttackResult::Sink:
		// Continue as random mode
		StartRandomAttackMode();
		break;
	}
}

void SmartBattleshipGameAlgo::HandleMyTargetMode(int row, int col, AttackResult result)
{
}

void SmartBattleshipGameAlgo::HandleMyAttackResult(int row, int col, AttackResult result)
{
	switch (m_mode)
	{
	case AttackMode::RandomMode:
		HandleMyRandomMode(row, col, result);
		break;
	case AttackMode::TargetMode:
		HandleMyTargetMode(row, col, result);
		break;
	}
}

void SmartBattleshipGameAlgo::HandleRivalAttackResult(int row, int col, AttackResult result)
{

}

void SmartBattleshipGameAlgo::MarkInvalidCell(int row, int col, AttackResult result) const
{
	char markValue = m_cannotAttackBoard[row][col];
	switch (result)
	{
	case AttackResult::Miss:
		markValue = CannotAttck;
		break;
	case AttackResult::Hit:
		markValue = CannotAttackShip;
		break;
	case AttackResult::Sink:
		markValue = CannotAttackShip;
		MarkSinkBattleAroundAsInvlid(row, col);
		break;
	}

	m_cannotAttackBoard[row][col] = markValue;
}

void SmartBattleshipGameAlgo::MarkSinkBattleAroundAsInvlid(int row, int col) const
{
	if (row < 0 || row >= m_NumRow || col < 0 || col >= m_NumCol)
		return;

	char curCell = m_cannotAttackBoard[row][col];
	m_cannotAttackBoard[row][col] = CannotAttck;

	if (curCell == CannotAttackShip)
	{
		MarkSinkBattleAroundAsInvlid(row - 1, col);
		MarkSinkBattleAroundAsInvlid(row + 1, col);
		MarkSinkBattleAroundAsInvlid(row, col - 1);
		MarkSinkBattleAroundAsInvlid(row, col + 1);
	}
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
	MarkInvalidCell(row, col, result);

	if(player == m_myPlayerNum)
	{
		HandleMyAttackResult(row, col, result);
	}
	else
	{
		HandleRivalAttackResult(row, col, result);
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

void SmartBattleshipGameAlgo::StartRandomAttackMode()
{
	m_mode = AttackMode::RandomMode;
	m_CurrentDir = AttackDir::Unknown;

	// Empty potentail attack collection
	m_PotentialAttacks.clear();
}

bool SmartBattleshipGameAlgo::IsAttackValid(int row, int col) const
{
	char value = m_cannotAttackBoard[row][col];
	return value != CannotAttck && value != CannotAttackShip;
}

/**
* \brief Return instance of SmartBattleshipGameAlgo
* \return SmartBattleshipGameAlgo initalized object - used for loading DLL
*/
IBattleshipGameAlgo* GetAlgorithm()
{
	return (new SmartBattleshipGameAlgo());
}
