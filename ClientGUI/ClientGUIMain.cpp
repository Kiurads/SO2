#include "ClientGUI.h"

HANDLE hGameThread = NULL;
game gameData;
player data;
bool termina = false;

HWND hWnd_global = NULL;

TCHAR szProgName[TAM] = TEXT("Breakout - Client");

TCHAR tPrintableMessage[200];

int maxX = 0, maxY = 0;
TCHAR playerScoreString[30];
HDC barDC, ballDC, loadingDC, memDC, brickDC, brindeDC;
HBITMAP hBit;
HBITMAP hBmpBar;
HBITMAP hBricks[5];
HBITMAP hBmpBall;
HBITMAP hBmpLoading;
HBITMAP hBmpLife;
HBITMAP hBmpSpeedUp;
HBITMAP hBmpSpeedDown;
HBITMAP hBmpTriple;
BITMAP bmpBar;
BITMAP bmpBall;
BITMAP bmpLoading;
BITMAP bmpBricks[5];
BITMAP bmpLife;
BITMAP bmpSpeedUp;
BITMAP bmpSpeedDown;
BITMAP bmpTriple;

HANDLE hServerPipe;
HANDLE hClientPipe;
BOOL isPipeSuccess = false;

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

	if (hGameThread != NULL)
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
		else {
			InvalidateRect(hWnd_global, NULL, TRUE);
		}
	}

	return 0;
}

