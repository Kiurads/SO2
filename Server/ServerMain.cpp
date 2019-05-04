#include "ServerHeader.h"

HKEY hRegKey; //Top 10 stored in registry
player tpTopTen[TOP], newUser; //Top 10 usernames
HANDLE hBallThread;
DWORD dwBallThreadId;
DWORD dwResult;
DWORD dwSize;
ball gameBall;

int _tmain(int argc, LPTSTR argv) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	if (setupServer() == -1) {
		_tprintf(TEXT("Não foi possível criar o pipe do Servidor\n"));
		_gettchar();
		exit(-1);
	}

	if (setupRegisty() == -1) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		exit(-1);
	}

	do {
		if (getLogin() == -1)
			_tprintf(TEXT("Login inválido por parte de um Cliente\n"));
		else
			break;
	} while (TRUE);

	hBallThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BallThread, NULL, NULL, &dwBallThreadId);

	WaitForSingleObject(hBallThread, INFINITE);

	_gettchar();

	return 0;
}

DWORD WINAPI BallThread(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	int x = 1;
	int y = 1;

	TCHAR message[TAM];

	while (TRUE) {
		gameBall.x += x;
		gameBall.y += y;

		if (gameBall.x == MAX_X || gameBall.x == 0) x = x * (-1);
		if (gameBall.y == MAX_Y || gameBall.y == 0) y = y * (-1);

		_stprintf(message, TEXT("X:%d Y:%d"), gameBall.x, gameBall.y);

		_tprintf(TEXT("%s\n"), message);

		if (!WriteFile(hClientPipe, message, (DWORD)_tcslen(message) * sizeof(TCHAR), &nBytes, NULL)) {
			_tprintf(TEXT("[ERRO] Não foi possível escrever para o pipe do Cliente\n"));
			return -1;
		}

		SetEvent(hReadEvent);

		Sleep(1000);
	}

	return 0;
}

int setupServer() {
	hLoginPipe = CreateNamedPipe(LOGIN_PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, sizeof(buffer), sizeof(buffer), 2000, NULL);
	hClientPipe = CreateNamedPipe(CLIENT_PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, sizeof(buffer), sizeof(buffer), 2000, NULL);
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ReadEvent"));

	gameBall.x = 0;
	gameBall.y = 0;

	return 0;
}

int getLogin() {
	BOOL check = false;
	_tprintf(TEXT("À espera que um Cliente estabeleça uma ligação com o Servidor\n"));

	check = ConnectNamedPipe(hLoginPipe, NULL);

	if (!check) {
		_tprintf(TEXT("[ERRO] Ligação inválida com o Cliente"));
		return -1;
	}

	if (!ReadFile(hLoginPipe, &buffer, BUFFER_MAX_SIZE * sizeof(TCHAR), &nBytes, NULL)) {
		_tprintf(TEXT("[ERRO] %d"), GetLastError());
		_gettchar();
		return -1;
	}
	else {
		buffer[nBytes / sizeof(TCHAR)] = '\0';
		_tprintf(TEXT("Utilizador %s autenticado com sucesso\n"), buffer);
		_tcscpy(newUser.tUsername, buffer);
		newUser.hiScore = 0;

		if (!WriteFile(hClientPipe, LOGIN_SUCCESS, sizeof(TCHAR), &nBytes, NULL)) {
			_tprintf(TEXT("[ERRO] Não foi possível escrever para o pipe do Cliente\n"));
			return -1;
		}
	}

	return 0;
}

int setupRegisty() {

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Breakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwResult) != ERROR_SUCCESS) {
		return -1;
	}
	if (dwResult == REG_CREATED_NEW_KEY) {
		_tprintf(TEXT("Key: HKEY_CURRENT_USER\\Software\\Breakout created\n\n"));
		_tprintf(TEXT("TOP 10\n\n"));

		for (int i = 0; i < TOP; i++) {
			TCHAR tPoints[TAM];

			_tprintf(TEXT("Username %d: "), i + 1);
			_tscanf(TEXT("%49s"), tpTopTen[i].tUsername);

			_tprintf(TEXT("Hi-Score: "));
			_tscanf(TEXT("%d"), &tpTopTen[i].hiScore);

			_stprintf(tPoints, TEXT("%d"), tpTopTen[i].hiScore);

			RegSetValueEx(hRegKey, tpTopTen[i].tUsername, 0, REG_SZ, (LPBYTE)tPoints, (DWORD)_tcslen(tPoints) * sizeof(TCHAR));

			fflush(stdin);
		}

		qsort(tpTopTen, TOP, sizeof(player), cmpfunc);

		_tprintf(TEXT("\n"));

		for (int i = 0; i < TOP; i++) {
			int position;
			TCHAR tPosition[3];

			position = TOP - i;

			_stprintf(tPosition, TEXT("%d"), position);

			_tprintf(TEXT("%s: %d\n"), tpTopTen[i].tUsername, tpTopTen[i].hiScore);

			//Register positions of the users
			RegSetValueEx(hRegKey, tPosition, 0, REG_SZ, (LPBYTE)tpTopTen[i].tUsername, (DWORD)_tcslen(tpTopTen[i].tUsername) * sizeof(TCHAR));
		}
	}
	else {
		_tprintf(TEXT("Key: HKEY_CURRENT_USER\\Software\\Breakout opened\n\n"));

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

		for (int i = 0; i < TOP; i++)
			_tprintf(TEXT("%d - %s: %d\n"), i + 1, tpTopTen[i].tUsername, tpTopTen[i].hiScore);
	}

	RegCloseKey(hRegKey);

	return 0;
}

int cmpfunc(const void * a, const void * b) {
	pPlayer x = (pPlayer)a;
	pPlayer y = (pPlayer)b;

	return (x->hiScore - y->hiScore);
}