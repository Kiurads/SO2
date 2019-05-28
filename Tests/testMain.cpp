#include <windows.h>
#include <tchar.h>
/* ===================================================== */
/* Programa base (esqueleto) para aplica��es Windows */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
// Modelo para programas Windows:
// Composto por 2 fun��es:
// WinMain() = Ponto de entrada dos programas windows
// 1) Define, cria e mostra a janela
// 2) Loop de recep��o de mensagens provenientes do Windows
// TrataEventos()= Processamentos da janela (pode ter outro nome)
// 1) � chamada pelo Windows (callback)
// 2) Executa c�digo em fun��o da mensagem recebida
LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GereEventosDialog(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

// Nome da classe da janela (para programas de uma s� janela, normalmente este nome �
// igual ao do pr�prio programa) "szprogName" � usado mais abaixo na defini��o das
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Base");
// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()que desempenha
// o papel da fun��o main() do C em modo consola WINAPI indica o "tipo da fun��o" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as fun��es de
// processamento da janela)
// Par�metros:
// hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa
// hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
// lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0
// destinada a conter par�metros para o programa
// nCmdShow: Par�metro que especifica o modo de exibi��o da janela (usado em
// ShowWindow()

DWORD WINAPI ThreadFunc(LPVOID param);

HWND global_hWnd = NULL;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd; // hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX � uma estrutura cujos membros servem para
   // definir as caracter�sticas da classe da janela
   // ============================================================================
   // 1. Defini��o das caracter�sticas da janela "wcApp"
   // (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
   // ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Inst�ncia da janela actualmente exibida
   // ("hInst" � par�metro de WinMain e vem
   // inicializada da�)
	wcApp.lpszClassName = szProgName; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endere�o da fun��o de processamento da janela
   // ("TrataEventos" foi declarada no in�cio e
   // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for
   // (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = CreateSolidBrush(RGB(255, 0, 0)); // UPDATED
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por
	// "GetStockObject".Neste caso o fundo ser� branco
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName, // Nome da janela (programa) definido acima
		TEXT("SO2 - Ficha 6 - Exerc�cio 1"),// Texto que figura na barra do t�tulo UPDATED
		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
		5, // Posi��o x pixels (default=� direita da �ltima) UPDATED
		5, // Posi��o y pixels (default=abaixo da �ltima) UPDATED
		800, // Largura da janela (em pixels) UPDATED
		600, // Altura da janela (em pixels) UPDATED
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
		// outra) ou HWND_DESKTOP se a janela for a primeira,
		// criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da inst�ncia do programa actual ("hInst" �
		// passado num dos par�metros de WinMain()
		0); // N�o h� par�metros adicionais para a janela
	// ============================================================================
	// 4. Mostrar a janela
	// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
   // "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e.
   // normal/modal); � passado como par�metro de WinMain()

	global_hWnd = hWnd;

	UpdateWindow(hWnd); // Refrescar a janela (Windows envia � janela uma
   // mensagem para pintar, mostrar dados, (refrescar)�
   // ============================================================================
   // 5. Loop de Mensagens
   // ============================================================================
   // O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa fila de
   // espera at� que GetMessage(...) possa ler "a mensagem seguinte"
   // Par�metros de "getMessage":
   // 1)"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no
   // in�cio de WinMain()):
   // HWND hwnd handler da janela a que se destina a mensagem
   // UINT message Identificador da mensagem
   // WPARAM wParam Par�metro, p.e. c�digo da tecla premida
   // LPARAM lParam Par�metro, p.e. se ALT tamb�m estava premida
   // DWORD time Hora a que a mensagem foi enviada pelo Windows
   // POINT pt Localiza��o do mouse (x, y)
   // 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem
   // receber as mensagens para todas as janelas pertencentes � thread actual)
   // 3)C�digo limite inferior das mensagens que se pretendem receber
   // 4)C�digo limite superior das mensagens que se pretendem receber
   // NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
   // terminando ent�o o loop de recep��o de mensagens, e o programa

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, 0, NULL);

	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		TranslateMessage(&lpMsg); // Pr�-processamento da mensagem (p.e. obter c�digo
	   // ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
	   // aguarda at� que a possa reenviar � fun��o de
	   // tratamento da janela, CALLBACK TrataEventos (abaixo)
	}
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam); // Retorna sempre o par�metro wParam da estrutura lpMsg
}
// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Esta fun��o pode ter um nome qualquer: Apenas � neces�rio que na inicializa��o da
// estrutura "wcApp", feita no in�cio de WinMain(), se identifique essa fun��o. Neste
// caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pr�-processadas
// no loop "while" da fun��o WinMain()
//
// Par�metros:
// hWnd O handler da janela, obtido no CreateWindow()
// messg Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
// wParam O par�metro wParam da estrutura messg (a mensagem)
// lParam O par�metro lParam desta mesma estrutura
//
// NOTA:Estes par�metros est�o aqui acess�veis o que simplifica o acesso aos seus valores
//
// A fun��o EndProc � sempre do tipo "switch..." com "cases" que descriminam a mensagem
// recebida e a tratar. Estas mensagens s�o identificadas por constantes (p.e.
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================

