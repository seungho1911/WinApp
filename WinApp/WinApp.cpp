// WinApp.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WinApp.h"
#include "Game.h"
#include "Shop.h"
#include <stdio.h>
#include <time.h>
#include <cstdlib>

#define MAX_LOADSTRING 100

// 전역 변수:
extern std::vector <BulletObj* > bullets;
extern std::vector<MobObj*> enemys;
extern Player* player;
extern BarrierObj* barrier;
extern RECT rtMapSize;
extern std::vector<Shop*> shop;
extern int ShopChoice[3];

HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
WCHAR szWindowClass_Shop[MAX_LOADSTRING] = _T("game shop window");
WCHAR szWindowClass_Barrier[MAX_LOADSTRING] = _T("game barrier window");
BOOL bShopOpen = false;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE, LRESULT(*)(HWND, UINT, WPARAM, LPARAM), WCHAR*);
BOOL                InitInstance(HINSTANCE, int);
BOOL                InitInstance_Shop(HINSTANCE, int);
BOOL                InitInstance_Barrier(HINSTANCE, int);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc_Shop(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc_Barrier(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	// TODO: 여기에 코드를 입력합니다.
	
	//콘솔창 살리기
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
	
	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINAPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, WndProc, szWindowClass);
	MyRegisterClass(hInstance, WndProc_Shop, szWindowClass_Shop);
	MyRegisterClass(hInstance, WndProc_Barrier, szWindowClass_Barrier);

	srand(time(NULL));
	
	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPP));
	
	MSG msg;
	
	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	FreeConsole();
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance, LRESULT (*WndProc)(HWND, UINT, WPARAM, LPARAM), WCHAR* name)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_WINAPP);
	wcex.lpszClassName = name;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP|WS_VISIBLE,//WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 200,200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
BOOL InitInstance_Shop(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd = CreateWindowW(szWindowClass_Shop, szTitle, WS_POPUP|WS_VISIBLE,//WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, 0, 1200,900, nullptr, nullptr, hInstance, nullptr);
	
	if (!hWnd)
	{
		return FALSE;
	}
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	return TRUE;
}


void DrawRoundRectangle(HDC hdc, RECT rt, int curve)
{
	MoveToEx(hdc, rt.left, rt.top + curve, nullptr);
	LineTo(hdc, rt.left, rt.bottom - curve);
	MoveToEx(hdc, rt.right, rt.top + curve, nullptr);
	LineTo(hdc, rt.right, rt.bottom - curve);
	MoveToEx(hdc, rt.left + curve, rt.top, nullptr);
	LineTo(hdc, rt.right - curve, rt.top);
	MoveToEx(hdc, rt.left + curve, rt.bottom, nullptr);
	LineTo(hdc, rt.right - curve, rt.bottom);
	Ellipse(hdc, rt.left, rt.top, rt.left + curve*2, rt.top + curve*2);
	Ellipse(hdc, rt.right - curve*2, rt.top, rt.right, rt.top + curve*2);
	Ellipse(hdc, rt.left, rt.bottom - curve*2, rt.left + curve*2, rt.bottom);
	Ellipse(hdc, rt.right - curve*2, rt.bottom - curve*2, rt.right, rt.bottom);
}

//Frame Func
void UpdateFrame(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc)
{
    GetWindowRect(hWnd, &rtMapSize);

    RunFrame();
    
    MoveWindow(hWnd, rtMapSize.left, rtMapSize.top, rtMapSize.right - rtMapSize.left, rtMapSize.bottom - rtMapSize.top, true);
}

void MobSpawn(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc)
{
	SpawnEnemy();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 1, (TIMERPROC)UpdateFrame);
        SetTimer(hWnd, 2, 1000, (TIMERPROC)MobSpawn);
        Init();
        InitShop();
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case 0x50:
        case 0x70:
			if (!bShopOpen){
                InitInstance_Shop(hInst, 10);
			    bShopOpen = true;
            }
            break;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rtWindow;
        GetWindowRect(hWnd, &rtWindow);

		HPEN myPen = CreatePen(PS_SOLID, 7, RGB(0, 255, 0));
		HGDIOBJ oldPen = SelectObject(hdc, myPen);

		player->Draw(hdc);

		SelectObject(hdc, oldPen);
		DeleteObject(myPen);
        
        for (auto i : enemys) {
            i->Draw(hdc);
        }
        for (auto i : bullets) {
            i->Draw(hdc);
        }

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
LRESULT CALLBACK WndProc_Shop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont100,hFont50,hFont30;
	static int scroll=0;
	switch (message)
	{
	case WM_CREATE:
		hFont100 = CreateFont(100, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("명조"));
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
		break; //"break;" is out of parentheses in window example code
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT TextArea;
		GetClientRect(hWnd, &TextArea);
	
		HFONT hOldFont = (HFONT)SelectObject(hdc, hFont100);
		DrawText(hdc, _T("Shop"), -1, &TextArea, DT_CENTER);
		
		int k=0;
		for(int i : ShopChoice){
			int scnt = shop[i]->GetCnt();
			int smaxcnt = shop[i]->GetMaxcnt();
			int center = 300 + k*300;
			int width = 200/smaxcnt;

			//square
			SetDCPenColor(hdc, RGB(0, 0, 0));
			DrawRoundRectangle(hdc, { center-150,250,center+150,800 }, 50);
			
			//text
			SelectObject(hdc, hFont50);
			TextArea = {center-130, 300, center+150, 350};
			DrawText(hdc, shop[i]->GetName(), -1, &TextArea, DT_CENTER);
			SelectObject(hdc, hFont30);
			TextArea = {center-130, 450, center+150, 750};
			TCHAR* view = shop[i]->GetDescription();
			int lee = lstrlen(view);
			DrawText(hdc, shop[i]->GetDescription(), -1, &TextArea, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
			
			//bar
			for (int j = 0; j<smaxcnt; j++) {
				if(j<scnt)SetDCPenColor(hdc, RGB(200, 0, 0));  //alternative : use graypen and not fill
				else if(j==scnt)SetDCPenColor(hdc, RGB(0, 0, 200));
				else SetDCPenColor(hdc, RGB(0, 200, 0));
				Rectangle(hdc, center - smaxcnt*width/2 + j*width, 700, center - smaxcnt*width/2 + j*width, 750);///not work
			}
			
			//upgradebutton
			k++;
		}
		
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		bShopOpen = false;
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK WndProc_Barrier(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hBrush;
	switch (message)
	{
	case WM_CREATE:
		hBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(255, 0, 0));
		break;
	case WM_KEYDOWN:
		break;
	case WM_MOVE:
		barrier->MovePos(POINT({ 1, 1 }));
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
	
		POINT pos = barrier->GetPos();
		int r = barrier->GetR();
	
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		SetDCPenColor(hdc, RGB(255, 0, 0));
		Rectangle(hdc, pos.x - r/2, pos.y - r/2, pos.x + r / 2, pos.y + r / 2);
		SelectObject(hdc, hOldBrush);
	
		//Not End
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
