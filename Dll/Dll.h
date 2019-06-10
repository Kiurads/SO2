#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

//Definir uma constante para facilitar a leitura do prot�tipo da fun��o
//Este .h deve ser inclu�do no projeto que o vai usar (modo impl�cito)

//Pipes e Remote Client info

#define SERVER_PIPE_NAME TEXT("\\\\.\\pipe\\server")
#define CLIENT_PIPE_NAME TEXT("\\\\.\\pipe\\client")

//Zona de dados do jogo
#define TOP 10
#define MAX_X 320
#define MAX_Y 200
#define MAX_BRIX_WIDTH 10
#define MAX_BRIX_HEIGHT 10
#define MAX_SPEED -250000LL
#define MIN_SPEED -800000LL
#define INITIAL_LIVES 3
#define MAIN_BALL 0
#define MAX_PLAYERS 20

//Zona de tamanhos para buffers
#define TAM 50
#define TTAM 100
#define BUFFER_MAX_SIZE 4096
#define MAX_BRINDES 100

#define IMAGE_WIDTH 32
#define IMAGE_WIDTH_OFFSET 2
#define IMAGE_HEIGHT 8
#define IMAGE_HEIGHT_OFFSET 2
#define BALL_WIDTH 8
#define BALL_HEIGHT 8

#define DEAD -1
#define LFT -1
#define RGHT 1
#define DWN 1
#define UP -1

#define SPEED_UP 0
#define SPEED_DOWN 1
#define EXTRA_LIFE 2
#define TRIPLE 3

#define BRICK_ONE 1
#define BRICK_POINTS 10
#define BRICK_TWO 2
#define BRICK_THREE 3
#define BRICK_FOUR 4


#define GREEN_BRICK 0
#define BLUE_BRICK 1
#define RED_BRICK 2
#define PINK_BRICK 3
#define ORANGE_BRICK 4

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
#define SERVER_PIPE_MUTEX TEXT("ServerMutex")

//Zona para mensagens
#define LOGIN TEXT("Login")
#define	LEFT TEXT("Left")
#define RIGHT TEXT("Right")
#define SPACE TEXT("Space")
#define EXIT TEXT("Exit")
#define LOGIN_SUCCESS TEXT("1")

typedef struct {
	TCHAR tUsername[TAM];
	int hiScore;
	HANDLE hReadEvent;
	HANDLE hHasReadEvent;
	TCHAR tReadEventName[TTAM];
	TCHAR tHasReadEventName[TTAM];
	int isPlaying;
	int isRemote;
} player, *pPlayer;

typedef struct {
	TCHAR tUsername[TAM];
	int hiScore;
} topPlayer;

typedef struct {
	int x;
	int hspeed;
	int y;
	int vspeed;
	int speed;
	int	isMoving;
} ball;

typedef struct {
	int pos;
	int size;
	int lastTouch;
} bar;

typedef struct {
	int type;
	int posx, posy;
	int isMoving;
} brinde;

typedef struct {
	int posx, posy;
	int health;
	int isSpecial;
	int points;
	int dying;
} brick;

typedef struct {
	ball gameBall[TRIPLE];
	bar gameBar[MAX_PLAYERS];
	brick brix[MAX_BRIX_HEIGHT][MAX_BRIX_WIDTH];
	brinde brindes[MAX_BRINDES];
	int nPlayers;
	int points;
	int isRunning;
	int	max_x;
	int max_y;
	int nLives;
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
	extern DLL_APIS game *gGameData;
	extern DLL_APIS HANDLE hReadEvent;
	extern DLL_APIS HANDLE hHasReadEvent;
	extern DLL_APIS HANDLE hMessageMapFile;
	extern DLL_APIS HANDLE hMessageEvent;
	extern DLL_APIS HANDLE hLoggedEvent;
	extern DLL_APIS TCHAR(*lpMessageBuffer)[2][BUFFER_MAX_SIZE];
	extern DLL_APIS TCHAR RemoteMessage[2][BUFFER_MAX_SIZE];
	extern DLL_APIS	HANDLE hMessageMutex;
	extern DLL_APIS HANDLE hLoginPipe;
	extern DLL_APIS BOOL success;
	extern DLL_APIS DWORD nBytes;
	extern DLL_APIS TCHAR tName[TAM], buffer[BUFFER_MAX_SIZE];
	extern DLL_APIS int iAuthReply;
	extern DLL_APIS int isRemote;

	//Funções a serem exportadas/importadas
	DLL_APIS int Login(pPlayer data);
	DLL_APIS int ReceiveBroadcast(pGame gameData);
	DLL_APIS int SendMsg(player data, TCHAR *msg);
	DLL_APIS void SetupClient(pPlayer data, pGame gameData);
	DLL_APIS void CloseClient(void);

	DLL_APIS int ReceiveMessage(void);
}