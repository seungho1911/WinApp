#include "Game.h"

std::vector <BulletObj* > bullets;
std::vector<EnemyObj*> enemys;
Player* player;
BarrierObj* barrier;
RECT rtMapSize;

clock_t nLastTime;
POINT ptScreenSize;

Object::Object(int size, double angle, double speed, POS pos)
{
	_size = size;
	_angle = angle;
	_speed = speed;
	_pos = pos;
	_screenpos = { (LONG)pos.x ,(LONG)pos.y };
}
void Object::MoveToAngle(int deltatime)
{
	POS psPos = GetPos();
	psPos.x += (double)(cos(_angle) * _speed)*SECOND(deltatime);
	psPos.y += (double)(sin(_angle) * _speed)*SECOND(deltatime);
	SetPos(psPos);
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
	POS tpos = t.GetPos();
	long long int dis = (_pos.x - tpos.x) * (_pos.x - tpos.x) + (_pos.y - tpos.y) * (_pos.y - tpos.y);
		return dis <= (_size + t.GetR())*(_size + t.GetR());
}
bool Object::IsCollide(Object* t)
{
	POS tpos = t->GetPos();
	long long int dis = (_pos.x - tpos.x) * (_pos.x - tpos.x) + (_pos.y - tpos.y) * (_pos.y - tpos.y);
		return dis <= (_size + t->GetR())*(_size + t->GetR());
}
void Object::RunOneFrame(int deltatime)
{
	throw "임시";
}
void Object::Draw(HDC hdc)
{
	Ellipse(hdc, (_screenpos.x - _size) - rtMapSize.left, (_screenpos.y - _size) - rtMapSize.top, (_screenpos.x + _size) - rtMapSize.left, (_screenpos.y + _size) - rtMapSize.top);
}


BulletObj::BulletObj(int damage, int speed, int size, double angle, POS pos):Object(size,angle,speed,pos)
{
	_damage = damage;
}
void BulletObj::RunOneFrame(int deltatime)
{
	MoveToAngle(deltatime);
}


MobObj::MobObj(int hp, int damage, int size, double angle, double speed, POS pos):Object(size, angle, speed, pos)
{
	_hp = hp;
	_damage = damage;
	_shield = 0;
}


Player::Player(int hp, int damage, int size, double angle, double speed, int cooltime, POS pos)
	:MobObj(hp,damage,size,angle,speed,pos)
{
	_bulletcooltime = cooltime;
	_bulletlasttime = 0;
}
void Player::RunOneFrame(int deltatime)
{
	POS psPlayer = player->GetPos();
	double speed = _speed * SECOND(deltatime);

	//Player Move
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		psPlayer.x -= speed;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		psPlayer.x += speed;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		psPlayer.y -= speed;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		psPlayer.y += speed;
	}

	//wall collsion check
	int MLeft = -1, MRight = -1, MTop = -1, MBottom = -1; //TODO:변수명

	if (psPlayer.x - _size < rtMapSize.left) {
		MLeft = +rtMapSize.left - (psPlayer.x - _size);
		psPlayer.x += MLeft - 1;
	}
	if (psPlayer.x + _size > rtMapSize.right) {
		MRight = (psPlayer.x + _size) - rtMapSize.right;
		psPlayer.x -= MRight - 1;
	}
	if (psPlayer.y - _size < rtMapSize.top) {
		MTop = rtMapSize.top - (psPlayer.y - _size);
		psPlayer.y += MTop - 1;
	}
	if (psPlayer.y + _size > rtMapSize.bottom) {
		MBottom = (psPlayer.y + _size) - rtMapSize.bottom;
		psPlayer.y -= MBottom - 1;
	}

	rtMapSize.left -= MLeft;
	rtMapSize.top -= MTop;
	rtMapSize.right += MRight;
	rtMapSize.bottom += MBottom;
	
	if (rtMapSize.top < 0)rtMapSize.top = 0;
	if (rtMapSize.bottom > ptScreenSize.y)rtMapSize.bottom = ptScreenSize.y;
	if (rtMapSize.left < 0)rtMapSize.left = 0;
	if (rtMapSize.right > ptScreenSize.x)rtMapSize.right = ptScreenSize.x;

	//Shot bullet
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && player->GetLasttime() <= 0) {
		POINT ptMouse;
		POS psPlayer = player->GetPos();
		GetCursorPos(&ptMouse);
		bullets.push_back(new BulletObj(100, 50, R/10 ,atan2((ptMouse.y - psPlayer.y), (ptMouse.x - psPlayer.x)), psPlayer));
		player->SetLasttime(player->GetCooltime());
	}

	player->SetLasttime(player->GetLasttime() - deltatime);
	player->SetPos(psPlayer);
}


