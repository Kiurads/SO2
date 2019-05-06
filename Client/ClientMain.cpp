#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\Dll\Dll.h"

bool termina = false;
HANDLE hThread;

DWORD WINAPI ReceiveBall(LPVOID);

int _tmain(int argc, LPTSTR argv) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	if (Login() == -1) {
		_gettchar();
		exit(-1);
	}

	hReadEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ReadEvent"));

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		MAPPED_FILE_NAME);

	lpMappedBuffer = (TCHAR(*)[BUFFER_MAX_SIZE])MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,  
		0,
		0,
		BUFFER_MAX_SIZE);


	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveBall, NULL, 0, NULL);

	_gettch();

	termina = TRUE;

	WaitForSingleObject(hThread, INFINITE);

	UnmapViewOfFile(lpMappedBuffer);
	CloseHandle(hMapFile);
	CloseHandle(hReadEvent);
	return 0;
}

DWORD WINAPI ReceiveBall(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(hReadEvent, 5000);

		if (dwWaitResult != WAIT_OBJECT_0) {
			_tprintf(TEXT("[ERRO] Conexão deu timeout\n"));
			termina = TRUE;
		}

		/*if (!ReadFile(hClientPipe, buffer, sizeof(buffer), &nBytes, NULL)) {
			_tprintf(TEXT("[ERRO] Erro na leitura de dados do servidor\n"));
			termina = TRUE;
		}*/

		//(*lpMappedBuffer)[_tcslen((*lpMappedBuffer)) / sizeof(TCHAR)] = '\0';
		_tprintf(TEXT("[SERVER] Posição da bola: %s\n"), lpMappedBuffer);
	}

	return 0;
}
