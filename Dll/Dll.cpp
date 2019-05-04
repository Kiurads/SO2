// DLL.cpp : Defines the exported functions for the DLL application.

#include <windows.h>
#include "Dll.h"

char pointer[4096];

HANDLE hServerPipe;
HANDLE hLoginPipe;;
HANDLE hClientPipe;
HANDLE hBroadcastPipe;
BOOL success = false;
DWORD nBytes;
TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
int iAuthReply = -1;

int Login(void) {
	//verificar se o servidor existe
	//receber username
	//abrir pipe do servidor para escrita
	//escrever o username do utilizador para a pipe do servidor (mutex para o acesso e escrita do named pipe do servidor)
	//fechar a extremidade do pipe
	//abrir pipe do cliente para leitura
	//verificar se foi autentcado com sucesso
	//se sim, continuar a ler do seu named pipe para receber as posi��es da bola (thread que l� de segundo a segundo)
	//se n�o, fechar o seu named pipe e voltar a pedir o username
	BOOL loggedIn = false, check = true;

	hLoginPipe = CreateFile(LOGIN_PIPE_NAME, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	//hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hLoginPipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("LAST ERROR: %d"), GetLastError());
		_tprintf(TEXT("[ERRO] Não foi possível abrir o pipe do Servidor para escrita\n"));
		return -1;
	}

	/*check = WaitNamedPipe(LOGIN_PIPE_NAME, INFINITE);

	if (!check) {
		_tprintf(TEXT("[ERRO] N�o existe nenhum servidor a correr neste momento\n Programa a terminar... (any key to exit)\n"));
		return -1;
	}*/

	_tprintf(TEXT(" - Seja bem vindo ao Breakout! - \n"));

	do {
		_tprintf(TEXT("Introduza o Username para se autenticar no servidor: \n"));
		_fgetts(tName, 256, stdin);

		tName[_tcslen(tName) - 1] = '\0';

		if (!WriteFile(hLoginPipe, buffer, _tcsclen(tName) * sizeof(TCHAR), &nBytes, NULL)) {
			_tprintf(TEXT("[ERRO] Não foi possível escrever para o pipe do Servidor (any key to exit)\n"));
			return -1;
		}

		if (nBytes != _tcsclen(tName) * sizeof(TCHAR)) {
			_tprintf(TEXT("[ERRO] A mensagem enviada não corresponde ao Username\n"));
		}

		hClientPipe = CreateFile(CLIENT_PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		success = ReadFile(hClientPipe, buffer, sizeof(buffer), &nBytes, NULL);

		if (nBytes != sizeof(int)) {
			_tprintf(TEXT("[ERRO] Mensagem recebida do servidor é incoerente\n"));
		}
		else if (!success) {
			_tprintf(TEXT("[ERRO] Não foi possível abrir o pipe do Cliente para leitura\n"));
		}
		else {
			buffer[nBytes / sizeof(int)] = '\0';
			if (_tcscmp(buffer, TEXT("1"))) {
				_tprintf(TEXT("Autenticação bem sucedida. Bem vindo %s\n\n"), tName);
				loggedIn = true;
				break;
			}
			else {
				_tprintf(TEXT("[ERRO] Autenticação inválida\n"));
			}
		}

	} while (nBytes != _tcsclen(tName) * sizeof(TCHAR));


	CloseHandle(hClientPipe);
	CloseHandle(hLoginPipe);

	return 0;

}
int SendMsg(void) {

	hServerPipe = CreateFile(SERVER_PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hServerPipe == NULL) {
		_tprintf(TEXT("[ERRO] Não foi poss�vewl estabelecer ligação com o Servidor\n"));
		return -1;
	}

	_tprintf(TEXT("Introduza a mensagem a enviar ao servidor: \n"));
	_fgetts(buffer, 256, stdin);

	tName[_tcslen(tName) - 1] = '\0';

	if (!WriteFile(hServerPipe, buffer, _tcsclen(tName) * sizeof(TCHAR), &nBytes, NULL)) {
		_tprintf(TEXT("[ERRO] Não foi poss�vel escrever para o pipe do Servidor (any key to exit)\n"));
		return -1;
	}

	return 0;
}

int ReceiveMessage(void) {

	hClientPipe = CreateFile(CLIENT_PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hClientPipe == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possível abrir o pipe do Cliente\n"));
		return -1;
	}

	if (!ReadFile(hClientPipe, buffer, sizeof(buffer), &nBytes, NULL)) {
		_tprintf(TEXT("[ERRO] Leitura do Pipe do Cliente não foi bem sucedida\n"));
		return -1;
	}

	buffer[nBytes / sizeof(TCHAR)] = '\0';

	_tprintf(TEXT("Mensagem recebida do Servidor: \n %s"), buffer);
	_puttchar('\n');

	CloseHandle(hClientPipe);

	return 0;


}