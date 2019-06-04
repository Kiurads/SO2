#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>  
#include "..\Dll\Dll.h"

int setupRegisty();
int setupServer();
DWORD WINAPI MessageThread(LPVOID);
DWORD WINAPI BallThread(LPVOID);
DWORD WINAPI GameThread(LPVOID);