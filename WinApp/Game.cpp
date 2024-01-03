#include "Game.h"

std::vector <BulletObj* > bullets;
std::vector<MobObj*> enemys;
Player* player;
BarrierObj* barrier;
std::vector<Shop*> shop;
RECT rtMapSize;

clock_t nLastTime;
int ShopChoice[3];

Object::Object(int size, double angle, double speed, POINT pos)
{
	_size = size;
	_angle = angle;
	_speed = speed;
	_pos = pos;
}

void Object::MoveAngleToDir()
{
	_pos.x += (double)cos(_angle) * _speed;
	_pos.y += (double)sin(_angle) * _speed;
}

void Object::Rotate(double angle)
{
	_angle += angle;
}
void Object::SetAngle(double angle)
{
	_angle = angle;
}

bool Object::IsCollide(Object t)
{
	POINT tpos = t.GetPos();
	long long int dis = (_pos.x - tpos.x) * (_pos.x - tpos.x) + (_pos.y - tpos.y) * (_pos.y - tpos.y);
		return dis <= (_size + t.GetR());
}
bool Object::IsCollide(Object* t)
{
	POINT tpos = t->GetPos();
	long long int dis = (_pos.x - tpos.x) * (_pos.x - tpos.x) + (_pos.y - tpos.y) * (_pos.y - tpos.y);
		return dis <= (_size + t->GetR())*(_size + t->GetR());
}

void Object::MoveOneFrame(int deltatime)
{
	throw "임시";
}

void Object::Draw(HDC hdc)
{
	Ellipse(hdc, (_pos.x - _size) - rtMapSize.left, (_pos.y - _size) - rtMapSize.top, (_pos.x + _size) - rtMapSize.left, (_pos.y + _size) - rtMapSize.top);
}


BulletObj::BulletObj(int damage, int speed, int size, double angle, POINT pos):Object(size,angle,speed,pos)
{
	_damage = damage;
	_speed = speed;
	_angle = angle;
	_pos = pos;
}
void BulletObj::MoveOneFrame(int deltatime)
{
	MoveAngleToDir();
}


MobObj::MobObj(int hp, int damage, int size, double angle, double speed, POINT pos):Object(size, angle, speed, pos)
{
	_hp = hp;
	_damage = damage;
}


Player::Player(int hp, int damage, int size, double angle, double speed, int cooltime, POINT pos)
	:MobObj(hp,damage,size,angle,speed,pos)
{
	_bulletcooltime = cooltime;
	_bulletlasttime = 0;
}

void Player::MoveOneFrame(int deltatime)
{
	POINT ptPlayer = player->GetPos();

	//Player Move
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		ptPlayer.x -= _speed;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		ptPlayer.x += _speed;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		ptPlayer.y -= _speed;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		ptPlayer.y += _speed;
	}

	//wall collsion check
	int MLeft = -1, MRight = -1, MTop = -1, MBottom = -1; //TODO:변수명

	if (ptPlayer.x - R < rtMapSize.left) {
		MLeft = +rtMapSize.left - (ptPlayer.x - R);
		ptPlayer.x += MLeft - 1;
	}
	if (ptPlayer.x + R > rtMapSize.right) {
		MRight = (ptPlayer.x + R) - rtMapSize.right;
		ptPlayer.x -= MRight - 1;
	}
	if (ptPlayer.y - R < rtMapSize.top) {
		MTop = rtMapSize.top - (ptPlayer.y - R);
		ptPlayer.y += MTop - 1;
	}
	if (ptPlayer.y + R > rtMapSize.bottom) {
		MBottom = (ptPlayer.y + R) - rtMapSize.bottom;
		ptPlayer.y -= MBottom - 1;
	}

	rtMapSize.left -= MLeft;
	rtMapSize.top -= MTop;
	rtMapSize.right += MRight;
	rtMapSize.bottom += MBottom;

	//Shot bullet
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && player->GetLasttime() <= 0) {
		POINT ptMouse, ptPlayer = player->GetPos();
		GetCursorPos(&ptMouse);
		bullets.push_back(new BulletObj(1, 5, R/2 ,atan2((ptMouse.y - ptPlayer.y), (ptMouse.x - ptPlayer.x)), ptPlayer));
		player->SetLasttime(player->GetCooltime());
	}

	player->SetLasttime(player->GetLasttime() - 1);
	player->SetPos(ptPlayer);
}


Enemy1::Enemy1(int hp, int damage, int size, double angle, double speed, POINT pos)
	:MobObj(hp, damage, size, angle, speed, pos)
{}

void Enemy1::MoveOneFrame(int deltatime)
{
	POINT ptPlayer = player->GetPos();
	SetAngle(atan2(-(_pos.y - ptPlayer.y), -(_pos.x - ptPlayer.x)));
	MoveAngleToDir();
}


