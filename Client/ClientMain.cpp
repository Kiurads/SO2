#include "ClientHeader.h"

HANDLE hGameThread;
game gameData;
player data;
bool termina = false;

TCHAR szProgName[] = TEXT("Breakout - Cliente");

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	SetupClient();

	HWND hWnd;
	MSG lpMsg;
	WNDCLASSEX wcApp;

	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;
	wcApp.lpszClassName = szProgName;
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LOGO_BREAKOUT));
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LOGO_BREAKOUT));
	wcApp.hCursor = LoadCursor(hInst, IDC_ARROW);
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

	if (!RegisterClassEx(&wcApp)) 
		return(0);

	hWnd = CreateWindow(szProgName, TEXT("Breakout"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, (HWND)HWND_DESKTOP, (HMENU)NULL, (HINSTANCE)hInst, 0);

	if (Login(&data) == -1) {
		_gettchar();
		exit(-1);
	}

	hGameThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveGame, NULL, 0, NULL);

	WaitForSingleObject(hGameThread, INFINITE);

	UnmapViewOfFile(lpMessageBuffer);
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
	lpMessageBuffer = (TCHAR(*)[BUFFER_MAX_SIZE])MapViewOfFile(hLoginMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_MAX_SIZE);
	hLoginMutex = CreateMutex(NULL, FALSE, LOGIN_MUTEX_NAME);
	hLoginEvent = CreateEvent(NULL, FALSE, FALSE, LOGIN_EVENT_NAME);
	hLoggedEvent = CreateEvent(NULL, FALSE, FALSE, LOGGED_EVENT_NAME);

	hGameMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, GAME_FILE_NAME);
	gMappedGame = (game(*))MapViewOfFile(hGameMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(game));
}

DWORD WINAPI ReceiveGame(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		if (ReceiveBroadcast(&gameData) != 0) {
			_tprintf(TEXT("[TIMEOUT] A conexão foi perdida\n"));
			break;
		}

		_tprintf(TEXT("[SERVER] Posição da bola (%d, %d)\n"), gameData.gameBall.x, gameData.gameBall.y);
	}

	return 0;
}
