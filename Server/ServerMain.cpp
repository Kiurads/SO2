#include "ServerHeader.h"

HKEY hRegKey; //Top 10 stored in registry
player tpTopTen[TOP], newUser; //Top 10 usernames
HANDLE hBallThread;
HANDLE hGameThread;
HANDLE hBallTimer;
DWORD dwGameThreadId;
DWORD dwBallThreadId;
DWORD dwResult;
DWORD dwSize;
game gameData;
int termina;

int _tmain(int argc, LPTSTR argv) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	if (setupServer() == -1) {
		_tprintf(TEXT("Não foi possível criar o pipe do Servidor\n"));
		_gettchar();
		exit(-1);
	}
	hBallThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BallThread, NULL, NULL, &dwBallThreadId);


	if (setupRegisty() == -1) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		exit(-1);
	}

	do {
		if (getLogin() == -1)
			_tprintf(TEXT("Login inválido por parte de um Cliente\n"));
		else
			break;
	} while (TRUE);

	hGameThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GameThread, NULL, NULL, &dwGameThreadId);

	_gettchar();

	termina = 1;

	WaitForSingleObject(hBallThread, INFINITE);
	WaitForSingleObject(hGameThread, INFINITE);
	UnmapViewOfFile(lpLoginBuffer);
	UnmapViewOfFile(gMappedGame);
	CloseHandle(hGameMapFile);
	CloseHandle(hReadEvent);
	CloseHandle(hLoginMutex);
	CloseHandle(hLoginEvent);
	CloseHandle(hLoggedEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hHasReadEvent);
	CloseHandle(hBallTimer);

	return 0;
}

int setupServer() {
	hLoginMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_MAX_SIZE, LOGIN_FILE_NAME);
	lpLoginBuffer = (TCHAR(*)[BUFFER_MAX_SIZE])MapViewOfFile(hLoginMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_MAX_SIZE);
	hLoginMutex = CreateMutex(NULL, FALSE, LOGIN_MUTEX_NAME);
	hLoginEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_MAX_SIZE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, GAME_READ_EVENT);
	hHasReadEvent = CreateEvent(NULL, FALSE, FALSE, GAME_HAS_READ_EVENT);

	hBallTimer = CreateWaitableTimer(NULL, TRUE, TEXT("BallTimer"));

	gameData.gameBall.x = 0;
	gameData.gameBall.y = 0;
	gameData.gameBall.speed = 1;

	gameData.gameBar.pos = MAX_X / 2;

	termina = 0;

	return 0;
}

int getLogin() {
	WaitForSingleObject(hLoginEvent, INFINITE);

	_tcscpy(newUser.tUsername, (*lpLoginBuffer));

	newUser.hiScore = 0;

	SetEvent(hLoggedEvent);

	return 0;
}

int setupRegisty() {

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Breakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwResult) != ERROR_SUCCESS) {
		return -1;
	}
	if (dwResult == REG_CREATED_NEW_KEY) {
		_tprintf(TEXT("Key: HKEY_CURRENT_USER\\Software\\Breakout created\n\n"));
		_tprintf(TEXT("TOP 10\n\n"));

		for (int i = 0; i < TOP; i++) {
			TCHAR tPoints[TAM];

			_tprintf(TEXT("Username %d: "), i + 1);
			_tscanf(TEXT("%49s"), tpTopTen[i].tUsername);

			_tprintf(TEXT("Hi-Score: "));
			_tscanf(TEXT("%d"), &tpTopTen[i].hiScore);

			_stprintf(tPoints, TEXT("%d"), tpTopTen[i].hiScore);

			RegSetValueEx(hRegKey, tpTopTen[i].tUsername, 0, REG_SZ, (LPBYTE)tPoints, (DWORD)_tcslen(tPoints) * sizeof(TCHAR));

			fflush(stdin);
		}

		qsort(tpTopTen, TOP, sizeof(player), cmpfunc);

		_tprintf(TEXT("\n"));

		for (int i = 0; i < TOP; i++) {
			int position;
			TCHAR tPosition[3];

			position = TOP - i;

			_stprintf(tPosition, TEXT("%d"), position);

			_tprintf(TEXT("%s: %d\n"), tpTopTen[i].tUsername, tpTopTen[i].hiScore);

			//Register positions of the users
			RegSetValueEx(hRegKey, tPosition, 0, REG_SZ, (LPBYTE)tpTopTen[i].tUsername, (DWORD)_tcslen(tpTopTen[i].tUsername) * sizeof(TCHAR));
		}
	}
	else {
		_tprintf(TEXT("Key: HKEY_CURRENT_USER\\Software\\Breakout opened\n\n"));

		for (int i = 0; i < TOP; i++) {
			int position;
			TCHAR tPosition[3];
			TCHAR tPoints[TOP];

			position = i + 1;

			_stprintf(tPosition, TEXT("%d"), position);

			dwSize = TAM;
			memset(tpTopTen[i].tUsername, '\0', dwSize);

			//Read usernames from registry
			RegQueryValueEx(hRegKey, tPosition, NULL, NULL, (LPBYTE)tpTopTen[i].tUsername, &dwSize);

			dwSize = TOP;
			memset(tPoints, '\0', TOP);

			//Read hiScore from registry
			RegQueryValueEx(hRegKey, tpTopTen[i].tUsername, NULL, NULL, (LPBYTE)tPoints, &dwSize);

			tpTopTen[i].hiScore = _ttoi(tPoints);
		}

		for (int i = 0; i < TOP; i++)
			_tprintf(TEXT("%d - %s: %d\n"), i + 1, tpTopTen[i].tUsername, tpTopTen[i].hiScore);
	}

	RegCloseKey(hRegKey);

	return 0;
}

int cmpfunc(const void * a, const void * b) {
	pPlayer x = (pPlayer)a;
	pPlayer y = (pPlayer)b;

	return (x->hiScore - y->hiScore);
}

DWORD WINAPI BallThread(LPVOID lpArg) {
	UNREFERENCED_PARAMETER(lpArg);

	LARGE_INTEGER li;

	li.QuadPart = -10000000LL;

	SetWaitableTimer(hBallTimer, &li, 0, NULL, NULL, 0);

	int x = 1;
	int y = 1;

	while (!termina) {
		WaitForSingleObject(hBallTimer, INFINITE);

		gameData.gameBall.x += x;
		gameData.gameBall.y += y;

		if (gameData.gameBall.x == MAX_X || gameData.gameBall.x == 0) x = x * (-1);
		if (gameData.gameBall.y == MAX_Y || gameData.gameBall.y == 0) y = y * (-1);

		SetWaitableTimer(hBallTimer, &li, 0, NULL, NULL, 0);
	}

	return 0;
}

DWORD WINAPI GameThread(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		(*gMappedGame) = gameData;

		SetEvent(hReadEvent);

		WaitForSingleObject(hHasReadEvent, 5000);
	}

	return 0;
}