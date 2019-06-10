// DLL.cpp : Defines the exported functions for the DLL application.

#include <windows.h>
#include "Dll.h"

HANDLE hServerPipe;
HANDLE hClientPipe;
BOOL isPipeSuccess = false;

HANDLE hGameChangedEvent;
HANDLE hHasReadEvent;
HANDLE hReadEvent;
HANDLE hMessageEvent;
HANDLE hLoggedEvent;

HANDLE hGameMapFile;
game *gGameData;
HANDLE hMessageMapFile;
HANDLE hMessageMutex;
HANDLE hServerPipeMutex;
TCHAR RemoteMessage[2][BUFFER_MAX_SIZE];

TCHAR(*lpMessageBuffer)[2][BUFFER_MAX_SIZE];
BOOL success;
DWORD nBytes;
TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];

int iAuthReply;
int isRemote;

void SetupClient(pPlayer data, pGame gameData) {

	if (!isRemote) {

	hGameChangedEvent = CreateEvent(NULL, FALSE, FALSE, GAME_CHANGED_EVENT_NAME);

	hMessageMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MESSAGE_FILE_NAME);
	lpMessageBuffer = (TCHAR(*)[2][BUFFER_MAX_SIZE])MapViewOfFile(hMessageMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TCHAR[2][BUFFER_MAX_SIZE]));
	hMessageMutex = CreateMutex(NULL, FALSE, MESSAGE_MUTEX_NAME);
	hMessageEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, GAME_FILE_NAME);
	gGameData = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));

	for (int i = 0; i < TRIPLE; i++) {
		if (i == 0) {
			gameData->gameBall[i].x = 0;
			gameData->gameBall[i].y = 0;
			gameData->gameBall[i].speed = 1;
		}
		else {
			gameData->gameBall[i].x = -1;
			gameData->gameBall[i].y = -1;
			gameData->gameBall[i].speed = -1;
		}
	}

	for(int i = 0; i < gameData->nPlayers; i++)
		gameData->gameBar[i].pos = MAX_X / 2;

	gameData->isRunning = 0;
	gameData->points = 0;
	gameData->max_x = MAX_X;
	gameData->max_y = MAX_Y;

	memset(data->tUsername, '\0', sizeof(TCHAR) * TAM);
	gameData->nLives = INITIAL_LIVES;
	}
	else {
		gGameData = gameData;

		for (int i = 0; i < TRIPLE; i++) {
			if (i == 0) {
				gameData->gameBall[i].x = 0;
				gameData->gameBall[i].y = 0;
				gameData->gameBall[i].speed = 1;
			}
			else {
				gameData->gameBall[i].x = -1;
				gameData->gameBall[i].y = -1;
				gameData->gameBall[i].speed = -1;
			}
		}

		for(int i = 0; i < gameData->nPlayers; i++)
			gameData->gameBar[i].pos = MAX_X / 2;

		gameData->isRunning = 0;
		gameData->points = 0;
		gameData->max_x = MAX_X;
		gameData->max_y = MAX_Y;

		memset(data->tUsername, '\0', sizeof(TCHAR) * TAM);
		gameData->nLives = INITIAL_LIVES;
	}
}

int Login(pPlayer data) {
	DWORD dwWaitResult;

	if (!isRemote) {

		WaitForSingleObject(hMessageMutex, INFINITE);	//Wait for the server to allow the login to be done

		_stprintf((*lpMessageBuffer)[0], TEXT("%s"), LOGIN);
		_stprintf((*lpMessageBuffer)[1], TEXT("%s"), data->tUsername);

		SetEvent(hMessageEvent);

		dwWaitResult = WaitForSingleObject(hLoggedEvent, 1000);

		if (dwWaitResult != WAIT_OBJECT_0) {
			ReleaseMutex(hMessageMutex);
			return -1;
		}

		_tcscpy(data->tReadEventName, GAME_READ_EVENT);
		_tcscat(data->tReadEventName, data->tUsername);

		_tcscpy(data->tHasReadEventName, GAME_HAS_READ_EVENT);
		_tcscat(data->tHasReadEventName, data->tUsername);

		hReadEvent = CreateEvent(NULL, FALSE, FALSE, data->tReadEventName);
		hHasReadEvent = CreateEvent(NULL, FALSE, FALSE, data->tHasReadEventName);

		ReleaseMutex(hMessageMutex);
	}
	else {
		_stprintf((RemoteMessage[0]), TEXT("%s"), LOGIN);
		_stprintf((RemoteMessage[1]), TEXT("%s"), data->tUsername);

		if (!WaitNamedPipe(SERVER_PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
			return -1;
		}

		hServerPipe = CreateFile(SERVER_PIPE_NAME, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (hServerPipe == INVALID_HANDLE_VALUE) {
			return -1;
		}

		hClientPipe = CreateFile(CLIENT_PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (!WriteFile(hServerPipe, RemoteMessage, sizeof(RemoteMessage), &nBytes, NULL)) {
			CloseHandle(hServerPipe);
			CloseHandle(hClientPipe);
			return -1;
		}

		if (!ReadFile(hClientPipe, &gGameData, sizeof(game), &nBytes, NULL)) {
			return -1;
		}

		if (nBytes != sizeof(game)) {
			return -1;
		}
	}

	return 0;
}

int ReceiveBroadcast(pGame gameData) {
	DWORD dwWaitResult;

	if (!isRemote) {

	dwWaitResult = WaitForSingleObject(hReadEvent, 1000);

	if (dwWaitResult == WAIT_FAILED)
		return -1;
	else
		(*gameData) = (*gGameData);

	SetEvent(hHasReadEvent);	//Signal the server that the info has been read
	}
	else {
		if (!ReadFile(hClientPipe, gameData, sizeof(game), &nBytes, NULL))
			return -1;


		if (nBytes != sizeof(game)) {

			return -1;
		}
	}
	return 0;
}

int SendMsg(player data, TCHAR *msg) {
	if (!isRemote) {
		WaitForSingleObject(hMessageMutex, INFINITE);

		_stprintf((*lpMessageBuffer)[0], msg);
		_stprintf((*lpMessageBuffer)[1], data.tUsername);

		SetEvent(hMessageEvent);

		ReleaseMutex(hMessageMutex);
	}
	else {
		_stprintf((RemoteMessage)[0], msg);
		_stprintf((RemoteMessage)[1], data.tUsername);

		if (!WriteFile(hServerPipe, &RemoteMessage, sizeof(RemoteMessage), &nBytes, NULL)) {

			return -1;
		}

		if (nBytes != sizeof(RemoteMessage)) {

			return -1;
		}
	}
	return 0;
}

int ReceiveMessage(void) {
	return 0;
}

void CloseClient(void) {
	if (!isRemote) {
		UnmapViewOfFile(lpMessageBuffer);
		UnmapViewOfFile(gGameData);
		CloseHandle(hGameMapFile);
		CloseHandle(hReadEvent);
		CloseHandle(hMessageMutex);
		CloseHandle(hMessageEvent);
		CloseHandle(hLoggedEvent);
		CloseHandle(hReadEvent);
		CloseHandle(hHasReadEvent);
	}
	else {
		CloseHandle(hClientPipe);
		CloseHandle(hServerPipe);
	}
}