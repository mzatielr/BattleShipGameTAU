#include "SmartBattleshipGameAlgo.h"
#include "../Common/GameBoardUtils.h"
#include "../Common/Contants.h"
#include <cassert>

pair<int, int> SmartBattleshipGameAlgo::attack()
{
	if (m_mode == AttackMode::RandomMode)
	{
		return GetValidRandomAttack();
	}
	
	// Target Mode
	pair<int, int> attackPair = GetValidOptionalAttack();
	if(attackPair.first == AttckDoneIndex)
	{
		// Return to random state
		StartRandomAttackMode();
		return GetValidRandomAttack();
	}

	return attackPair;
}

pair<int,int> SmartBattleshipGameAlgo::GetValidRandomAttack ()
{
	while (m_attacksRemain.size() > 0)
	{
		int randomLocation = GetRandom(m_attacksRemain.size());
		pair<int, int> currentAttack = m_attacksRemain[randomLocation];

		if (!IsAttackValid(currentAttack.first, currentAttack.second))
		{
			m_attacksRemain.erase(m_attacksRemain.begin() + randomLocation);
		}
		else
		{
			return currentAttack;
		}
	}
	return{ AttckDoneIndex, AttckDoneIndex };
}

pair<int, int> SmartBattleshipGameAlgo::GetValidOptionalAttack()
{
	bool isCheckDir = m_CurrentDir != AttackDir::Unknown;

	while (m_PotentialAttacks.size() > 0)
	{
		int randomLocation = GetRandom(m_PotentialAttacks.size());
		tuple<int, int, AttackDir> currentAttack = m_PotentialAttacks[randomLocation];

		if (!IsAttackValid(get<0>(currentAttack), get<1>(currentAttack)))
		{
			m_PotentialAttacks.erase(m_PotentialAttacks.begin() + randomLocation);
		}

		else if (isCheckDir && get<2>(currentAttack) != m_CurrentDir)
		{
			m_PotentialAttacks.erase(m_PotentialAttacks.begin() + randomLocation);
		}

		else
		{
			return { get<0>(currentAttack) , get<1>(currentAttack) };
		}
	}
	return{ AttckDoneIndex, AttckDoneIndex };
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

tuple<int, int, AttackDir> SmartBattleshipGameAlgo::Dequeue(int row, int col)
{
	for (auto itr = m_PotentialAttacks.begin(); itr != m_PotentialAttacks.end(); ++itr)
	{
		int y = get<0>(*itr);
		int x = get<1>(*itr);

		if (y == row && x == col)
		{
			tuple<int, int, AttackDir> retValue = *itr;
			m_PotentialAttacks.erase(itr);
			return retValue;
		}
	}
	return{ 0,0,AttackDir::Unknown };
}

void SmartBattleshipGameAlgo::AddToQueueAfterHit(int row, int col)
{
	if(m_CurrentDir == AttackDir::Vertical)
	{
		// Add above
		if(row > 0)
		{
			AddPotentialAttckIfLegal(row - 1, col, AttackDir::Vertical);
		}

		// Add below
		if (row < m_NumRow - 1)
		{
			AddPotentialAttckIfLegal(row + 1, col, AttackDir::Vertical);
		}
	}

	else if(m_CurrentDir == AttackDir::Horizontal)
	{
		// Add left
		if(col > 0)
		{
			AddPotentialAttckIfLegal(row, col - 1, AttackDir::Horizontal);
		}

		// Add right
		if(col < m_NumCol -1 )
		{
			AddPotentialAttckIfLegal(row, col + 1, AttackDir::Horizontal);
		}
	}
}

void SmartBattleshipGameAlgo::HandleMyTargetMode(int row, int col, AttackResult result)
{
	switch (result)
	{
	case AttackResult::Miss:
		Dequeue(row, col);
		break;
	case AttackResult::Hit:
	{
		tuple<int, int, AttackDir> val = Dequeue(row, col);

		if (m_CurrentDir == AttackDir::Unknown)
		{
			m_CurrentDir = get<2>(val);
		}
		AddToQueueAfterHit(row, col);
		break;
	}
	case AttackResult::Sink:
		// Back to random state
		StartRandomAttackMode();
		break;
	}
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

void SmartBattleshipGameAlgo::HandleRivalAttackResult(int row, int col, AttackResult result) const
{
	if(result == AttackResult::Miss)
	{
		m_cannotAttackBoard[row][col] = CannotAttck;
	}
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

	m_cannotAttackBoard = GameBoardUtils::InitializeNewEmptyBoard(numRows, numCols);
	GameBoardUtils::InitBoard(m_cannotAttackBoard, numRows, numCols, CanAttck);
	
	m_board = GameBoardUtils::InitializeNewEmptyBoard(numRows,numCols);
	GameBoardUtils::CloneBoardToPlayer(board, m_myPlayerNum, m_board, numRows, numCols);

	//prepering a matrix to know where not to attack
	GameBoardUtils::MarkCannotAttack(m_cannotAttackBoard, m_myPlayerNum, board, numRows, numCols);
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

	if(player == m_myPlayerNum)
	{
		MarkInvalidCell(row, col, result);
		HandleMyAttackResult(row, col, result);
	}
	else
	{
		HandleRivalAttackResult(row, col, result);
	}
}

SmartBattleshipGameAlgo::~SmartBattleshipGameAlgo()
{
	GameBoardUtils::DeleteBoard(m_board,m_NumRow);
	GameBoardUtils::DeleteBoard(m_cannotAttackBoard,m_NumRow);
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
