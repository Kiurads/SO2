#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

//Definir uma constante para facilitar a leitura do prot�tipo da fun��o
//Este .h deve ser inclu�do no projeto que o vai usar (modo impl�cito)

#define TOP 10
#define TAM 50
#define TTAM 100
#define BUFFER_MAX_SIZE 4096
#define MAX_X 10
#define MAX_Y 15
#define	GAME_CHANGED_EVENT_NAME TEXT("ChangeEvent")
#define LOGIN_FILE_NAME TEXT("LoginFile")
#define LOGIN_MUTEX_NAME TEXT("LoginMutex")
#define LOGIN_EVENT_NAME TEXT("LoginEvent")
#define LOGGED_EVENT_NAME TEXT("LoggedEvent")
#define GAME_FILE_NAME TEXT("MappedFile")
#define	GAME_READ_EVENT	TEXT("ReadEvent")
#define	GAME_HAS_READ_EVENT	TEXT("HasReadEvent")
#define SERVER_PIPE_NAME TEXT("\\\\.\\pipe\\server")
#define LOGIN_SUCCESS TEXT("1")

typedef struct {
	TCHAR tUsername[TAM];
	int hiScore;
	HANDLE hReadEvent;
	HANDLE hHasReadEvent;
	TCHAR tReadEventName[TTAM];
	TCHAR tHasReadEventName[TTAM];
} player, *pPlayer;

typedef struct {
	TCHAR tUsername[TAM];
	int hiScore;
} topPlayer;

typedef struct {
	int x;
	int y;
	int speed;
} ball;

typedef struct {
	int pos;
	int size;
} bar;

typedef struct {
	ball gameBall;
	bar gameBar;
	int points;
	int isRunning;
} game, *pGame;

//Esta macro é definida pelo sistema caso estejamos na DLL (<DLL_IMP>_EXPORTS definida)
//ou na app (<DLL_IMP>_EXPORTS não definida) onde DLL_IMP é o nome deste projeto

#ifdef DLL_EXPORTS //macro depende do nome do projeto
#define DLL_APIS __declspec(dllexport)
#else
#define DLL_APIS __declspec(dllimport)
#endif

extern "C"
{
	//Variáveis globais da DLL
	extern DLL_APIS HANDLE hGameChangedEvent;
	extern DLL_APIS HANDLE hGameMapFile;
	extern DLL_APIS game *gMappedGame;
	extern DLL_APIS HANDLE hReadEvent;
	extern DLL_APIS HANDLE hHasReadEvent;
	extern DLL_APIS HANDLE hLoginMapFile;
	extern DLL_APIS HANDLE hLoginEvent;
	extern DLL_APIS HANDLE hLoggedEvent;
	extern DLL_APIS TCHAR(*lpMessageBuffer)[BUFFER_MAX_SIZE];
	extern DLL_APIS	HANDLE hLoginMutex;
	extern DLL_APIS HANDLE hLoginPipe;
	extern DLL_APIS BOOL success;
	extern DLL_APIS DWORD nBytes;
	extern DLL_APIS TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
	extern DLL_APIS int iAuthReply;

	//Funções a serem exportadas/importadas
	DLL_APIS int Login(pPlayer data);
	DLL_APIS int ReceiveBroadcast(pGame gameData);
	DLL_APIS int SendMsg(void);
	DLL_APIS int ReceiveMessage(void);
	DLL_APIS void SetupClient(pPlayer data);
	DLL_APIS void CloseClient(void);
}