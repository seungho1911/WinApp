#pragma once

#include "WinApp.h"
#include "Game.h"
#include <time.h>
#include <cmath>
#include <cstdlib>

#define SHOPWIDTH 300
#define SHOPGAP 20
#define SHOPCURVE 50

class Shop
{
	int _maxcnt, _cnt, _delta;
	Shop* _parent;
	TCHAR _name[MAXLENGTH], _description[MAXLENGTH], _detail[MAXLENGTH];
	void(*_message)(Shop*);
	void(*_upgrade)(Shop*);
public:
	Shop(TCHAR*, int, int, void(*)(Shop*), void(*)(Shop*));
	Shop(TCHAR*, int, int, void(*)(Shop*), void(*)(Shop*), Shop*);
	int GetMaxcnt() { return _maxcnt; }
	int GetCnt() { return _cnt; }
	int GetDelta() { return _delta; }
	TCHAR* GetName() { return _name; }
	TCHAR* GetDescription() { return _description; }
	TCHAR* GetDetail() { return _detail; }
	void SetCnt(int cnt) { _cnt = cnt; }
	void UpdateMessage() { _message(this); }
	void Upgrade() { _upgrade(this); _cnt++; _message(this); }
	bool IsVisiable() { return _parent->_cnt != 0; }
};

bool DrawRoundRectangle(HDC hdc, RECT rt, int curve);
bool IsCollideInRoundRectangle(int x,int y, RECT rt, int curve);
void InitShop();
void UpdateShopChoice();
void AddShotMessage(Shop*);
void AddShotUpgrade(Shop*);
void ShotDamageMessage(Shop*);
void ShotDamageUpgrade(Shop*);
void ShotReloadMessage(Shop*);
void ShotReloadUpgrade(Shop*);
void AddBarrierMessage(Shop*);
void AddBarrierUpgrade(Shop*);
void Blank(Shop*);