LRESULT CALLBACK WindowEventsHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int value;
	RECT rect, playerScore;
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	TCHAR positions[50];

	switch (message) {
	case WM_CREATE:
		maxX = GetSystemMetrics(SM_CXSCREEN);
		maxY = GetSystemMetrics(SM_CYSCREEN);

		hdc = GetDC(hWnd);

		memDC = CreateCompatibleDC(hdc);

		hBit = CreateCompatibleBitmap(hdc, maxX, maxY);
		
		SelectObject(memDC, hBit);

		DeleteObject(hBit);

		hBrush = CreateSolidBrush(RGB(0, 0, 0));

		SelectObject(memDC, hBrush);

		PatBlt(memDC, 0, 0, maxX, maxY, PATCOPY);

		ReleaseDC(hWnd, hdc);

		hBmpBar = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BAR), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		hBmpBall = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BALL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		hBmpLoading = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_LOADING), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		hBricks[GREEN_BRICK] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_GREEN_BRICK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBricks[BLUE_BRICK] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BLUE_BRICK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBricks[RED_BRICK] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_RED_BRICK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBricks[PINK_BRICK] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_PINK_BRICK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBricks[ORANGE_BRICK] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_ORANGE_BRICK), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		hBmpLife = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_LIFE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBmpSpeedUp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_SPEED_UP), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBmpSpeedDown = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_SPEED_DOWN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		hBmpTriple = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TRIPLE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		GetObject(hBmpBar, sizeof(bmpBar), &bmpBar);

		GetObject(hBmpBall, sizeof(bmpBall), &bmpBall);
		
		GetObject(hBmpLoading, sizeof(bmpLoading), &bmpLoading);

		GetObject(hBricks[GREEN_BRICK], sizeof(bmpBricks[GREEN_BRICK]), &bmpBricks[GREEN_BRICK]);
		GetObject(hBricks[BLUE_BRICK], sizeof(bmpBricks[BLUE_BRICK]), &bmpBricks[BLUE_BRICK]);
		GetObject(hBricks[RED_BRICK], sizeof(bmpBricks[RED_BRICK]), &bmpBricks[RED_BRICK]);
		GetObject(hBricks[PINK_BRICK], sizeof(bmpBricks[PINK_BRICK]), &bmpBricks[PINK_BRICK]);
		GetObject(hBricks[ORANGE_BRICK], sizeof(bmpBricks[ORANGE_BRICK]), &bmpBricks[ORANGE_BRICK]);
		
		GetObject(hBmpLife, sizeof(bmpLife), &bmpLife);
		GetObject(hBmpSpeedUp, sizeof(bmpSpeedUp), &bmpSpeedUp);
		GetObject(hBmpSpeedDown, sizeof(bmpSpeedDown), &bmpSpeedDown);
		GetObject(hBmpTriple, sizeof(bmpTriple), &bmpTriple);
		break;

	case WM_PAINT:
		if (_tcslen(data.tUsername) > 0) {
			RECT rcBack;

			rcBack.left = 0;
			rcBack.top = 0;
			rcBack.right = gameData.max_x;
			rcBack.bottom = gameData.max_y;

			hBrush = CreateSolidBrush(RGB(255, 255, 255));
			PatBlt(memDC, 0, 0, maxX, maxY, PATCOPY);

			FillRect(memDC, &rcBack, hBrush);

			if (gameData.isRunning) {

				brickDC = CreateCompatibleDC(memDC);
				barDC = CreateCompatibleDC(memDC);
				ballDC = CreateCompatibleDC(memDC);
				brindeDC = CreateCompatibleDC(memDC);

				SelectObject(barDC, hBmpBar);  // colocar bitmap no DC
				SelectObject(ballDC, hBmpBall);  // colocar bitmap no DC
				for(int i = 0; i < gameData.nPlayers; i++)
					BitBlt(memDC, gameData.gameBar[i].pos, gameData.max_y - 8, bmpBar.bmWidth, bmpBar.bmHeight, barDC, 0, 0, SRCCOPY);

				for(int i = 0; i < TRIPLE; i++)
					BitBlt(memDC, gameData.gameBall[i].x, gameData.gameBall[i].y, bmpBall.bmWidth, bmpBall.bmHeight, ballDC, 0, 0, SRCAND);

				for (int i = 0; i < MAX_BRIX_HEIGHT; i++){
					for (int j = 0; j < MAX_BRIX_WIDTH; j++) {
						switch (gameData.brix[i][j].health) {
						case 1:
							if(gameData.brix[i][j].isSpecial)
								SelectObject(brickDC, hBricks[ORANGE_BRICK]);
							else
								SelectObject(brickDC, hBricks[GREEN_BRICK]);
								
							BitBlt(memDC, gameData.brix[i][j].posx, gameData.brix[i][j].posy, bmpBricks[GREEN_BRICK].bmWidth, bmpBricks[GREEN_BRICK].bmHeight, brickDC, 0, 0, SRCAND);
							break;
						case 2:
							SelectObject(brickDC, hBricks[BLUE_BRICK]);
							BitBlt(memDC, gameData.brix[i][j].posx, gameData.brix[i][j].posy, bmpBricks[BLUE_BRICK].bmWidth, bmpBricks[BLUE_BRICK].bmHeight, brickDC, 0, 0, SRCAND);
							break;
						case 3:
							SelectObject(brickDC, hBricks[RED_BRICK]);
							BitBlt(memDC, gameData.brix[i][j].posx, gameData.brix[i][j].posy, bmpBricks[RED_BRICK].bmWidth, bmpBricks[RED_BRICK].bmHeight, brickDC, 0, 0, SRCAND);
							break;
						case 4:
							SelectObject(brickDC, hBricks[PINK_BRICK]);
							BitBlt(memDC, gameData.brix[i][j].posx, gameData.brix[i][j].posy, bmpBricks[PINK_BRICK].bmWidth, bmpBricks[PINK_BRICK].bmHeight, brickDC, 0, 0, SRCAND);
							break;
						}
					}

				}

				for (int i = 0; i < MAX_BRINDES; i++) {
					if (gameData.brindes[i].isMoving == 1) {
						switch (gameData.brindes[i].type) {
						case SPEED_DOWN:
							SelectObject(brindeDC, hBmpSpeedDown);
							BitBlt(memDC, gameData.brindes[i].posx, gameData.brindes[i].posy, bmpSpeedDown.bmWidth, bmpSpeedDown.bmHeight, brindeDC, 0, 0, SRCAND);
							break;
						case SPEED_UP:
							SelectObject(brindeDC, hBmpSpeedUp);
							BitBlt(memDC, gameData.brindes[i].posx, gameData.brindes[i].posy, bmpSpeedUp.bmWidth, bmpSpeedUp.bmHeight, brindeDC, 0, 0, SRCAND);
							break;
						case TRIPLE:
							SelectObject(brindeDC, hBmpTriple);
							BitBlt(memDC, gameData.brindes[i].posx, gameData.brindes[i].posy, bmpTriple.bmWidth, bmpTriple.bmHeight, brindeDC, 0, 0, SRCAND);
							break;
						case EXTRA_LIFE:
							SelectObject(brindeDC, hBmpLife);
							BitBlt(memDC, gameData.brindes[i].posx, gameData.brindes[i].posy, bmpLife.bmWidth, bmpLife.bmHeight, brindeDC, 0, 0, SRCAND);
							break;
						}

						_stprintf_s(positions, 50, TEXT("~x: %d y: %d"), gameData.brindes[i].posx, gameData.brindes[i].posy);
					}
				}

				GetClientRect(hWnd, &playerScore);
				playerScore.top = 20;
				playerScore.left = MAX_X + 50;
				_stprintf_s(playerScoreString, 30, TEXT("%s Points: %d Life: %d"), data.tUsername, gameData.points, gameData.nLives);
				DrawText(memDC, playerScoreString, _tcsclen(playerScoreString), &playerScore, DT_SINGLELINE | DT_NOCLIP);

				DeleteDC(brindeDC);
				DeleteDC(brickDC);
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

		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);
		SetTextColor(memDC, RGB(255, 255, 255));
		SetBkMode(memDC, TRANSPARENT);
		rect.left = 0;
		rect.top = 0;
		
		BitBlt(hdc, 0, 0, maxX, maxY, memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_LOGIN:	//Login
			isRemote = 0;
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

		case ID_REMOTO:
			isRemote = 1;
			memset(tPrintableMessage, '\0', sizeof(TCHAR) * 200);

			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, (DLGPROC)LoginEventHandler);

			if (_tcslen(tPrintableMessage) == 0) {
				MessageBeep(MB_ICONERROR);
				MessageBox(hWnd, TEXT("Login falhou"), TEXT("ERRO"), MB_ICONERROR | MB_OK);
			}
			else {
				MessageBeep(MB_ICONINFORMATION);
				MessageBox(hWnd, tPrintableMessage, TEXT("Sucesso!"), MB_ICONINFORMATION | MB_OK);

				EnableMenuItem(GetMenu(hWnd), ID_REMOTO, MF_DISABLED);

				
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

			case VK_SPACE:
				SendMsg(data, (TCHAR*)SPACE);
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
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}