int xPos = 0, yPos = 0;
TCHAR c = '?';
TCHAR frase[200];

int maxX = 0, maxY = 0;
HDC memDC = NULL;
HBITMAP hBit = NULL;
HBRUSH hBrush = NULL;
HDC tempDC = NULL;
HBITMAP hBmp = NULL;
BITMAP bmp;

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	RECT rect;

	switch (messg)
	{
	case WM_CREATE:

		maxX = GetSystemMetrics(SM_CXSCREEN);
		maxY = GetSystemMetrics(SM_CYSCREEN);

		hDC = GetDC(hWnd);

		memDC = CreateCompatibleDC(hDC);

		hBit = CreateCompatibleBitmap(hDC, maxX, maxY);

		SelectObject(memDC, hBit);

		DeleteObject(hBit);

		hBrush = CreateSolidBrush(RGB(70, 150, 70));

		SelectObject(memDC, hBrush);

		PatBlt(memDC, 0, 0, maxX, maxY, PATCOPY);

		ReleaseDC(hWnd, hDC);

		//hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), 
		//	MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

		hBmp = (HBITMAP)LoadImage(NULL, TEXT("cavaleiro.bmp"),
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		GetObject(hBmp, sizeof(bmp), &bmp);

		break;

	case WM_LBUTTONDOWN:

		xPos = LOWORD(lParam); // Alternativa!
		yPos = HIWORD(lParam); // Alternativa!

		InvalidateRect(hWnd, NULL, TRUE);

		break;

	case WM_CHAR:

		c = (TCHAR)wParam;

		InvalidateRect(hWnd, NULL, TRUE);

		break;

	case WM_PAINT:

		tempDC = CreateCompatibleDC(memDC);

		SelectObject(tempDC, hBmp);

		PatBlt(memDC, 0, 0, maxX, maxY, PATCOPY);

		BitBlt(memDC, 25, 25, bmp.bmWidth, bmp.bmHeight, tempDC, 0, 0, SRCCOPY);

		DeleteDC(tempDC);

		GetClientRect(hWnd, &rect);
		SetTextColor(memDC, RGB(255, 255, 255));
		SetBkMode(memDC, TRANSPARENT);
		rect.left = xPos;
		rect.top = yPos;
		DrawText(memDC, &c, 1, &rect, DT_SINGLELINE | DT_NOCLIP);

		hDC = BeginPaint(hWnd, &ps);

		BitBlt(hDC, 0, 0, maxX, maxY, memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);

		break;

	case WM_ERASEBKGND:
		return(1); // Prevent erasing the background to reduce flickering
		break;

	case WM_CLOSE:

		if (MessageBox(hWnd, TEXT("Tem a certeza que quer sair?"),
			TEXT("Sair"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			DestroyWindow(hWnd);
		}

		break;
	case WM_DESTROY: // Destruir a janela e terminar o programa

		DeleteDC(memDC);
		DeleteObject(hBrush);

		global_hWnd = NULL;

		PostQuitMessage(0);
		break;
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar")
		// n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows
		return DefWindowProc(hWnd, messg, wParam, lParam);
		break;
	}
	return(0);
}

LRESULT CALLBACK GereEventosDialog(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR login[100], password[100];

	switch (messg)
	{
	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{

			_stprintf_s(frase, 200, TEXT("%s :: %s"), login, password);

			EndDialog(hWnd, 0);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			_stprintf_s(frase, 200, TEXT(""));

			EndDialog(hWnd, 0);
			return TRUE;
		}

		break;

	case WM_CLOSE:

		EndDialog(hWnd, 0);
		return TRUE;

	default:
		return FALSE;
	}

	return FALSE;
}

DWORD WINAPI ThreadFunc(LPVOID param)
{
	while (global_hWnd != NULL)
		InvalidateRect(global_hWnd, NULL, TRUE);

	return 0;
}