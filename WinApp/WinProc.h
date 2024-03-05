#pragma once

#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <cstdlib>
#include "Shop.h"
#include "Game.h"

#define MAX_LOADSTRING 100

void UpdateFrame(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc);
void MobSpawn(HWND hWnd, UINT_PTR nID, UINT uElapse, TIMERPROC lpTimerFunc);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_Extra(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_Shop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_End(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class GrapicBuffer
{
private:
	RECT rtClient,rtWindow;
	HDC hdc, hMemdc;
	HWND hwnd;
	HBITMAP hBitmap, hOldBitmap;
	bool makehdc;
	bool paint;
	void ready()
	{
		GetClientRect(hwnd, &rtClient);
		GetClientRect(hwnd, &rtWindow);
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rtWindow.left)); // convert top-left
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rtWindow.right)); // convert bottom-right

		hMemdc = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rtClient.right, rtClient.bottom);
		hOldBitmap = (HBITMAP)SelectObject(hMemdc, hBitmap);

		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(hMemdc, &rtClient, hBrush);
		DeleteObject(hBrush);
	}
public:
	GrapicBuffer(HWND _hwnd)
	{
		hwnd = _hwnd;
		hdc = GetDC(hwnd);

		ready();

		paint = false;
		makehdc = true;
	}
	GrapicBuffer(HWND _hwnd, HDC _hdc)
	{
		hwnd = _hwnd;
		hdc = _hdc;

		ready();

		paint = false;
		makehdc = false;
	}
	~GrapicBuffer()
	{
		if(!paint)BitBlt(hdc, 0, 0, rtClient.right, rtClient.bottom, hMemdc, 0, 0, SRCCOPY);
		SelectObject(hMemdc, hOldBitmap);

		DeleteObject(hBitmap);
		DeleteDC(hMemdc);
		if(makehdc)ReleaseDC(hwnd, hdc);
	}
	HDC GetHDC() { return hMemdc; }
	HWND GetHWND() { return hwnd; }
	RECT* GetWindowRECT() { return &rtWindow; }
	RECT* GetClientRECT() { return &rtClient; }
	void EndPaint(){
		BitBlt(hdc, 0, 0, rtClient.right, rtClient.bottom, hMemdc, 0, 0, SRCCOPY);
		paint = true;
	}
};