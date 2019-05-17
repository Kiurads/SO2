#include "ClientHeader.h"

HANDLE hThread;
game gameData;
player data;
bool termina = false;

int _tmain(int argc, LPTSTR argv) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	SetupClient();

	if (Login(&data) == -1) {
		_gettchar();
		exit(-1);
	}

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveGame, NULL, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	UnmapViewOfFile(lpLoginBuffer);
	UnmapViewOfFile(gMappedGame);
	CloseHandle(hGameMapFile);
	CloseHandle(hReadEvent);
	CloseHandle(hLoginMutex);
	CloseHandle(hLoginEvent);
	CloseHandle(hLoggedEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hHasReadEvent);

	return 0;
}

void SetupClient() {
	hGameChangedEvent = CreateEvent(NULL, FALSE, FALSE, GAME_CHANGED_EVENT_NAME);

	hLoginMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, LOGIN_FILE_NAME);
	lpLoginBuffer = (TCHAR(*)[BUFFER_MAX_SIZE])MapViewOfFile(hLoginMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_MAX_SIZE);
	hLoginMutex = CreateMutex(NULL, FALSE, LOGIN_MUTEX_NAME);
	hLoginEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));
}

DWORD WINAPI ReceiveGame(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(hReadEvent, 10000);

		if (dwWaitResult != WAIT_OBJECT_0) {
			_tprintf(TEXT("[ERRO] Conexão deu timeout\n"));
			break;
		}
		else {
			gameData = (*gMappedGame);

			_tprintf(TEXT("[SERVER] Posição da bola (%d, %d)\n"), gameData.gameBall.x, gameData.gameBall.y);

			SetEvent(hHasReadEvent);
		}
	}

	return 0;
}
