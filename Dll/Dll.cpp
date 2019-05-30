// DLL.cpp : Defines the exported functions for the DLL application.

#include <windows.h>
#include "Dll.h"

HANDLE hGameChangedEvent;
HANDLE hGameMapFile;
game *gMappedGame;
HANDLE hReadEvent;
HANDLE hHasReadEvent;
HANDLE hMessageMapFile;
HANDLE hMessageEvent;
HANDLE hLoggedEvent;
TCHAR(*lpMessageBuffer)[2][BUFFER_MAX_SIZE];
HANDLE hMessageMutex;
HANDLE hLoginPipe;
BOOL success;
DWORD nBytes;
TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
int iAuthReply;

int Login(pPlayer data) {
	DWORD dwWaitResult;

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
	return 0;
}

int ReceiveBroadcast(pGame gameData) {
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(hReadEvent, 1000);

	if (dwWaitResult == WAIT_FAILED)
		return -1;
	else
		(*gameData) = (*gMappedGame);

	SetEvent(hHasReadEvent);	//Signal the server that the info has been read
	return 0;
}

int SendMsg(player data, TCHAR *msg) {
	WaitForSingleObject(hMessageMutex, INFINITE);

	_stprintf((*lpMessageBuffer)[0], msg);
	_stprintf((*lpMessageBuffer)[1], data.tUsername);

	SetEvent(hMessageEvent);

	ReleaseMutex(hMessageMutex);
	return 0;
}

int ReceiveMessage(void) {
	return 0;
}

void SetupClient(pPlayer data, pGame gameData) {
	hGameChangedEvent = CreateEvent(NULL, FALSE, FALSE, GAME_CHANGED_EVENT_NAME);

	hMessageMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MESSAGE_FILE_NAME);
	lpMessageBuffer = (TCHAR(*)[2][BUFFER_MAX_SIZE])MapViewOfFile(hMessageMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TCHAR[2][BUFFER_MAX_SIZE]));
	hMessageMutex = CreateMutex(NULL, FALSE, MESSAGE_MUTEX_NAME);
	hMessageEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));

	gameData->gameBall.x = 0;
	gameData->gameBall.y = 0;
	gameData->gameBall.speed = 1;

	gameData->gameBar.pos = MAX_X / 2;

	gameData->isRunning = 0;
	gameData->points = 0;
	gameData->max_x = MAX_X;
	gameData->max_y = MAX_Y;

	memset(data->tUsername, '\0', sizeof(TCHAR) * TAM);
}

void CloseClient() {
	UnmapViewOfFile(lpMessageBuffer);
	UnmapViewOfFile(gMappedGame);
	CloseHandle(hGameMapFile);
	CloseHandle(hReadEvent);
	CloseHandle(hMessageMutex);
	CloseHandle(hMessageEvent);
	CloseHandle(hLoggedEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hHasReadEvent);
}