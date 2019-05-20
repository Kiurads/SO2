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

	_tprintf(TEXT(" - Seja bem vindo ao Breakout! - \n"));

	_tprintf(TEXT("Introduza o Username para se autenticar no servidor: \n"));
	_fgetts(data->tUsername, 256, stdin);

	data->tUsername[_tcslen(data->tUsername) - 1] = '\0';

	_stprintf((*lpMessageBuffer), TEXT("%s"), data->tUsername);

	SetEvent(hLoginEvent);

	dwWaitResult = WaitForSingleObject(hLoggedEvent, 5000);

	if (dwWaitResult != WAIT_OBJECT_0) {
		_tprintf(TEXT("[ERRO] Login não foi aceite\n"));

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