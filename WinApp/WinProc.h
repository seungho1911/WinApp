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
LRESULT CALLBACK WndProc_BOSS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_Shop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_Barrier(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);