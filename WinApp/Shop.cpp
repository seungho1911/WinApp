#include "Shop.h"

extern Player* player;

std::vector<Shop*> shop;
int ShopChoice[3];



Shop::Shop(TCHAR* name, int maxcnt, void(*message)(Shop*), void(*upgrade)(Shop*))//TODO:_name 초기화
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_parent = NULL;
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}
Shop::Shop(TCHAR* name, int maxcnt, void(*message)(Shop*), void(*upgrade)(Shop*), Shop* parent)//TODO:_name 초기화
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_parent = parent;
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}


void InitShop()
{
	Shop* parent;
	srand(time(NULL));

	TCHAR title[MAXLENGTH];
	_tcscpy(title, _T("Add Shot"));
	shop.push_back(new Shop(title, 5, AddShotMessage, AddShotUpgrade));
	_tcscpy(title, _T("Shot Damage"));
	shop.push_back(new Shop(title, 5, ShotDamageMessage, ShotDamageUpgrade));
	_tcscpy(title, _T("Shot Reload"));
	shop.push_back(new Shop(title, 5, ShotReloadMessage, ShotReloadUpgrade));
	_tcscpy(title, _T("Add Barrier"));
	shop.push_back(new Shop(title, 1, AddBarrierMessage, AddBarrierUpgrade));
	parent = shop.back();
}
void UpdateShopChoice()
{
	bool check[SHOPLENGTH] = { false, };
	for (int i = 0; i < 3; i++) {
		int g = rand() % SHOPLENGTH;
		while (check[g] || shop[g]->GetMaxcnt() == shop[g]->GetCnt())g = rand() % SHOPLENGTH;
		check[g] = true;
		ShopChoice[i] = g;
		shop[g]->UpdateMessage();
	}
}
void AddShotMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int smaxcnt = subject->GetMaxcnt();
	TCHAR* sdescription = subject->GetDescription();

	TCHAR text[MAXLENGTH] = _T("Fire ");
	_tcscat(text, INTTOCHAR(scnt));
	if (scnt != smaxcnt) {
		_tcscat(text, _T("("));
		_tcscat(text, INTTOCHAR(scnt + 1));
		_tcscat(text, _T(")"));
	}
	_tcscat(text, (scnt == 1 ? _T(" bullet") : _T(" bullets")));
	_tcscpy(sdescription, text);
}
void AddShotUpgrade(Shop* subject)
{

}
void ShotDamageMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int smaxcnt = subject->GetMaxcnt();
	TCHAR* sdescription = subject->GetDescription();

	TCHAR text[MAXLENGTH] = _T("Shot damage is ");
	_tcscat(text, INTTOCHAR(player->GetDamage()));
	if (scnt != smaxcnt) {
		_tcscat(text, _T("("));
		_tcscat(text, INTTOCHAR(player->GetDamage() + 1));
		_tcscat(text, _T(")"));
	}
	_tcscpy(sdescription, text);
}
void ShotDamageUpgrade(Shop* subject)
{
	int scnt = subject->GetCnt();

	subject->SetCnt(scnt + 1);
	player->SetDamage(player->GetDamage() + 1);
}
void ShotReloadMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int smaxcnt = subject->GetMaxcnt();
	TCHAR* sdescription = subject->GetDescription();

	TCHAR text[MAXLENGTH] = _T("Reload speed is ");
	_tcscat(text, INTTOCHAR(player->GetCooltime()));
	if (scnt != smaxcnt) {
		_tcscat(text, _T("("));
		_tcscat(text, INTTOCHAR(player->GetCooltime() - 1));
		_tcscat(text, _T(")"));
	}
	_tcscat(text, _T("seconds"));
	_tcscpy(sdescription, text);
}
void ShotReloadUpgrade(Shop* subject)
{
	int scnt = subject->GetCnt();

	subject->SetCnt(scnt + 1);
	player->SetCooltime(player->GetCooltime() - 1);
}
void AddBarrierMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int smaxcnt = subject->GetMaxcnt();
	TCHAR* sdescription = subject->GetDescription();

	_tcscpy(sdescription, (scnt == 1 ? _T("(Add Barrier)") : _T("Add Barrier")));
}
void AddBarrierUpgrade(Shop* subject)
{

}