Enemy1::Enemy1(int hp, int damage, int size, double angle, double speed, POS pos)
	:EnemyObj(hp, damage, size, angle, speed, pos)
{
	_maxtime = 1;
}
void Enemy1::RunOneFrame(int deltatime)
{
	POS psPlayer = player->GetPos();
	SetAngle(atan2(-(GetPos().y - psPlayer.y), -(GetPos().x - psPlayer.x)));
	MoveToAngle(deltatime);
}

Enemy2::Enemy2(int hp, int damage, int size, double angle, double speed, POS pos)
	:EnemyObj(hp, damage, size, angle, speed, pos)
{
	_maxtime = 1;
}
void Enemy2::RunOneFrame(int deltatime)
{
	MoveToAngle(deltatime);
}

Enemy3::Enemy3(int hp, int damage, int size, double angle, double speed, POS pos)
	:EnemyObj(hp, damage, size, angle, speed, pos)
{
	_maxtime = 1000;
}
void Enemy3::RunOneFrame(int deltatime)
{
	while (deltatime>0)
	{
		deltatime--;
		_lasttime++;
		_lasttime %= _maxtime;
		if(_lasttime == 0) {
			for (int i = 0; i < 360; i += 45) {
				EnemyObj* p = new Enemy2(5, 1, 4, RADIAN(i), 7, GetPos());
				enemys.push_back(p);
			}
		}
		if (_lasttime % 50 == 0) {
			Rotate(RADIAN(4.5));
		}
	}
}
void Enemy3::Draw(HDC hdc)
{
	POINT thispos = GetScreenPos();
	POINT pos[5];
	pos[0] = { -_size, -_size };
	pos[1] = { -_size, _size };
	pos[2] = { _size, _size };
	pos[3] = { _size, -_size };
	pos[4] = { -_size, -_size };
	for (auto &i : pos) {
		POINT p;
		p.x = (i.x * cos(_angle)) - (i.y * sin(_angle));
		p.y = (i.x * sin(_angle)) + (i.y * cos(_angle));
		i = p;
	}
	MoveToEx(hdc, pos[0].x + thispos.x - rtMapSize.left, pos[0].y + thispos.y - rtMapSize.top, nullptr);
	for (int i = 1; i <= 4; i++) {
		LineTo(hdc, pos[i].x + thispos.x - rtMapSize.left, pos[i].y + thispos.y - rtMapSize.top);
	}
	for (auto &i : pos) {
		POINT p;
		p.x = (i.x * cos(RADIAN(45))) - (i.y * sin(RADIAN(45)));
		p.y = (i.x * sin(RADIAN(45))) + (i.y * cos(RADIAN(45)));
		i = p;
	}
	MoveToEx(hdc, pos[0].x + thispos.x - rtMapSize.left, pos[0].y + thispos.y - rtMapSize.top, nullptr);
	for (int i = 1; i <= 4; i++) {
		LineTo(hdc, pos[i].x + thispos.x - rtMapSize.left, pos[i].y + thispos.y - rtMapSize.top);
	}
}


