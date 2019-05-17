#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\Dll\Dll.h"

int setupRegisty();
int setupServer();
int cmpfunc(const void * a, const void * b);
DWORD WINAPI LoginThread(LPVOID);
DWORD WINAPI BallThread(LPVOID);
DWORD WINAPI GameThread(LPVOID);