void BarrierObj::MovePos(POINT pos)
{
	_pos = pos;
}

bool BarrierObj::IsCollide(Object t)
{
	POINT tpos = t.GetPos();
	return (pow(abs((((_pos.x-_size/2<=tpos.x?2:0)|((_pos.x-_size/2>tpos.x||_pos.x+_size/2<tpos.x)?1:0))-2))*(_pos.x + _size*(((_pos.x-_size/2<=tpos.x?2:0)|((_pos.x-_size/2>tpos.x||_pos.x+_size/2<tpos.x)?1:0))-2) - tpos.x), 2)
		+ pow(abs((((_pos.y-_size/2<=tpos.y?2:0)|((_pos.y-_size/2>tpos.y||_pos.y+_size/2<tpos.y)?1:0))-2))*(_pos.y + _size*(((_pos.y-_size/2<=tpos.y?2:0)|((_pos.y-_size/2>tpos.y||_pos.y+_size/2<tpos.y)?1:0))-2) - tpos.y), 2)
		<= pow(t.GetR(),2)) ;
}

bool BarrierObj::IsCollide(Object* t)
{
	
	POINT tpos = t->GetPos();
	return (pow(abs((((_pos.x-_size/2<=tpos.x?2:0)|((_pos.x-_size/2>tpos.x||_pos.x+_size/2<tpos.x)?1:0))-2))*(_pos.x + _size*(((_pos.x-_size/2<=tpos.x?2:0)|((_pos.x-_size/2>tpos.x||_pos.x+_size/2<tpos.x)?1:0))-2) - tpos.x), 2)
		+ pow(abs((((_pos.y-_size/2<=tpos.y?2:0)|((_pos.y-_size/2>tpos.y||_pos.y+_size/2<tpos.y)?1:0))-2))*(_pos.y + _size*(((_pos.y-_size/2<=tpos.y?2:0)|((_pos.y-_size/2>tpos.y||_pos.y+_size/2<tpos.y)?1:0))-2) - tpos.y), 2)
		<= pow(t->GetR(),2));
	
	/*POINT tpos = t->GetPos(), vertex;
	int x = 0, y = 0,check;
	if(_pos.x - _r/2 > tpos.x)x = 1;
	else if(_pos.x + _r/2 < tpos.x)x = 3;
	else x = 2;
	if(_pos.y - _r/2 > tpos.y)y = 1;
	else if(_pos.y + _r/2 < tpos.y)y = 3;
	else y = 2;
	check = (y<<2) | x;
	switch(check)
	{
		case 6:
			return _pos.y - _r/2 <= (t->GetR());
		case 9:
			return _pos.x - _r/2 <= (t->GetR());
		case 10:
			return true;
		case 12:
			return _pos.x + _r/2 <= (t->GetR());
		case 14:
			return _pos.y + _r/2 <= (t->GetR());
		case 5:
			vertex = {_pos.x - _r/2, _pos.y - _r/2};
			break;
		case 7:
			vertex = {_pos.x + _r/2, _pos.y - _r/2};
			break;
		case 13:
			vertex = {_pos.x - _r/2, _pos.y + _r/2};
			break;
		case 15:
			vertex = {_pos.x - _r/2, _pos.y + _r/2};
			break;
	}
	long long int dis = (vertex.x - tpos.x) * (vertex.x - tpos.x) + (vertex.y - tpos.y) * (vertex.y - tpos.y);
	return dis <= (t->GetR());*/
}

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

void Init()
{
	Shop* parent;
	srand(time(NULL));
	
	player = new Player(10,10,R,0,10,10, {100,100});
	nLastTime = clock();

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

bool MoveFrame()
{
	int deltatime = clock() - nLastTime;

	for (auto i : bullets) {
		i->MoveOneFrame(deltatime);
	}
	for (auto i : enemys) {
		i->MoveOneFrame(deltatime);
	}
	player->MoveOneFrame(deltatime);

	for (auto i : bullets) {
		for (auto j : enemys) {
			if (i->IsCollide(j)) {
				int hp = j->GetHp();
				j->SetHp(hp - (i->GetDamage()));
			}
		} 
	}
	for (auto& i : enemys) {
		if (i->IsCollide(player)) {
			int hp = player->GetHp();
			player->SetHp(hp - (i->GetDamage()));
			delete i;
			i = nullptr;
		}
	}

	for (auto P = enemys.begin(); P != enemys.end();) {
		if (*P == nullptr)P = enemys.erase(P);
		else ++P;
	}
	
	if (player->GetHp() <= 0)return false;
	return true;
}

void UpdateShopChoice()
{
	bool check[SHOPLENGTH] = {false,};
	for(int i=0;i<3;i++){
		int g = rand()%SHOPLENGTH;
		while(check[g] || shop[g]->GetMaxcnt() == shop[g]->GetCnt())g = rand()%SHOPLENGTH;
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
