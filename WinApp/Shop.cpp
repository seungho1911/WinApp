#include "Shop.h"

extern Player* player;
extern BarrierStats* barrier;
extern std::vector <BarrierObj*> barriers;

extern HINSTANCE hInst;

std::vector<Shop*> shop;
int ShopChoice[3];

int cost[SHOPLENGTH + 1][MAXCNTLENGTH+1] = { 
	{ 10,30,100,500,1000},
	{ 5,20,40,100, }, 
	{ 5,10,30,50, },
	{ 30 },
	{ 30 },
	{ 5,10,20,30,40,50,60,70,80 }, };

Shop::Shop(TCHAR* name, int maxcnt, int delta, int* cost, void(*message)(Shop*), void(*upgrade)(Shop*))//TODO:_name ÃÊ±âÈ­
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_delta = delta;
	_parent = nullptr;
	for (int i = 0; i < maxcnt; i++)_cost[i] = cost[i];
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	memset(_detail, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}
Shop::Shop(TCHAR* name, int maxcnt, int delta, int* cost, void(*message)(Shop*), void(*upgrade)(Shop*), Shop* parent)//TODO:_name ÃÊ±âÈ­
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_delta = delta;
	_parent = parent;
	for (int i = 0; i < maxcnt; i++)_cost[i] = cost[i];
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	memset(_detail, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}
bool Shop::Upgrade() {
	if(_cost[_cnt] > player->GetXp())return false;
	player->AddXp(-_cost[_cnt]);
	_upgrade(this);
	_cnt++;
	_message(this);
	return true;
}

Button::Button()
{
	_rt = { 0,0,0,0 };
	_curve = 0;
}
Button::Button(RECT rt, int curve)
{
	_rt = rt;
	_curve = curve;
}
void Button::Draw(HDC hdc)
{
	if (_rt.right - _rt.left < _curve * 2)return;

	MoveToEx(hdc, _rt.left, _rt.top + _curve, nullptr);
	LineTo(hdc, _rt.left, _rt.bottom - _curve);
	AngleArc(hdc, _rt.left + _curve, _rt.bottom - _curve, _curve, 180, 90);
	LineTo(hdc, _rt.right - _curve, _rt.bottom);
	AngleArc(hdc, _rt.right - _curve, _rt.bottom - _curve, _curve, 270, 90);
	LineTo(hdc, _rt.right, _rt.top + _curve);
	AngleArc(hdc, _rt.right - _curve, _rt.top + _curve, _curve, 0, 90);
	LineTo(hdc, _rt.left + _curve, _rt.top);
	AngleArc(hdc, _rt.left + _curve, _rt.top + _curve, _curve, 90, 90);
}
bool Button::IsCollide(POINT pos)
{
	return (_rt.left <= pos.x && pos.x <= _rt.right && _rt.top <= pos.y && pos.y <= _rt.bottom) && (
		((pos.x - _rt.left >= _curve && _rt.right - pos.x >= _curve) ||
			(pos.y - _rt.top >= _curve && _rt.bottom - pos.y >= _curve)) ||
		(pow(min(pos.x - _rt.left, _rt.right - pos.x), 2) + pow(min(pos.y - _rt.top, _rt.bottom - pos.y), 2) <= pow(_curve,2)));
}

bool DrawRoundRectangle(HDC hdc, RECT rt, int curve)
{
	if(rt.right-rt.left<curve*2)return false;
	
	MoveToEx(hdc, rt.left, rt.top + curve, nullptr);
	LineTo(hdc, rt.left, rt.bottom - curve);
	AngleArc(hdc, rt.left + curve, rt.bottom - curve, curve, 180, 90);
	LineTo(hdc, rt.right - curve, rt.bottom);
	AngleArc(hdc, rt.right - curve, rt.bottom - curve, curve, 270, 90);
	LineTo(hdc, rt.right, rt.top + curve);
	AngleArc(hdc, rt.right - curve, rt.top + curve, curve, 0, 90);
	LineTo(hdc, rt.left + curve, rt.top);
	AngleArc(hdc, rt.left + curve, rt.top + curve, curve, 90, 90);
	return true;
}
/*
bool IsCollideInRoundRectangle(int x,int y, RECT rt, int curve)
{
	return (rt.left<=x&&x<=rt.right&&rt.top<=y&&y<=rt.bottom)&&(
		((x - rt.left >= curve && rt.right - x >= curve) ||
		 (y - rt.top >= curve && rt.bottom - y >= curve))||
		(pow(min(x-rt.left,rt.right-x),2) + pow(min(y-rt.top,rt.bottom-y),2)<= curve*curve));
}*/

