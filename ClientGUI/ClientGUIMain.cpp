#include "ClientGUI.h"

HANDLE hGameThread = NULL;
game gameData;
player data;
bool termina = false;

HWND hWnd_global = NULL;

TCHAR szProgName[TAM] = TEXT("Breakout - Client");

TCHAR tPrintableMessage[200];

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	SetupClient(&data, &gameData);

	HWND hWnd;
	MSG lpMsg;
	WNDCLASSEX wcApp;

	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;
	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = WindowEventsHandler;

	wcApp.style = CS_HREDRAW | CS_VREDRAW;

	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_BREAKOUT));

	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_BREAKOUT));

	wcApp.hCursor = LoadCursor(hInst, IDC_ARROW);

	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	if (!RegisterClassEx(&wcApp))
		return(0);

	hWnd = CreateWindow(szProgName, TEXT("Breakout - Client"), WS_OVERLAPPEDWINDOW, 0, 0, 640, 360, (HWND)HWND_DESKTOP, (HMENU)NULL, (HINSTANCE)hInst, 0);

	ShowWindow(hWnd, nCmdShow);

	UpdateWindow(hWnd);

	hWnd_global = hWnd;

	while (GetMessage(&lpMsg, NULL, 0, 0) && !termina) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}

	termina = 1;

	if(hGameThread != NULL)
		WaitForSingleObject(hGameThread, INFINITE);

	CloseClient();
	return((int)lpMsg.wParam);
}

DWORD WINAPI ReceiveGame(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	while (!termina) {
		if (ReceiveBroadcast(&gameData) != 0) {
			_tprintf(TEXT("[TIMEOUT] A conexão foi perdida\n"));
			termina = 1;
		}
		else
			InvalidateRect(hWnd_global, NULL, TRUE);
	}

	return 0;
}

int maxX = 0, maxY = 0;
TCHAR frase[200];
HDC barDC, ballDC, loadingDC, memDC;
HBITMAP hBit;
HBITMAP hBmpBar;
HBITMAP hBmpBall;
HBITMAP hBmpLoading;
BITMAP bmpBar;
BITMAP bmpBall;
BITMAP bmpLoading;

