#include "ClientHeader.h"

HANDLE hThread;
game gameData;
bool termina = false;

int _tmain(int argc, LPTSTR argv) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	SetupClient();

	if (Login() == -1) {
		_gettchar();
		exit(-1);
	}

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveGame, NULL, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	UnmapViewOfFile(lpLoginBuffer);
	UnmapViewOfFile(gMappedGame);
	CloseHandle(hGameMapFile);
	CloseHandle(hReadEvent);
	return 0;
}

void SetupClient() {
	hLoginMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, LOGIN_FILE_NAME);
	lpLoginBuffer = (TCHAR(*)[BUFFER_MAX_SIZE])MapViewOfFile(hLoginMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_MAX_SIZE);
	hLoginMutex = CreateMutex(NULL, FALSE, LOGIN_MUTEX_NAME);
	hLoginEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, GAME_READ_EVENT);
	hHasReadEvent = CreateEvent(NULL, FALSE, FALSE, GAME_HAS_READ_EVENT);
}

DWORD WINAPI ReceiveGame(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(hReadEvent, 5000);

		if (dwWaitResult != WAIT_OBJECT_0) {
			_tprintf(TEXT("[ERRO] Conex�o deu timeout\n"));
			break;
		}
		else {
			gameData = (*gMappedGame);

			_tprintf(TEXT("[SERVER] Posi��o da bola (%d, %d)\n"), gameData.gameBall.x, gameData.gameBall.y);

			SetEvent(hHasReadEvent);
		}
	}

	return 0;
}
