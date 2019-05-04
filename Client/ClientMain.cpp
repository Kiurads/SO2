#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "Dll.h"

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

	/*if (SendMsg() == -1) {
		_gettchar();
		exit(-1);
	}*/

	_gettchar();

	return 0;
}
