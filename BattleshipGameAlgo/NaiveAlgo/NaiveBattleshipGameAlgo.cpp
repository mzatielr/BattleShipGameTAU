// BattleshipGameAlgo.cpp : Defines the entry point for the console application.

#include "NaiveBattleshipGameAlgo.h"
#include <utility>
#include "../Common/GameBoardUtils.h"

/*
 * \brief Find next cell to attck 
 * \return (-1,-1) in case of EOF. (-2, -2) in case of any failure - should return values between 1-10 and not 0-9 (EX2 specification)
 */
pair<int, int> NaiveBattleshipGameAlgo::attack()
{
	if (m_AttacksDone)
	{
		return pair<int, int>{AttckDoneIndex, AttckDoneIndex};
	}

	//convert back to our matrix coordinates
	int i = m_CurrentAttack.first - 1;
	int j = m_CurrentAttack.second - 1;

	//find next attack
	while (i < m_NumRow)
	{
		while (j < m_NumCol)
		{
			if (m_CannotAttackBoard[i][j] == CanAttck)
			{
				//set cannot attack this spot again
				m_CannotAttackBoard[i][j] = CannotAttck;
				int returnattacki = i + 1;
				int returnattackj = j + 1;

				//set the next index to search from
				if (j == m_NumCol - 1)
				{
					m_CurrentAttack.first = i + 2;
					m_CurrentAttack.second = 1;
				}
				else 
				{
					m_CurrentAttack.first = i + 1;
					m_CurrentAttack.second = j + 2;
				}
				return pair<int, int>{returnattacki, returnattackj};
			}
			j++;
		}
		j = 0;
		i++;
	}

	//end of attacks
	m_AttacksDone = true;
	return pair<int, int>{AttckDoneIndex, AttckDoneIndex};
}

/*
this function is called at startup to update each players board game
*/
void NaiveBattleshipGameAlgo::setBoard(int player, const char** board, int numRows, int numCols)
{
	m_NumCol = numCols;
	m_NumRow = numRows;
	m_PlayerNum = player;

	//setting current attack to (1,1)
	m_CurrentAttack.first = 1;
	m_CurrentAttack.second = 1;

	m_Board = GameBoardUtils::InitializeNewEmptyBoard();
	m_CannotAttackBoard = GameBoardUtils::InitializeNewEmptyBoard();

	// Init all cells in m_CannotAttackBoard matrix with CanAttack Char
	GameBoardUtils::InitBoard(m_CannotAttackBoard, m_NumRow, m_NumCol, CanAttck);

	// Init current player board with its board
	GameBoardUtils::CloneBoardToPlayer(board, m_PlayerNum, m_Board);

	//prepering a matrix to know where not to attack
	GameBoardUtils::MarkCannotAttack(m_CannotAttackBoard, m_PlayerNum, m_Board, m_NumRow, m_NumCol);
}

/*
this function only updates board of player & adds score if needed.
checking of if the game is ended will be at main function
*/
void NaiveBattleshipGameAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	// Do nothing - Naive algo use only data from the begining and don't infulended from any other data
}

/*
 * destructor
 */
NaiveBattleshipGameAlgo::~NaiveBattleshipGameAlgo()
{
	GameBoardUtils::DeleteBoard(m_Board);
	GameBoardUtils::DeleteBoard(m_CannotAttackBoard);
}

// Init Algorithm - Do nothing
bool NaiveBattleshipGameAlgo::init(const std::string& path)
{
	return true; //return true always because doesnt need to init nothing
}


/**
 * \brief Return instance of NaiveBattleshipGameAlgo
 * \return NaiveBattleshipGameAlgo initalized object - used for loading DLL 
 */
IBattleshipGameAlgo* GetAlgorithm()
{
	return new NaiveBattleshipGameAlgo();
}



