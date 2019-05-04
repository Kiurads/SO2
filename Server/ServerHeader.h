#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "..\Dll\Dll.h"

int getLogin();
int setupRegisty();
int setupServerPipes();
int cmpfunc(const void * a, const void * b);