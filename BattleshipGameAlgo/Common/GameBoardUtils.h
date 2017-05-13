#pragma once

#include "Contants.h"
#include <thread>
#include "IOLib.h"

using namespace std;

/*
* a utility class for performing static functions at main function
*/
class GameBoardUtils {
	static BoardFileErrorCode ValidateGameBoard(char** board, int rows, int cols);
public:
	static void InitBoard(char** board, int rows, int cols, char InitChar);
	static bool IsPlayerIdChar(int playerID, char current);
	static int GetCharPlayerId(char current);
	static bool IsLegalBoradChar(char current);
	static void LoadLineToBoard(char** board, int row, int cols, const string& cs);
	static char** InitializeNewEmptyBoard(int rows, int cols);
	static void DeleteBoard(char** board, int rows);
	static BoardFileErrorCode LoadBoardFromFile(char** board, int rows, int cols, const string& filePath);
	static void PrintBoard(ostream& stream, char** board, int rows, int cols);
	static void CloneBoardToPlayer(const char** full_board, int playerID, char** player_board, int rows, int cols);
	static void CloneBoard(char** full_board, char** player_board, int rows, int cols);
	static void MarkCannotAttack(char** markBoard, int playernum, const char** mainBoard, int rows, int cols );
	static string GetFilePathBySuffix(int argc, string customPath, string filesuffix, bool direxists = true);
	static bool DirExists(const std::string& dirName_in);
	static char** ClonePlayerBoard(const char** fullBoard, int i, int rows, int cols);
	static void ChangeFontSize();
	static void InitLogger(Logger& logger,string logpath);
};