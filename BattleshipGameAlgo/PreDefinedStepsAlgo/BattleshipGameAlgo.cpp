// BattleshipGameAlgo.cpp : Defines the entry point for the console application.
//

#include "BattleshipGameAlgo.h"
#include <utility>
#include "../Common/GameBoardUtils.h"
#include "AttackReciever.h"
#include "AttackFileUtilis.h"

Logger MainLogger;

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

	char name[100];
	sprintf_s(name, "PreDefinedBattleshipGameAlgo%d.log", m_myPlayerNum);
	MainLogger.InitLogger(name);

	m_board = GameBoardUtils::InitializeNewEmptyBoard();
	GameBoardUtils::CloneBoardToPlayer(board, m_myPlayerNum, m_board);
}

/*
this function only updates board of player & adds score if needed.
checking of if the game is ended will be at main function
*/
void PreDefinedBattleshipGameAlgo::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	//Do nothing
}

PreDefinedBattleshipGameAlgo::~PreDefinedBattleshipGameAlgo()
{
	GameBoardUtils::DeleteBoard(m_board);
	delete m_attackReceiver;

	MainLogger.logFile << "Disposing object" << endl;
	MainLogger.LoggerDispose();
}

bool PreDefinedBattleshipGameAlgo::init(const std::string& path)
{
	string attackFilePath = AttackFileUtils::GetAttackFile(path, m_myPlayerNum);
	if(attackFilePath.empty())
	{
		MainLogger.logFile << "[PreDefinedAlgo - Init] Init failed due to non found attack file" << endl;;
		return false;
	}
	MainLogger.logFile << "[PreDefinedAlgo - Init] Attack file for player " << m_myPlayerNum << " is " << attackFilePath << endl;
	
	m_attackReceiver = new AttackReciever(attackFilePath);
	return m_attackReceiver->Init();
}


/**
* \brief Return instance of PreDefinedBattleshipGameAlgo
* \return PreDefinedBattleshipGameAlgo initalized object - used for loading DLL
*/
IBattleshipGameAlgo* GetAlgorithm()
{
	return new PreDefinedBattleshipGameAlgo();
}

