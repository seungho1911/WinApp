#pragma once

#include "Game.h"
#include <time.h>
#include <cstdlib>

#define SHOPWIDTH 300
#define SHOPGAP 20

class Shop
{
	int _maxcnt, _cnt;
	Shop* _parent;
	TCHAR _name[MAXLENGTH], _description[MAXLENGTH];
	void(*_message)(Shop*);
	void(*_upgrade)(Shop*);
public:

	Shop(TCHAR*, int, void(*)(Shop*), void(*)(Shop*));
	Shop(TCHAR*, int, void(*)(Shop*), void(*)(Shop*), Shop*);
	int GetMaxcnt() { return _maxcnt; }
	int GetCnt() { return _cnt; }
	TCHAR* GetName() { return _name; }
	TCHAR* GetDescription() { return _description; }
	void SetCnt(int cnt) { _cnt = cnt; }
	void UpdateMessage() { _message(this); }
	void Upgrade() { _upgrade(this); _message(this); }
	bool IsVisiable() { return _parent->_cnt != 0; }
};

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
