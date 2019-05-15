// DLL.cpp : Defines the exported functions for the DLL application.

#include <windows.h>
#include "Dll.h"

char pointer[4096];

HANDLE hGameMapFile;
game *gMappedGame;
HANDLE hReadEvent;
HANDLE hHasReadEvent;
HANDLE hLoginMapFile;
HANDLE hLoggedEvent;
TCHAR(*lpLoginBuffer)[BUFFER_MAX_SIZE];
HANDLE hLoginPipe;
BOOL success;
DWORD nBytes;
TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
int iAuthReply;

int Login(void) {
	_tprintf(TEXT(" - Seja bem vindo ao Breakout! - \n"));

	DWORD dwWaitResult;

	_tprintf(TEXT("Introduza o Username para se autenticar no servidor: \n"));
	_fgetts(tName, 256, stdin);

	tName[_tcslen(tName) - 1] = '\0';

	_stprintf((*lpLoginBuffer), TEXT("%s"), tName);

	dwWaitResult = WaitForSingleObject(hLoggedEvent, 5000);

	if (dwWaitResult != WAIT_OBJECT_0) {
		_tprintf(TEXT("[ERRO] Conexão deu timeout\n"));
		return -1;
	}

	return 0;
}
int ReceiveBroadcast(void) {
	return 0;
}
int SendMsg(void) {
	return 0;
}

int ReceiveMessage(void) {
	return 0;
}