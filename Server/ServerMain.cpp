#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "Dll.h"

#define TOP 10
#define SIZE 50

typedef struct player {
	TCHAR tUsername[SIZE];
	int hiScore;
} player;

int cmpfunc(const void * a, const void * b) {
	player *x = (player*)a;
	player *y = (player*)b;

	return (x->hiScore - y->hiScore);
}

HKEY hRegKey; //Top 10 stored in registry
player tpTopTen[TOP], newUser; //Top 10 usernames
DWORD dwResult;
DWORD dwSize;

int setupServerPipes() {
	hServerPipe = CreateNamedPipe(SERVER_PIPE_NAME, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, sizeof(buffer), sizeof(buffer), 2000, NULL);
	hLoginPipe = CreateNamedPipe(LOGIN_PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, sizeof(buffer), sizeof(buffer), 2000, NULL);
	hClientPipe = CreateNamedPipe(CLIENT_PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, sizeof(buffer), sizeof(buffer), 2000, NULL);

	if (hServerPipe == INVALID_HANDLE_VALUE || hLoginPipe == INVALID_HANDLE_VALUE || hClientPipe == INVALID_HANDLE_VALUE) {
		return -1;
	}
	return 0;
}

int setupRegisty() {

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Breakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwResult) != ERROR_SUCCESS) {
		return -1;
	}
	else {
		if (dwResult == REG_CREATED_NEW_KEY) {
			_tprintf(TEXT("Key: HKEY_CURRENT_USER\\Software\\Breakout created\n"));

			for (int i = 0; i < TOP; i++) {
				TCHAR tPoints[SIZE];

				_tprintf(TEXT("Username %d: (25 characters max): "), i);
				_tscanf(TEXT("%49s"), tpTopTen[i].tUsername);

				_tprintf(TEXT("hiScore: "));
				_tscanf(TEXT("%d"), &tpTopTen[i].hiScore);

				_stprintf(tPoints, TEXT("%d"), tpTopTen[i].hiScore);

				RegSetValueEx(hRegKey,
					tpTopTen[i].tUsername,
					0,
					REG_SZ,
					(LPBYTE)tPoints,
					_tcslen(tPoints) * sizeof(TCHAR));

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

				RegSetValueEx(hRegKey,
					tPosition,
					0,
					REG_SZ,
					(LPBYTE)tpTopTen[i].tUsername,
					_tcslen(tpTopTen[i].tUsername) * sizeof(TCHAR)); //Register positions of the users
			}
		}
		else {
			_tprintf(TEXT("Key: HKEY_CURRENT_USER\\Software\\Breakout opened\n"));

			for (int i = 0; i < TOP; i++) {
				int position;
				TCHAR tPosition[3];
				TCHAR tPoints[TOP];

				position = i + 1;

				_stprintf(tPosition, TEXT("%d"), position);

				dwSize = SIZE;

				memset(tpTopTen[i].tUsername, '\0', dwSize);

				RegQueryValueEx(hRegKey,
					tPosition,
					NULL,
					NULL,
					(LPBYTE)tpTopTen[i].tUsername,
					&dwSize); //Read usernames from registry

				dwSize = TOP;

				memset(tPoints, '\0', TOP);

				RegQueryValueEx(hRegKey,
					tpTopTen[i].tUsername,
					NULL,
					NULL,
					(LPBYTE)tPoints,
					&dwSize); //Read hiScore from registry

				tpTopTen[i].hiScore = _ttoi(tPoints);
			}

			for (int i = 0; i < TOP; i++)
				_tprintf(TEXT("%d - %s: %d\n"), i + 1, tpTopTen[i].tUsername, tpTopTen[i].hiScore);
		}
	}

	RegCloseKey(hRegKey);

	return 0;
}

int getLogin() {
	BOOL check = false;
	_tprintf(TEXT("� espera que um Cliente estabele�a uma liga��o com o Servidor\n"));

	check = ConnectNamedPipe(hLoginPipe, NULL);

	if (!check) {
		_tprintf(TEXT("[ERRO] Liga��o inv�lida com o Cliente"));
		return -1;
	}

	if (!ReadFile(hLoginPipe, &buffer, BUFFER_MAX_SIZE * sizeof(TCHAR), &nBytes, NULL)) {
		_tprintf(TEXT("LONG DICK %d"), GetLastError());
		_gettchar();
		return -1;
	}
	else {
		buffer[nBytes / sizeof(TCHAR)] = '\0';
		_tprintf(TEXT("Utilizador %s autenticado com sucesso"), buffer);
		_tcscpy(newUser.tUsername, buffer);
		_gettchar();
	}

	return 0;
}

int _tmain(int argc, LPTSTR argv) {
	BOOL loggedIn = false;

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	if (setupServerPipes() == -1) {
		_tprintf(TEXT("N�o foi poss�vel criar o pipe do Servidor\n"));
		_gettchar();
		exit(-1);
	}

	if (setupRegisty() == -1) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		exit(-1);
	}

	do {
		if (getLogin() == -1) {
			_tprintf(TEXT("Login Inv�lido por parte de um Cliente\n"));
		}
		else
			break;
	} while (!loggedIn);


	//startBallMovement();

	_gettchar();

	_gettchar();

	return 0;
}