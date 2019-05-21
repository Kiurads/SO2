// DLL.cpp : Defines the exported functions for the DLL application.

#include <windows.h>
#include "Dll.h"

HANDLE hGameChangedEvent;
HANDLE hGameMapFile;
game *gMappedGame;
HANDLE hReadEvent;
HANDLE hHasReadEvent;
HANDLE hLoginMapFile;
HANDLE hLoginEvent;
HANDLE hLoggedEvent;
TCHAR(*lpMessageBuffer)[BUFFER_MAX_SIZE];
HANDLE hLoginMutex;
HANDLE hLoginPipe;
BOOL success;
DWORD nBytes;
TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
int iAuthReply;

int Login(pPlayer data) {
	DWORD dwWaitResult;

	WaitForSingleObject(hLoginMutex, INFINITE);	//Wait for the server to allow the login to be done

	_stprintf((*lpMessageBuffer), TEXT("%s"), data->tUsername);

	SetEvent(hLoginEvent);

	dwWaitResult = WaitForSingleObject(hLoggedEvent, 1000);

	if (dwWaitResult != WAIT_OBJECT_0) {
		ReleaseMutex(hLoginMutex);
		return -1;
	}

	_tcscpy(data->tReadEventName, GAME_READ_EVENT);
	_tcscat(data->tReadEventName, data->tUsername);

	_tcscpy(data->tHasReadEventName, GAME_HAS_READ_EVENT);
	_tcscat(data->tHasReadEventName, data->tUsername);

	hReadEvent = CreateEvent(NULL, FALSE, FALSE, data->tReadEventName);
	hHasReadEvent = CreateEvent(NULL, FALSE, FALSE, data->tHasReadEventName);
	
	ReleaseMutex(hLoginMutex);
	return 0;
}

int ReceiveBroadcast(pGame gameData) {
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(hReadEvent, 5000);	//Wait 5 seconds to get game data

	if (dwWaitResult != WAIT_OBJECT_0)
		return -1;
	else
		(*gameData) = (*gMappedGame);

	SetEvent(hHasReadEvent);	//Signal the server that the info has been read
	return 0;
}

int SendMsg(void) {
	return 0;
}

int ReceiveMessage(void) {
	return 0;
}

void SetupClient(pPlayer data) {
	hGameChangedEvent = CreateEvent(NULL, FALSE, FALSE, GAME_CHANGED_EVENT_NAME);

	hLoginMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, LOGIN_FILE_NAME);
	lpMessageBuffer = (TCHAR(*)[BUFFER_MAX_SIZE])MapViewOfFile(hLoginMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_MAX_SIZE);
	hLoginMutex = CreateMutex(NULL, FALSE, LOGIN_MUTEX_NAME);
	hLoginEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));

	memset(data->tUsername, '\0', sizeof(TCHAR) * TAM);
}

void CloseClient() {
	UnmapViewOfFile(lpMessageBuffer);
	UnmapViewOfFile(gMappedGame);
	CloseHandle(hGameMapFile);
	CloseHandle(hReadEvent);
	CloseHandle(hLoginMutex);
	CloseHandle(hLoginEvent);
	CloseHandle(hLoggedEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hHasReadEvent);
}