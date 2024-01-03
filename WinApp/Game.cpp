#include "Game.h"

std::vector <BulletObj* > bullets;
std::vector<MobObj*> enemys;
Player* player;
BarrierObj* barrier;
RECT rtMapSize;

clock_t nLastTime;

Object::Object(int size, double angle, double speed, POINT pos)
{
	_size = size;
	_angle = angle;
	_speed = speed;
	_pos = pos;
}
void Object::MoveAngleToDir()
{
	_pos.x += (double)(cos(_angle) * _speed);
	_pos.y += (double)(sin(_angle) * _speed);
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
void Object::RunOneFrame(int deltatime)
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
void BulletObj::RunOneFrame(int deltatime)
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
void Player::RunOneFrame(int deltatime)
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

	if (ptPlayer.x - _size < rtMapSize.left) {
		MLeft = +rtMapSize.left - (ptPlayer.x - _size);
		ptPlayer.x += MLeft - 1;
	}
	if (ptPlayer.x + _size > rtMapSize.right) {
		MRight = (ptPlayer.x + _size) - rtMapSize.right;
		ptPlayer.x -= MRight - 1;
	}
	if (ptPlayer.y - _size < rtMapSize.top) {
		MTop = rtMapSize.top - (ptPlayer.y - _size);
		ptPlayer.y += MTop - 1;
	}
	if (ptPlayer.y + _size > rtMapSize.bottom) {
		MBottom = (ptPlayer.y + _size) - rtMapSize.bottom;
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
		bullets.push_back(new BulletObj(100, 5, R/10 ,atan2((ptMouse.y - ptPlayer.y), (ptMouse.x - ptPlayer.x)), ptPlayer));
		player->SetLasttime(player->GetCooltime());
	}

	player->SetLasttime(player->GetLasttime() - 1);
	player->SetPos(ptPlayer);
}


Enemy1::Enemy1(int hp, int damage, int size, double angle, double speed, POINT pos)
	:MobObj(hp, damage, size, angle, speed, pos)
{}
void Enemy1::RunOneFrame(int deltatime)
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


void Init()
{
	player = new Player(10,10,R/5,0,10,10, {100,100});
	nLastTime = clock();
}

bool RunFrame()
{
	int deltatime = clock() - nLastTime;

	player->RunOneFrame(deltatime);
	for (auto i : bullets) {
		i->RunOneFrame(deltatime);
	}
	for (auto i : enemys) {
		i->RunOneFrame(deltatime);
	}

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

	for (auto& i : bullets) {
		POINT ptBullet = i->GetPos();
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
	for (auto P = bullets.begin(); P != bullets.end();) {
		if (*P == nullptr)P = bullets.erase(P);
		else ++P;
	}
}