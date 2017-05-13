#pragma once
#include <windows.h>
#include <wincon.h>
#include "IOLib.h"

enum class RetVal { Success, UnknownError };

typedef struct BonusParams
{
	bool isQuiet = false;
	int delayInMiliseconds = 1500;
}BounsParams;

class Bonus
{
	const WORD firstPlayerColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	const WORD secondPlayerColor = FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	const WORD blankPlayerColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;

	bool enable;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int waitInMiliseconds;
	int absX;
	int absY;

	void Wait() const;
	static void* GetstdOutHandle();
	RetVal StoreCurrentConsoleState();
	RetVal gotoxy(int x, int y, bool isAbsVal = false) const;
	RetVal SetTextColor(int playerID) const;
	RetVal RestoreConsoleState() const;
	RetVal StoreCurrentCoord();

public:	
	Bonus(bool enable, int waitTimeout = 1500);

	static void ShowConsoleCursor(bool b);
	void Init(char** board, int rows, int cols);
	void PrintPlayerChar(char letter, short x, short y, int playerID, bool isWaitAfterPrint = true) const;
	void Dispose() const;
};
