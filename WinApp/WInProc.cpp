#include "WinProc.h"
#include "WinApp.h"

extern std::vector <BulletObj* > bullets;
extern std::vector<EnemyObj*> enemys;
extern Player* player;
extern RECT rtMainScreen;
extern BarrierStats* barrier;
extern std::vector <BarrierObj*> barriers;
extern std::vector<Shop*> shop;
extern int ShopChoice[3];

BOOL bShopOpen = false;
HINSTANCE hInst;
std::vector<HWND> hWnds;

//Frame Func
void UpdateFrame(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc)
{
	if (bShopOpen)return;
	GetWindowRect(hWnd, &rtMainScreen);

	if (!RunFrame()) {
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc_End;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WINAPP));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = _T("GameResult");
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
		RegisterClassExW(&wcex);

		HWND hResultWnd = CreateWindowW(_T("GameResult"), _T("GameResult"), WS_POPUP | WS_VISIBLE,
			CW_USEDEFAULT, 0, 200, 200, nullptr, nullptr, hInst, nullptr);
		if (!hResultWnd)
		{
			throw("GameResult ERROR");
		}
		ShowWindow(hResultWnd, 10);
		UpdateWindow(hResultWnd);

		SendMessage(hWnd, WM_CLOSE, 0, 0); 
	}

	MoveWindow(hWnd, rtMainScreen.left, rtMainScreen.top, rtMainScreen.right - rtMainScreen.left, rtMainScreen.bottom - rtMainScreen.top, false);
	InvalidateRect(hWnd, nullptr, false);
}

