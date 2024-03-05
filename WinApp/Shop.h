#pragma once

#include "WinApp.h"
#include "Game.h"
#include <vector>
#include <time.h>
#include <cmath>
#include <cstdlib>

#define SHOPLENGTH 6
#define MAXCNTLENGTH 9
#define SHOPWIDTH 300
#define SHOPGAP 20
#define SHOPCURVE 50

class Shop
{
	int _maxcnt, _cnt, _delta;
	Shop* _parent;
	int _cost[MAXCNTLENGTH];
	TCHAR _name[MAXLENGTH], _description[MAXLENGTH], _detail[MAXLENGTH];
	void(*_message)(Shop*);
	void(*_upgrade)(Shop*);
public:
	Shop(TCHAR*, int, int, int*, void(*)(Shop*), void(*)(Shop*));
	Shop(TCHAR*, int, int, int*, void(*)(Shop*), void(*)(Shop*), Shop*);
	int GetMaxcnt() { return _maxcnt; }
	int GetCnt() { return _cnt; }
	int GetDelta() { return _delta; }
	int GetCost() { return _cost[_cnt]; }
	TCHAR* GetName() { return _name; }
	TCHAR* GetDescription() { return _description; }
	TCHAR* GetDetail() { return _detail; }
	void SetCnt(int cnt) { _cnt = cnt; }
	void UpdateMessage() { _message(this); }
	bool Upgrade();
	bool IsVisiable() { return _parent!=nullptr?_parent->_cnt != 0:true; }
};

class Button
{
	RECT _rt;
	int _curve;
public:
	Button();
	Button(RECT, int);
	RECT GetRt() { return _rt; }
	void Draw(HDC);
	bool IsCollide(POINT);
};

bool DrawRoundRectangle(HDC hdc, RECT rt, int curve);
//bool IsCollideInRoundRectangle(int x,int y, RECT rt, int curve);
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
void AddShieldMessage(Shop*);
void AddShieldUpgrade(Shop*);
void ShieldChargeMessage(Shop*);
void ShieldChargeUpgrade(Shop*);
void Blank(Shop*);
