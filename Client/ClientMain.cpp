#include "ClientHeader.h"

HANDLE hGameThread;
game gameData;
player data;
bool termina = false;

int _tmain(int argc, LPTSTR argv) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	SetupClient(&data);

	if (Login(&data) == -1) {
		_gettchar();
		exit(-1);
	}

	hGameThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveGame, NULL, 0, NULL);

	WaitForSingleObject(hGameThread, INFINITE);

	CloseClient();

	return 0;
}

DWORD WINAPI ReceiveGame(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		if (ReceiveBroadcast(&gameData) != 0) {
			_tprintf(TEXT("[TIMEOUT] A conex�o foi perdida\n"));
			break;
		}

		_tprintf(TEXT("[SERVER] Posi��o da bola (%d, %d)\n"), gameData.gameBall.x, gameData.gameBall.y);
	}

	return 0;
}
