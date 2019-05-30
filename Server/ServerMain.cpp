#include "ServerHeader.h"

HKEY hRegKey; //Top 10 stored in registry
topPlayer tpTopTen[TOP]; //Top 10 usernames
pPlayer players;
HANDLE hBallThread;
HANDLE hGameThread;
HANDLE hMessageThread;
HANDLE hBallTimer;
DWORD dwGameThreadId;
DWORD dwBallThreadId;
DWORD dwLoginThreadId;
DWORD dwResult;
DWORD dwSize;
game gameData;
int termina;
int nPlayers;

int _tmain(int argc, LPTSTR argv) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	if (setupServer() == -1) {
		_tprintf(TEXT("[ERRO] Não foi possível inicializar o Servidor\n"));
		exit(-1);
	}

	if (setupRegisty() == -1) {
		_tprintf(TEXT("[ERRO] Erro ao criar/abrir chave do registo (%d)\n"), GetLastError());
		exit(-1);
	}

	hMessageThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MessageThread, NULL, NULL, &dwLoginThreadId);
	hGameThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GameThread, NULL, NULL, &dwGameThreadId);

	while (!termina) {
		_tscanf(TEXT("%s"), buffer);

		if (_tcscmp(buffer, TEXT("Start")) == 0) {
			if (gameData.isRunning)
				_tprintf(TEXT("[ERRO] Já está a decorrer um jogo\n"));
			else {
				gameData.isRunning = 1;

				SetEvent(hGameChangedEvent);

				hBallThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BallThread, NULL, NULL, &dwBallThreadId);
			}
		}

		if (_tcscmp(buffer, TEXT("Sair")) == 0) {
			termina = 1;
			gameData.isRunning = 0;
		}

		if (_tcscmp(buffer, TEXT("cls")) == 0) system("cls");
	}

	WaitForSingleObject(hBallThread, INFINITE);
	WaitForSingleObject(hGameThread, INFINITE);
	UnmapViewOfFile(lpMessageBuffer);
	UnmapViewOfFile(gMappedGame);
	CloseHandle(hGameMapFile);
	CloseHandle(hReadEvent);
	CloseHandle(hMessageMutex);
	CloseHandle(hMessageEvent);
	CloseHandle(hLoggedEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hHasReadEvent);
	CloseHandle(hBallTimer);

	return 0;
}

int setupServer() {
	hGameChangedEvent = CreateEvent(NULL, FALSE, FALSE, GAME_CHANGED_EVENT_NAME);

	hMessageMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TCHAR[2][BUFFER_MAX_SIZE]), MESSAGE_FILE_NAME);
	lpMessageBuffer = (TCHAR(*)[2][BUFFER_MAX_SIZE])MapViewOfFile(hMessageMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TCHAR[2][BUFFER_MAX_SIZE]));
	hMessageMutex = CreateMutex(NULL, FALSE, MESSAGE_MUTEX_NAME);
	hMessageEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_MAX_SIZE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, GAME_READ_EVENT);
	hHasReadEvent = CreateEvent(NULL, FALSE, FALSE, GAME_HAS_READ_EVENT);

	hBallTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	gameData.gameBall.x = 0;
	gameData.gameBall.y = 0;
	gameData.gameBall.speed = 1;

	gameData.gameBar.pos = MAX_X / 2;

	gameData.isRunning = 0;
	gameData.points = 0;
	gameData.max_x = MAX_X;
	gameData.max_y = MAX_Y;

	termina = 0;
	nPlayers = 0;

	return 0;
}

int setupRegisty() {

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Breakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwResult) != ERROR_SUCCESS) {
		return -1;
	}
	if (dwResult == REG_OPENED_EXISTING_KEY) {
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
	}

	RegCloseKey(hRegKey);

	return 0;
}

