// WinApp.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WinApp.h"


#define BUTTON_RESET 1001

// 전역 변수:


extern HINSTANCE hInst;                                // 현재 인스턴스입니다.

WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
WCHAR szWindowClass_Shop[MAX_LOADSTRING] = _T("game shop window");
WCHAR szWindowClass_Barrier[MAX_LOADSTRING] = _T("game barrier window");



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


ATOM MyRegisterClass(HINSTANCE hInstance, LRESULT(*WndProc)(HWND, UINT, WPARAM, LPARAM), WCHAR* name)
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,//WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 200, 200, nullptr, nullptr, hInstance, nullptr);

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
	HWND hWnd = CreateWindowW(szWindowClass_Shop, szTitle, WS_POPUP | WS_VISIBLE,//WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1200, 900, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
HWND InitInstance_Barrier(HINSTANCE hInstance, int size, int nCmdShow)
{
	HWND hWnd = CreateWindowW(szWindowClass_Barrier, szTitle, WS_VISIBLE,//WS_OVERLAPPEDWINDOW,
		200-(size + 100)/2, 200-(size + 100)/2, size+100, size+100, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return nullptr;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}