void InitShop()
{
	Shop* parent;
	srand(time(NULL));

	TCHAR title[MAXLENGTH];

	_tcscpy(title, _T("Add Shot"));
	shop.push_back(new Shop(title, 5, 1, cost[0], AddShotMessage, AddShotUpgrade));

	_tcscpy(title, _T("Shot Damage"));
	shop.push_back(new Shop(title, 5, 1, cost[1], ShotDamageMessage, ShotDamageUpgrade));

	_tcscpy(title, _T("Shot Reload"));
	shop.push_back(new Shop(title, 5, 10, cost[2], ShotReloadMessage, ShotReloadUpgrade));

	_tcscpy(title, _T("Add Barrier"));
	shop.push_back(new Shop(title, 2, 1, cost[3], AddBarrierMessage, AddBarrierUpgrade));
	parent = shop.back();

	_tcscpy(title, _T("Add Shield"));
	shop.push_back(new Shop(title, 1, 5000, cost[4], AddShieldMessage, AddShieldUpgrade));
	parent = shop.back();

	_tcscpy(title, _T("Shield Recharge Time"));
	shop.push_back(new Shop(title, 9, 500, cost[5], ShieldChargeMessage, ShieldChargeUpgrade, parent));


	_tcscpy(title, _T(""));
	shop.push_back(new Shop(title, 0, 0, cost[6], Blank, Blank));
}
void UpdateShopChoice()
{
	bool check[SHOPLENGTH] = { false, };

	int len = 0;
	for(int i = 0; i < SHOPLENGTH; i++){
		if (shop[i]->GetMaxcnt() == shop[i]->GetCnt())check[i] = true;
		else if (!shop[i]->IsVisiable())check[i] = true;
		else len++;
	}
	for (int i = 0; i < min(len, 3); i++) {
		int g = rand() % SHOPLENGTH;
		while (check[g])g = rand() % SHOPLENGTH;
		check[g] = true;
		ShopChoice[i] = g;
		shop[g]->UpdateMessage();
	}
	for(int i = len; i < 3; i++)ShopChoice[i] = 0;
}
void AddShotMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int smaxcnt = subject->GetMaxcnt();
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();

	TCHAR text[MAXLENGTH] = _T("Fire one");
	INTCAT(text, scnt);
	_tcscat(text, _T(" more shot"));
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Shot : "));
	INTCAT(text, scnt + 1);
	_tcscat(text, _T(" -> "));
	INTCAT(text, scnt + 2);
	_tcscpy(sdetail, text);
}
void AddShotUpgrade(Shop* subject)
{
	player->AddShot();
}
void ShotDamageMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int sdelta = subject->GetDelta();
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();

	TCHAR text[MAXLENGTH] = _T("Shot damage increases by  ");
	INTCAT(text, sdelta);
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Damage : "));
	INTCAT(text, player->GetDamage());
	_tcscat(text, _T(" -> "));
	INTCAT(text, player->GetDamage() + sdelta);
	_tcscpy(sdetail, text);
}
void ShotDamageUpgrade(Shop* subject)
{
	int scnt = subject->GetCnt();
	int sdelta = subject->GetDelta();

	player->SetDamage(player->GetDamage() + sdelta);
}
void ShotReloadMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int sdelta = subject->GetDelta();
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();

	TCHAR text[MAXLENGTH] = _T("Reload time is reduced by ");
	FLOATCAT(text, sdelta / 1000.0);
	_tcscat(text, _T(" seconds"));
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Reload : "));
	FLOATCAT(text, (player->GetCooltime() / 1000.0));
	_tcscat(text, _T(" -> "));
	FLOATCAT(text, (player->GetCooltime() - sdelta) / 1000.0);
	_tcscpy(sdetail, text);
}
void ShotReloadUpgrade(Shop* subject)
{
	int scnt = subject->GetCnt();
	int sdelta = subject->GetDelta();

	player->SetCooltime(player->GetCooltime() - sdelta);
}
void AddBarrierMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();
	TCHAR text[MAXLENGTH];

	_tcscpy(sdescription, _T("Creates a barrier that inflicts tick damage to enemies"));

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Barrier : "));
	INTCAT(text, scnt);
	_tcscat(text, _T(" -> "));
	INTCAT(text, scnt + 1);
	_tcscpy(sdetail, text);
}
void AddBarrierUpgrade(Shop* subject)
{
	barriers.push_back(new BarrierObj({ 500, 500 }));
}
void AddShieldMessage(Shop* subject) 
{
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();

	_tcscpy(sdescription, _T("Adds a rechargeable shield"));
	_tcscpy(sdetail, _T("Shield : X -> O"));
}
void AddShieldUpgrade(Shop* subject) 
{
	int sdelta = subject->GetDelta();

	player->SetShieldCooltime(sdelta);
}
void ShieldChargeMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int sdelta = subject->GetDelta();
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();

	TCHAR text[MAXLENGTH] = _T("Shield recharge time is reduced by ");
	FLOATCAT(text, (sdelta / 1000.0));
	_tcscat(text, _T(" seconds"));
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("ShieldCharge : "));
	FLOATCAT(text, (player->GetShieldCooltime() / 1000.0));
	_tcscat(text, _T(" -> "));
	FLOATCAT(text, ((player->GetShieldCooltime()-sdelta) / 1000.0));
	_tcscpy(sdetail, text);
}
void ShieldChargeUpgrade(Shop* subject)
{
	int sdelta = subject->GetDelta();
	player->SetShieldCooltime(player->GetShieldCooltime() - sdelta);
}

void Blank(Shop* subject)
{
	return;
}
