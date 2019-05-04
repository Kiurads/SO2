#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

//Definir uma constante para facilitar a leitura do prot�tipo da fun��o
//Este .h deve ser inclu�do no projeto que o vai usar (modo impl�cito)

#define TOP 10
#define SIZE 50
#define TAM 50
#define BUFFER_MAX_SIZE 256
#define SERVER_PIPE_NAME TEXT("\\\\.\\pipe\\server")
#define LOGIN_PIPE_NAME TEXT("\\\\.\\pipe\\login")
#define CLIENT_PIPE_NAME TEXT("\\\\.\\pipe\\client")
#define BROADCAST_PIPE_NAME TEXT("\\\\.\\pipe\\broadcast")
#define LOGIN_SUCCESS "1"

typedef struct {
	TCHAR tUsername[SIZE];
	int hiScore;
} player, *pPlayer;

//Esta macro � definida pelo sistema caso estejamos na DLL (<DLL_IMP>_EXPORTS definida)
//ou na app (<DLL_IMP>_EXPORTS n�o definida) onde DLL_IMP � o nome deste projeto

#ifdef DLL_EXPORTS //macro depende do nome do projeto
#define DLL_APIS __declspec(dllexport)
#else
#define DLL_APIS __declspec(dllimport)
#endif

extern "C"
{
	//Vari�veis globais da DLL
	extern DLL_APIS HANDLE hServerPipe;
	extern DLL_APIS HANDLE hLoginPipe;
	extern DLL_APIS HANDLE hClientPipe;
	extern DLL_APIS HANDLE hBroadcastPipe;
	extern DLL_APIS BOOL success;
	extern DLL_APIS DWORD nBytes;
	extern DLL_APIS TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
	extern DLL_APIS int iAuthReply;

	//Fun��es a serem exportadas/importadas
	DLL_APIS int Login(void);
	DLL_APIS int ReceiveBroadcast(void);
	DLL_APIS int SendMsg(void);
	DLL_APIS int ReceiveMessage(void);
}