DWORD WINAPI MessageThread(LPVOID lpArg) {
	UNREFERENCED_PARAMETER(lpArg);

	while (!termina) {
		WaitForSingleObject(hMessageEvent, INFINITE);

		if (_tcscmp((*lpMessageBuffer)[0], LOGIN) == 0) {	//Login
			int duplicate = 0;

			if (_tcslen((*lpMessageBuffer)[1]) > 0) {

				for (int i = 0; i < nPlayers; i++) {
					if (_tcscmp((*lpMessageBuffer)[1], players->tUsername) == 0) {
						duplicate = 1;
						break;
					}
				}

				if (!duplicate) {
					players = (pPlayer)realloc(players, sizeof(player) * (nPlayers + 1));

					_tcscpy(players[nPlayers].tUsername, (*lpMessageBuffer)[1]);
					players[nPlayers].hiScore = 0;

					_tprintf(TEXT("[LOGIN] O utilizador %s fez login\n"), players[nPlayers].tUsername);

					_tcscpy(players[nPlayers].tReadEventName, GAME_READ_EVENT);
					_tcscat(players[nPlayers].tReadEventName, players[nPlayers].tUsername);

					_tcscpy(players[nPlayers].tHasReadEventName, GAME_HAS_READ_EVENT);
					_tcscat(players[nPlayers].tHasReadEventName, players[nPlayers].tUsername);

					players[nPlayers].hReadEvent = CreateEvent(NULL, FALSE, FALSE, players[nPlayers].tReadEventName);
					players[nPlayers].hHasReadEvent = CreateEvent(NULL, FALSE, FALSE, players[nPlayers].tHasReadEventName);

					nPlayers++;

					SetEvent(hLoggedEvent);
				}
			}
		}

		if (_tcscmp((*lpMessageBuffer)[0], EXIT) == 0) {	//Logout
			for (int i = 0; i < nPlayers; i++) {
				if (_tcscmp((*lpMessageBuffer)[1], players->tUsername) == 0) {
					_tprintf(TEXT("[LOGOUT] O utilizador %s fez logout\n"), players[i].tUsername);

					CloseHandle(players[i].hReadEvent);
					CloseHandle(players[i].hHasReadEvent);

					nPlayers--;

					for (int j = i; j < nPlayers; j++) {
						players[j] = players[j + 1];
					}

					players = (pPlayer)realloc(players, sizeof(player) * nPlayers);
				}
			}
		}

		if (_tcscmp((*lpMessageBuffer)[0], LEFT) == 0) {
			if (gameData.gameBar.pos > 0) {
				gameData.gameBar.pos--;

				SetEvent(hGameChangedEvent);
			}
		}

		if (_tcscmp((*lpMessageBuffer)[0], RIGHT) == 0) {
			if (gameData.gameBar.pos + 32 < MAX_X) {
				gameData.gameBar.pos++;

				SetEvent(hGameChangedEvent);
			}
		}
	}

	return 0;
}

DWORD WINAPI BallThread(LPVOID lpArg) {
	UNREFERENCED_PARAMETER(lpArg);

	LARGE_INTEGER li;

	li.QuadPart = -1000000LL;

	SetWaitableTimer(hBallTimer, &li, 0, NULL, NULL, 0);

	int x = 1;
	int y = 1;

	while (gameData.isRunning && !termina) {
		WaitForSingleObject(hBallTimer, INFINITE);

		gameData.gameBall.x += x;
		gameData.gameBall.y += y;

		//Section for collision detection
		if (gameData.gameBall.x == gameData.max_x - 8 || gameData.gameBall.x == 0) x = x * (-1);
		if (gameData.gameBall.y == 0) y = y * (-1);
		if (gameData.gameBall.y >= gameData.max_y - 8) gameData.gameBall.y = 0;
		if (gameData.gameBall.y == gameData.max_y - 16 && gameData.gameBar.pos <= gameData.gameBall.x + 8 && gameData.gameBar.pos + 32 >= gameData.gameBall.x) y = y * (-1);

		SetEvent(hGameChangedEvent);

		SetWaitableTimer(hBallTimer, &li, 0, NULL, NULL, 0);
	}

	return 0;
}

DWORD WINAPI GameThread(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	DWORD dwWaitResult;
	int currentPlayers;

	while (!termina) {
		WaitForSingleObject(hGameChangedEvent, INFINITE);

		(*gMappedGame) = gameData;

		currentPlayers = nPlayers;

		for (int i = 0; i < currentPlayers; i++) {
			SetEvent(players[i].hReadEvent);
		}

		for (int i = 0; i < currentPlayers; i++) {
			dwWaitResult = WaitForSingleObject(players[i].hHasReadEvent, 5000);

			if (dwWaitResult != WAIT_OBJECT_0) {
				_tprintf(TEXT("[TIMEOUT] O utilizador %s deu timeout\n"), players[i].tUsername);

				CloseHandle(players[i].hReadEvent);
				CloseHandle(players[i].hHasReadEvent);

				nPlayers--;

				currentPlayers = nPlayers;

				for (int j = i; j < currentPlayers; j++) {
					players[j] = players[j + 1];
				}

				players = (pPlayer)realloc(players, sizeof(player) * nPlayers);

				i--;
			}
		}
	}

	return 0;
}