LRESULT CALLBACK WindowEventsHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int value;
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;

	switch (message) {
	case WM_CREATE:
		maxX = GetSystemMetrics(SM_CXSCREEN);
		maxY = GetSystemMetrics(SM_CYSCREEN);

		hdc = GetDC(hWnd);

		memDC = CreateCompatibleDC(hdc);

		hBit = CreateCompatibleBitmap(hdc, maxX, maxY);
		
		SelectObject(memDC, hBit);

		DeleteObject(hBit);

		hBrush = CreateSolidBrush(RGB(255, 255, 255));

		SelectObject(memDC, hBrush);
		PatBlt(memDC, 0, 0, maxX, maxY, PATCOPY);

		ReleaseDC(hWnd, hdc);

		hBmpBar = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BAR), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBmpBall = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BALL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBmpLoading = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_LOADING), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		GetObject(hBmpBar, sizeof(bmpBar), &bmpBar);
		GetObject(hBmpBall, sizeof(bmpBall), &bmpBall);
		GetObject(hBmpLoading, sizeof(bmpLoading), &bmpLoading);
		break;

	case WM_PAINT:
		if (_tcslen(data.tUsername) > 0) {
			RECT rcBack;

			rcBack.left = 0;
			rcBack.top = 0;
			rcBack.right = gameData.max_x;
			rcBack.bottom = gameData.max_y;

			hBrush = CreateSolidBrush(RGB(120, 120, 120));
			PatBlt(memDC, 0, 0, maxX, maxY, PATCOPY);

			FillRect(memDC, &rcBack, hBrush);

			if (gameData.isRunning) {
				barDC = CreateCompatibleDC(memDC);
				ballDC = CreateCompatibleDC(memDC);

				SelectObject(barDC, hBmpBar);  // colocar bitmap no DC
				SelectObject(ballDC, hBmpBall);  // colocar bitmap no DC
				BitBlt(memDC, gameData.gameBar.pos, gameData.max_y - 8, bmpBar.bmWidth, bmpBar.bmHeight, barDC, 0, 0, SRCCOPY);
				BitBlt(memDC, gameData.gameBall.x, gameData.gameBall.y, bmpBall.bmWidth, bmpBall.bmHeight, ballDC, 0, 0, SRCAND);

				DeleteDC(barDC);
				DeleteDC(ballDC);
			}
			else {
				loadingDC = CreateCompatibleDC(memDC);

				SelectObject(loadingDC, hBmpLoading);  // colocar bitmap no DC
				BitBlt(memDC, ((gameData.max_x - bmpLoading.bmWidth) / 2), 32, bmpLoading.bmWidth, bmpLoading.bmHeight, loadingDC, 0, 0, SRCCOPY);

				DeleteDC(loadingDC);
			}
		}

		GetClientRect(hWnd, &rect);
		SetTextColor(memDC, RGB(255, 255, 255));
		SetBkMode(memDC, TRANSPARENT);
		rect.left = 0;
		rect.top = 0;

		hdc = BeginPaint(hWnd, &ps);

		BitBlt(hdc, 0, 0, maxX, maxY, memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_LOGIN:	//Login
			memset(tPrintableMessage, '\0', sizeof(TCHAR) * 200);

			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, (DLGPROC)LoginEventHandler);

			if (_tcslen(tPrintableMessage) == 0) {
				MessageBeep(MB_ICONERROR);
				MessageBox(hWnd, TEXT("Login falhou"), TEXT("ERRO"), MB_ICONERROR | MB_OK);
			}
			else {
				MessageBeep(MB_ICONINFORMATION);
				MessageBox(hWnd, tPrintableMessage, TEXT("Sucesso!"), MB_ICONINFORMATION | MB_OK);

				EnableMenuItem(GetMenu(hWnd), ID_LOGIN, MF_DISABLED);

				hGameThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveGame, NULL, 0, NULL);
			}

			break;

		case ID_SOBRE:	//Sobre
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hWnd, (DLGPROC)AboutEventHandler);
			break;

		case ID_SAIR:	//Sair
			MessageBeep(MB_ICONQUESTION);
			value = MessageBox(hWnd, TEXT("Tem a certea que pretende fechar?"), TEXT("Fechar"), MB_ICONQUESTION | MB_YESNO);

			if (value == IDYES) {
				if (_tcslen(data.tUsername) > 0)
					SendMsg(data, (TCHAR*)EXIT);

				DestroyWindow(hWnd);
			}
			break;
		}
		break;

	case WM_KEYDOWN:
		if (_tcslen(data.tUsername) > 0) {
			switch (LOWORD(wParam)) {
			case VK_LEFT:
				SendMsg(data, (TCHAR*)LEFT);
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			case VK_RIGHT:
				SendMsg(data, (TCHAR*)RIGHT);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
		}
		break;

	case WM_ERASEBKGND:
		return(1); // Prevent erasing the background to reduce flickering
		break;

	case WM_CLOSE:
		MessageBeep(MB_ICONQUESTION);
		value = MessageBox(hWnd, TEXT("Tem a certea que pretende fechar?"), TEXT("Fechar"), MB_ICONQUESTION | MB_YESNO);

		if (value == IDYES) {
			if (_tcslen(data.tUsername) > 0)
				SendMsg(data, (TCHAR*)EXIT);

			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return(0);
}

LRESULT CALLBACK LoginEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			GetDlgItemText(hWnd, IDC_USERNAME, data.tUsername, TAM);

			if (Login(&data) == 0 && _tcslen(data.tUsername) > 0) {
				_stprintf(tPrintableMessage, TEXT("Bem-vindo/a ao Breakout, %s!"), data.tUsername);

				hGameThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveGame, NULL, 0, NULL);
				InvalidateRect(hWnd_global, NULL, TRUE);
			}
			else
				memset(data.tUsername, '\0', sizeof(data.tUsername));

			EndDialog(hWnd, 0);
			return TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}

LRESULT CALLBACK AboutEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}
