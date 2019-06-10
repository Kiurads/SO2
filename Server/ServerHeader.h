#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>  
#include "..\Dll\Dll.h"
#include <strsafe.h>
#include <aclapi.h>

int setupRegisty();
int setupServer();
DWORD WINAPI MessageThread(LPVOID);
DWORD WINAPI BallThread(LPVOID);
DWORD WINAPI GameThread(LPVOID);
DWORD WINAPI BrindeThread(LPVOID lpParam);
void setupGame();
void Seguranca(SECURITY_ATTRIBUTES * sa);
void Cleanup(PSID pEveryoneSID, PSID pAdminSID, PACL pACL, PSECURITY_DESCRIPTOR pSD);