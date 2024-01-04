#include "Shop.h"

extern Player* player;

std::vector<Shop*> shop;
int ShopChoice[3];



Shop::Shop(TCHAR* name, int maxcnt, void(*message)(Shop*), void(*upgrade)(Shop*))//TODO:_name ÃÊ±âÈ­
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_parent = NULL;
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}
Shop::Shop(TCHAR* name, int maxcnt, void(*message)(Shop*), void(*upgrade)(Shop*), Shop* parent)//TODO:_name ÃÊ±âÈ­
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_parent = parent;
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}

bool DrawRoundRectangle(HDC hdc, RECT rt, int curve)
{
	if(rt.right-rt.left<curve*2)return false;
	
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

	return true;
}
bool IsCollideInRoundRectangle(int x,int y, RECT rt, int curve)
{
	return (rt.left<=x&&x<=rt.right&&rt.top<=y&&y<=rt.bottom)&&(
		pow(min((x>=rt.left?x-rt.left:curve+1),(x<=rt.right?rt.right-x:curve+1)),2) + 
		pow(min((y>=rt.top?y-rt.top:curve+1),(y<=rt.bottom?rt.bottom-y:curve+1)),2)
		<= curve*curve);
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
