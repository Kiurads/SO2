#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

//Definir uma constante para facilitar a leitura do prot�tipo da fun��o
//Este .h deve ser inclu�do no projeto que o vai usar (modo impl�cito)

//Zona de dados do jogo
#define TOP 10
#define MAX_X 320
#define MAX_Y 200

//Zona de tamanhos para buffers
#define TAM 50
#define TTAM 100
#define BUFFER_MAX_SIZE 4096

//Zona de nomes para eventos
#define	GAME_CHANGED_EVENT_NAME TEXT("ChangeEvent")
#define LOGIN_EVENT_NAME TEXT("LoginEvent")
#define LOGGED_EVENT_NAME TEXT("LoggedEvent")
#define	GAME_READ_EVENT	TEXT("ReadEvent")
#define	GAME_HAS_READ_EVENT	TEXT("HasReadEvent")

//Zona para nomes de mapped files
#define MESSAGE_FILE_NAME TEXT("MessageFile")
#define GAME_FILE_NAME TEXT("MappedFile")

//Zona para nomes de mutexes
#define MESSAGE_MUTEX_NAME TEXT("MessageMutex")

//Zona para mensagens
#define LOGIN TEXT("Login")
#define	LEFT TEXT("Left")
#define RIGHT TEXT("Right")
#define EXIT TEXT("Exit")
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
	int	max_x;
	int max_y;
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
	extern DLL_APIS HANDLE hMessageMapFile;
	extern DLL_APIS HANDLE hMessageEvent;
	extern DLL_APIS HANDLE hLoggedEvent;
	extern DLL_APIS TCHAR(*lpMessageBuffer)[2][BUFFER_MAX_SIZE];
	extern DLL_APIS	HANDLE hMessageMutex;
	extern DLL_APIS HANDLE hLoginPipe;
	extern DLL_APIS BOOL success;
	extern DLL_APIS DWORD nBytes;
	extern DLL_APIS TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
	extern DLL_APIS int iAuthReply;

	//Funções a serem exportadas/importadas
	DLL_APIS int Login(pPlayer data);
	DLL_APIS int ReceiveBroadcast(pGame gameData);
	DLL_APIS int SendMsg(player data, TCHAR *msg);
	DLL_APIS int ReceiveMessage(void);
	DLL_APIS void SetupClient(pPlayer data, pGame gameData);
	DLL_APIS void CloseClient(void);
}