void BarrierObj::MovePos(POS pos)
{
	SetPos(pos);
}
bool BarrierObj::IsCollide(Object* t)
{
	
	POS tpos = t->GetPos();
	POS thispos = GetPos();

	return (pow(abs((((thispos.x-_size/2<=tpos.x?2:0)|((thispos.x-_size/2>tpos.x|| thispos.x+_size/2<tpos.x)?1:0))-2))*(thispos.x + _size*(((thispos.x-_size/2<=tpos.x?2:0)|((thispos.x-_size/2>tpos.x|| thispos.x+_size/2<tpos.x)?1:0))-2) - tpos.x), 2)
		+ pow(abs((((thispos.y-_size/2<=tpos.y?2:0)|((thispos.y-_size/2>tpos.y|| thispos.y+_size/2<tpos.y)?1:0))-2))*(thispos.y + _size*(((thispos.y-_size/2<=tpos.y?2:0)|((thispos.y-_size/2>tpos.y|| thispos.y+_size/2<tpos.y)?1:0))-2) - tpos.y), 2)
		<= pow(t->GetR(),2));
	
	/*POINT tpos = t->GetScreenPos(), vertex;
	int x = 0, y = 0,check;
	if(_screenpos.x - _r/2 > tpos.x)x = 1;
	else if(_screenpos.x + _r/2 < tpos.x)x = 3;
	else x = 2;
	if(_screenpos.y - _r/2 > tpos.y)y = 1;
	else if(_screenpos.y + _r/2 < tpos.y)y = 3;
	else y = 2;
	check = (y<<2) | x;
	switch(check)
	{
		case 6:
			return _screenpos.y - _r/2 <= (t->GetR());
		case 9:
			return _screenpos.x - _r/2 <= (t->GetR());
		case 10:
			return true;
		case 12:
			return _screenpos.x + _r/2 <= (t->GetR());
		case 14:
			return _screenpos.y + _r/2 <= (t->GetR());
		case 5:
			vertex = {_screenpos.x - _r/2, _screenpos.y - _r/2};
			break;
		case 7:
			vertex = {_screenpos.x + _r/2, _screenpos.y - _r/2};
			break;
		case 13:
			vertex = {_screenpos.x - _r/2, _screenpos.y + _r/2};
			break;
		case 15:
			vertex = {_screenpos.x - _r/2, _screenpos.y + _r/2};
			break;
	}
	long long int dis = (vertex.x - tpos.x) * (vertex.x - tpos.x) + (vertex.y - tpos.y) * (vertex.y - tpos.y);
	return dis <= (t->GetR());*/
}


void Init()
{
	srand(time(NULL));

	player = new Player(10,10,R/5,0,100,100, {100,100});
	nLastTime = clock();

	ptScreenSize = { GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN) };
}
bool RunFrame()
{
	int deltatime = clock() - nLastTime;

	player->RunOneFrame(deltatime);
	for (auto i : bullets) {
		i->RunOneFrame(deltatime);
	}
	for (int i = 0; i < enemys.size(); i++) {
		enemys[i]->RunOneFrame(deltatime);
	}/*
	for (auto i : enemys) {
 		i->RunOneFrame(deltatime);
	}*/

	CheckCollision();
	
	nLastTime = clock();
	if (player->GetHp() <= 0)return false;
	return true;
}
void CheckCollision()
{
	for (auto& i : bullets) {
		for (auto& j : enemys) {
			if (i->IsCollide(j)) {
				int hp = j->GetHp();
				j->SetHp(hp - (i->GetDamage()));
				delete i;
				i = nullptr;
				if (j->GetHp() <= 0) {
					delete j;
					j = nullptr;
				}
				break;
			}
		}
		DELETEINVECTOR(enemys)
	}
	DELETEINVECTOR(bullets)
	for (auto& i : enemys) {
		if (i->IsCollide(player)) {
			int hp = player->GetHp();
			player->SetHp(hp - (i->GetDamage()));
			delete i;
			i = nullptr;
		}
	}
	DELETEINVECTOR(enemys)
	for (auto& i : bullets) {
		POINT ptBullet = i->GetScreenPos();
		int size = i->GetSize();
		int damage = i->GetDamage();
		if (ptBullet.x - size < rtMapSize.left) {
			rtMapSize.left -= damage;
			delete i;
			i = nullptr;
			continue;
		}
		if (ptBullet.x + size > rtMapSize.right) {
			rtMapSize.right += damage;
			delete i;
			i = nullptr;
			continue;
		}
		if (ptBullet.y - size < rtMapSize.top) {
			rtMapSize.top -= damage;
			delete i;
			i = nullptr;
			continue;
		}
		if (ptBullet.y + size > rtMapSize.bottom) {
			rtMapSize.bottom += damage;
			delete i;
			i = nullptr;
			continue;
		}
	}
	DELETEINVECTOR(bullets)
}
void SpawnEnemy()
{
	POS psEnemy = player->GetPos();
	psEnemy.x += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);
	psEnemy.y += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);
	enemys.push_back((EnemyObj*)new Enemy3(1, 2, R / 2, 0, 10, psEnemy));
}
void DrawGame(HDC hdc, PAINTSTRUCT ps, RECT rtWindow)
{
	HPEN myPen = CreatePen(PS_SOLID, 7, RGB(0, 255, 0));
	HGDIOBJ oldPen = SelectObject(hdc, myPen);
	

	player->Draw(hdc);

	SelectObject(hdc, oldPen);
	DeleteObject(myPen);

	for (auto i : enemys) {
		i->Draw(hdc);
	}
	for (auto i : bullets) {
		i->Draw(hdc);
	}
}