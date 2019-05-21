#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\Dll\Dll.h"
#include "resource.h"

#define TODO TEXT("Under Development")

DWORD WINAPI ReceiveGame(LPVOID);

//Function responsible for handling the main windows events
LRESULT CALLBACK WindowEventsHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//Function responsible for handling the about event
LRESULT CALLBACK AboutEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//Function responsible for handling the login event
LRESULT CALLBACK LoginEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);