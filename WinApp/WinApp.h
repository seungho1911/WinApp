#pragma once

#include "resource.h"

#include "Game.h"
#include "Shop.h"
#include "WinProc.h"
#include <stdio.h>
#include <time.h>
#include <cstdlib>

ATOM MyRegisterClass(HINSTANCE hInstance, LRESULT(*WndProc)(HWND, UINT, WPARAM, LPARAM), WCHAR* name);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
BOOL InitInstance_Shop(HINSTANCE hInstance, int nCmdShow);
HWND InitInstance_Barrier(HINSTANCE hInstance, POS pos, int size, int nCmdShow);