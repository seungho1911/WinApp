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
	GetWindowRect(hWnd, &rtMainScreen);

	RunFrame();

	MoveWindow(hWnd, rtMainScreen.left, rtMainScreen.top, rtMainScreen.right - rtMainScreen.left, rtMainScreen.bottom - rtMainScreen.top, false);
	InvalidateRect(hWnd, nullptr, false);
}

void MobSpawn(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc)
{
	POS psEnemy = player->GetPos();
	psEnemy.x += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);
	psEnemy.y += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);

	//설정
	enemys.push_back((EnemyObj*)new Enemy3(1, 2, R / 2, 0, 10, psEnemy));
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
		_tcscat(thp, INTTOCHAR(player->GetHp()));
		_tcscat(thp, _T(" + "));
		_tcscat(thp, INTTOCHAR(player->GetShield()));
		DrawText(hdc, thp, -1, GBMain.GetClientRECT(), DT_TOP || DT_LEFT);
		SelectObject(hdc, oldfont);
		DeleteObject(font);

		DrawGame(hdc, *GBMain.GetWindowRECT());
		
		for (auto i : hWnds) {
			GrapicBuffer GB = GrapicBuffer(i);
			DrawGame(GB.GetHDC(), *GB.GetWindowRECT());
		}
		printf("%d\n", enemys.size());
		GBMain.EndPaint();
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_BOSS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	HWND hB1;
	static HFONT hFont80, hFont50, hFont30;
	static HBRUSH hBrushRed, hBrushGreen, hBrushBlue;
	static HPEN hPen5;
	static int scroll = 0;
	switch (message)
	{
	case WM_CREATE:
		hPen5 = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
		hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
		hFont80 = CreateFont(80, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		hFont50 = CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		hFont30 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
		UpdateShopChoice();
		break;
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

		int k = 0, windowcenter = rtwindow.right / 2;
		for (int i : ShopChoice) {
			int center = windowcenter + (k - 1) * (SHOPWIDTH + SHOPGAP);
			if (IsCollideInRoundRectangle(pt.x, pt.y, { center - SHOPWIDTH / 2 , 150, center + SHOPWIDTH / 2 ,700 }, SHOPCURVE)) {
				shop[i]->Upgrade();
				ShopChoice[k] = SHOPLENGTH;
				InvalidateRect(hWnd, nullptr, TRUE);

				return 0;
			}
			k++;
		}

		UpdateShopChoice();
		InvalidateRect(hWnd, nullptr, TRUE);

		break; //"break;" is out of parentheses in window example code
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

		hOldFont = (HFONT)SelectObject(hdc, hFont80);
		DrawText(hdc, _T("Upgrade"), -1, &TextArea, DT_CENTER);

		int k = 0, windowcenter = TextArea.right / 2;
		for (int i : ShopChoice) {
			int scnt = shop[i]->GetCnt();
			int smaxcnt = shop[i]->GetMaxcnt();
			int center = windowcenter + (k - 1) * (SHOPWIDTH + SHOPGAP);
			int barwidth = smaxcnt != 0 ? (SHOPWIDTH / 2) / smaxcnt : 0;

			//square
			hOldPen = (HPEN)SelectObject(hdc, hPen5);
			//hOldPen = (HPEN)SelectObject(hdc, GetStockObject(DC_PEN));
			if (i == SHOPLENGTH + 1)SetDCPenColor(hdc, RGB(50, 50, 50));
			else SetDCPenColor(hdc, RGB(0, 255, 0));
			DrawRoundRectangle(hdc, { center - SHOPWIDTH / 2,150,center + SHOPWIDTH / 2,700 }, SHOPCURVE);
			SelectObject(hdc, hOldPen);

			//text
			SelectObject(hdc, hFont50);
			TextArea = { center - SHOPWIDTH / 2, 200, center + SHOPWIDTH / 2, 250 };
			DrawText(hdc, shop[i]->GetName(), -1, &TextArea, DT_CENTER);
			SelectObject(hdc, hFont30);
			TextArea = { center - SHOPWIDTH / 2, 350, center + SHOPWIDTH / 2, 550 };
			DrawText(hdc, shop[i]->GetDescription(), -1, &TextArea, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
			TextArea = { center - SHOPWIDTH / 2, 600, center + SHOPWIDTH / 2, 700 };
			DrawText(hdc, shop[i]->GetDetail(), -1, &TextArea, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
			SelectObject(hdc, hOldFont);

			//bar
			hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
			for (int j = 0; j < smaxcnt; j++) {
				if (j < scnt)SelectObject(hdc, hBrushGreen);
				else if (j == scnt)SelectObject(hdc, hBrushBlue);
				else SelectObject(hdc, hBrushRed);
				Rectangle(hdc, center - smaxcnt * barwidth / 2 + j * barwidth, 260, center - smaxcnt * barwidth / 2 + (j + 1) * barwidth, 270);
			}
			SetDCBrushColor(hdc, RGB(255, 255, 255));
			SelectObject(hdc, hOldBrush);

			k++;
		}

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		bShopOpen = false;
		DeleteObject(hPen5);
		DeleteObject(hBrushRed);
		DeleteObject(hBrushGreen);
		DeleteObject(hBrushBlue);
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
LRESULT CALLBACK WndProc_Barrier(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*static HBRUSH hBrush;
	Object* tar = nullptr;
	if (message!=WM_CREATE&&barriers.find(hWnd) != barriers.end())tar = (barriers.find(hWnd))->second;
	//else tar = (barriers.insert({ hWnd, new Object(100, 0, 0, {500,500}) }).first)->second;
	else throw("tar is null");*/
	
	switch (message)
	{
	/*case WM_CREATE:
		hBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(255, 0, 0));
		barriers.insert({ hWnd, new Object(100, 0, 0, {500,500}) });
		//hWnds.push_back(hWnd);
		break;*/
	/*case WM_MOVE:
	{
		POINT pos = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
		tar->SetPos(pos);
		break;
	}*/
	/*case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		SetDCPenColor(hdc, RGB(255, 0, 0));
		barrier->Draw(hdc);
		SelectObject(hdc, hOldBrush);
		
		//Not End
		EndPaint(hWnd, &ps);
		break;
	}*/
	case WM_DESTROY:
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
