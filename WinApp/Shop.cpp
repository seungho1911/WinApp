#include "Shop.h"

extern Player* player;
extern BarrierStats* barrier;
extern std::vector <BarrierObj*> barriers;

extern HINSTANCE hInst;

std::vector<Shop*> shop;
int ShopChoice[3];


Shop::Shop(TCHAR* name, int maxcnt, int delta, void(*message)(Shop*), void(*upgrade)(Shop*))//TODO:_name ÃÊ±âÈ­
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_delta = delta;
	_parent = NULL;
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	memset(_detail, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
}
Shop::Shop(TCHAR* name, int maxcnt, int delta, void(*message)(Shop*), void(*upgrade)(Shop*), Shop* parent)//TODO:_name ÃÊ±âÈ­
{
	_maxcnt = maxcnt;
	_cnt = 0;
	_delta = delta;
	_parent = parent;
	_tcscpy(_name, name);
	memset(_description, 0, sizeof(char) * MAXLENGTH);
	memset(_detail, 0, sizeof(char) * MAXLENGTH);
	_message = message;
	_upgrade = upgrade;
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
bool IsCollideInRoundRectangle(int x,int y, RECT rt, int curve)
{
	return (rt.left<=x&&x<=rt.right&&rt.top<=y&&y<=rt.bottom)&&(
		((x - rt.left >= curve && rt.right - x >= curve) ||
		 (y - rt.top >= curve && rt.bottom - y >= curve))||
		(pow(min(x-rt.left,rt.right-x),2) + pow(min(y-rt.top,rt.bottom-y),2)<= curve*curve));
}

void InitShop()
{
	Shop* parent;
	srand(time(NULL));

	TCHAR title[MAXLENGTH];
	_tcscpy(title, _T("Add Shot"));
	shop.push_back(new Shop(title, 5, 1, AddShotMessage, AddShotUpgrade));
	_tcscpy(title, _T("Shot Damage"));
	shop.push_back(new Shop(title, 5, 1, ShotDamageMessage, ShotDamageUpgrade));
	_tcscpy(title, _T("Shot Reload"));
	shop.push_back(new Shop(title, 5, 10, ShotReloadMessage, ShotReloadUpgrade));
	_tcscpy(title, _T("Add Barrier"));
	shop.push_back(new Shop(title, 2, 1, AddBarrierMessage, AddBarrierUpgrade));
	parent = shop.back();
	_tcscpy(title, _T("Add Shield"));
	shop.push_back(new Shop(title, 1, 5000, AddShieldMessage, AddShieldUpgrade));
	parent = shop.back();


	_tcscpy(title, _T(""));
	shop.push_back(new Shop(title, 0, 0, Blank, Blank));
}
void UpdateShopChoice()
{
	bool check[SHOPLENGTH] = { false, };

	int len = 0;
	for(int i = 0; i < SHOPLENGTH; i++){
		if (shop[i]->GetMaxcnt() != shop[i]->GetCnt())len++;
	}
	for (int i = 0; i < len; i++) {
		int g = rand() % SHOPLENGTH;
		while (check[g] || shop[g]->GetMaxcnt() == shop[g]->GetCnt())g = rand() % SHOPLENGTH;
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
	_tcscat(text, INTTOCHAR(scnt));
	_tcscat(text, _T(" more shot"));
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Shot : "));
	_tcscat(text, INTTOCHAR(scnt + 1));
	_tcscat(text, _T(" -> "));
	_tcscat(text, INTTOCHAR(scnt + 2));
	_tcscpy(sdetail, text);
}
void AddShotUpgrade(Shop* subject)
{

}
void ShotDamageMessage(Shop* subject)
{
	int scnt = subject->GetCnt();
	int sdelta = subject->GetDelta();
	TCHAR* sdescription = subject->GetDescription();
	TCHAR* sdetail = subject->GetDetail();

	TCHAR text[MAXLENGTH] = _T("Shot damage increases by  ");
	_tcscat(text, INTTOCHAR(sdelta));
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Damage : "));
	_tcscat(text, INTTOCHAR(player->GetDamage()));
	_tcscat(text, _T(" -> "));
	_tcscat(text, INTTOCHAR(player->GetDamage() + sdelta));
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
	_tcscat(text, INTTOCHAR(sdelta));
	_tcscat(text, _T(" seconds"));
	_tcscpy(sdescription, text);

	memset(text, 0, sizeof(char) * MAXLENGTH);
	_tcscat(text, _T("Reload : "));
	_tcscat(text, FLOATTOCHAR(player->GetCooltime() / 1000.0));
	_tcscat(text, _T(" -> "));
	_tcscat(text, FLOATTOCHAR((player->GetCooltime()-sdelta) / 1000.0));
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
	_tcscat(text, INTTOCHAR(scnt));
	_tcscat(text, _T(" -> "));
	_tcscat(text, INTTOCHAR(scnt + 1));
	_tcscpy(sdetail, text);
}
void AddBarrierUpgrade(Shop* subject)
{
	//InitInstance_Barrier(hInst, {500,500}, barrier->GetSize(), 10);
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
	player->SetSCooltime(subject->GetDelta());
}

void Blank(Shop* subject)
{
	return;
}