void MobSpawn(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc)
{
	if (bShopOpen)return;
	POS psEnemy = player->GetPos();
	psEnemy.x += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);
	psEnemy.y += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);

	//설정
	enemys.push_back((EnemyObj*)new Enemy3(1, 2, R / 2, 0, 10, psEnemy));
	enemys.push_back((EnemyObj*)new Enemy1(1, 2, 20, 0, 6, psEnemy));
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 1, (TIMERPROC)UpdateFrame);
		SetTimer(hWnd, 2, 5000, (TIMERPROC)MobSpawn);
		Init();
		InitShop();
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x50:
		case 0x70:
			if (!bShopOpen) {
				InitInstance_Shop(hInst, 10);
				bShopOpen = true;
			}
			break;
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hMaindc = BeginPaint(hWnd, &ps);

		GrapicBuffer GBMain = GrapicBuffer(hWnd, hMaindc);
		HDC hdc = GBMain.GetHDC();

		HFONT font = CreateFont(40, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		HGDIOBJ oldfont = SelectObject(hdc, font);
		TCHAR thp[MAXLENGTH] = _T("HP:");
		INTCAT(thp, player->GetHp());
		_tcscat(thp, _T(" + "));
		INTCAT(thp, player->GetShield());
		_tcscat(thp, _T("\nXP:"));
		INTCAT(thp, player->GetXp());
		DrawText(hdc, thp, -1, GBMain.GetClientRECT(), DT_TOP || DT_LEFT);
		SelectObject(hdc, oldfont);
		DeleteObject(font);

		DrawGame(hdc, *GBMain.GetWindowRECT());

		for (auto i : hWnds) {
			GrapicBuffer GB = GrapicBuffer(i);
			DrawGame(GB.GetHDC(), *GB.GetWindowRECT());
		}

		GBMain.EndPaint();
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		DestroyWindow(hWnd);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_Extra(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rtWindow;
		GetWindowRect(hWnd, &rtWindow);

		DrawGame(hdc, rtWindow);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_Shop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont80, hFont50, hFont30, hFont20;
	static HBRUSH hBrushRed, hBrushGreen, hBrushBlue, hBrushBlack, hBrushNull;
	static HPEN hPen5White, hPen2Cyan, hPen5Cyan;
	static int scroll = 0;
	static Button B_reset, B_upgrade[3];

	switch (message)
	{
	case WM_CREATE:
	{
		hPen5White = CreatePen(PS_SOLID, 5, RGB(255, 255, 255));
		hPen2Cyan = CreatePen(PS_SOLID, 2, RGB(0, 255, 255));
		hPen5Cyan = CreatePen(PS_SOLID, 5, RGB(0, 255, 255));
		hBrushNull = (HBRUSH)GetStockObject(NULL_BRUSH);
		hBrushRed = CreateSolidBrush(RGB(255, 0, 200));
		hBrushGreen = CreateSolidBrush(RGB(200, 255, 200));
		hBrushBlue = CreateSolidBrush(RGB(0, 200, 255));
		hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
		hFont80 = CreateFont(80, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		hFont50 = CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		hFont30 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		hFont20 = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));

		RECT rtwindow;
		GetClientRect(hWnd, &rtwindow);
		int windowcenter = rtwindow.right / 2;
		for (int i = 0; i < 3; i++) {
			int center = windowcenter + (i - 1) * (SHOPWIDTH + SHOPGAP);
			B_upgrade[i] = Button({ center - SHOPWIDTH / 2 , 150, center + SHOPWIDTH / 2 ,700 }, SHOPCURVE);
		}
		B_reset = Button({ windowcenter - 100,750,windowcenter + 100,850 }, 50);

		UpdateShopChoice();
		break;
	}
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x1B:
		case 0x50:
		case 0x70:
			bShopOpen = false;
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		RECT rtwindow;
		GetClientRect(hWnd, &rtwindow);

		for (int i=0;i<3;i++) {
			int choice = ShopChoice[i];
			if (B_upgrade[i].IsCollide(pt)) {
				if (!shop[choice]->Upgrade())return 0;
				ShopChoice[i] = SHOPLENGTH;
				InvalidateRect(hWnd, nullptr, TRUE);

				break;
			}
		}
		if(B_reset.IsCollide(pt)){
			UpdateShopChoice();
			InvalidateRect(hWnd, nullptr, TRUE);
		}

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT TextArea;

		HPEN hOldPen;
		HFONT hOldFont;
		HBRUSH hOldBrush;

		GetClientRect(hWnd, &TextArea);

		FillRect(hdc, &TextArea, hBrushBlack);
		SetBkMode(hdc, TRANSPARENT);

		hOldBrush = (HBRUSH)SelectObject(hdc, hBrushNull);
		hOldPen = (HPEN)SelectObject(hdc, hPen5Cyan);
		DrawRoundRectangle(hdc, { TextArea.left + 5,TextArea.top + 5,TextArea.right - 5,TextArea.bottom - 5 }, 30);
		SelectObject(hdc, hOldPen);

		SetTextColor(hdc, RGB(254, 253, 72));
		hOldFont = (HFONT)SelectObject(hdc, hFont80);
		TextArea.top -= 20;
		DrawText(hdc, _T("Upgrade"), -1, &TextArea, DT_CENTER);

		int k = 0, windowcenter = TextArea.right / 2;
		for (int i=0;i<3;i++) {
			int choice = ShopChoice[i];
			int scnt = shop[choice]->GetCnt();
			int smaxcnt = shop[choice]->GetMaxcnt();
			int barwidth = smaxcnt != 0 ? (SHOPWIDTH / 2) / smaxcnt : 0;
			RECT rt = B_upgrade[i].GetRt();

			//square
			hOldPen = (HPEN)SelectObject(hdc, hPen5White);
			B_upgrade[i].Draw(hdc);
			SelectObject(hdc, hOldPen);

			//text
			SelectObject(hdc, hFont50);
			TextArea = { rt.left, 200, rt.right, 250 };
			DrawText(hdc, shop[i]->GetName(), -1, &TextArea, DT_CENTER);

			SelectObject(hdc, hFont30);
			TextArea = { rt.left, 350, rt.right, 550 };
			DrawText(hdc, shop[i]->GetDescription(), -1, &TextArea, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);

			TCHAR tcost[MAXLENGTH] = _T("");
			INTCAT(tcost, shop[i]->GetCost());
			TextArea = { rt.left, 590, rt.right, 640 };
			DrawText(hdc, tcost, -1, &TextArea, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);

			SelectObject(hdc, hFont20);
			TextArea = { rt.left, 650, rt.right, 700 };
			DrawText(hdc, shop[i]->GetDetail(), -1, &TextArea, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
			SelectObject(hdc, hOldFont);

			//bar
			hOldPen = (HPEN)SelectObject(hdc, hPen2Cyan);
			for (int j = 0; j < smaxcnt; j++) {
				if (j < scnt)SelectObject(hdc, hBrushGreen);
				else if (j == scnt)SelectObject(hdc, hBrushBlue);
				else SelectObject(hdc, hBrushRed);
				Rectangle(hdc, (rt.left+rt.right)/2 - smaxcnt * barwidth / 2 + j * barwidth, 260, (rt.left + rt.right) / 2 - smaxcnt * barwidth / 2 + (j + 1) * barwidth, 270);
			}
			SelectObject(hdc, hOldPen);
			k++;
		}
		SetTextColor(hdc, 0x000000);

		SetDCBrushColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, hOldBrush);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		bShopOpen = false;
		DeleteObject(hPen5White);
		DeleteObject(hPen5Cyan);
		DeleteObject(hPen2Cyan);
		DeleteObject(hBrushNull);
		DeleteObject(hBrushRed);
		DeleteObject(hBrushGreen);
		DeleteObject(hBrushBlue);
		DeleteObject(hBrushBlack);
		DeleteObject(hFont20);
		DeleteObject(hFont30);
		DeleteObject(hFont50);
		DeleteObject(hFont80);
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_End(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HFONT font;
	switch (message)
	{
	case WM_CREATE:
	{
		font = CreateFont(40, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		RECT rtClient;
		GetClientRect(hWnd, &rtClient);
		
		HGDIOBJ oldfont = SelectObject(hdc, font);
		TCHAR thp[MAXLENGTH] = _T("Game Over");
		DrawText(hdc, thp, -1, &rtClient, DT_CENTER || DT_VCENTER);
		SelectObject(hdc, oldfont);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		DeleteObject(font);
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
