#pragma once

#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <string>
#include <ctime>
#include <stdlib.h>

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

#define R 50
#define SPEED 10
#define MAXLENGTH 80
#define SHOPLENGTH 4

#define DELETEINVECTOR(V) for (auto P = (V).begin(); P != (V).end();) {if (*P == nullptr)P = (V).erase(P);else ++P;}

#define INTTOCHAR(N) [](int n) -> TCHAR* {TCHAR tchar[MAXLENGTH];  wsprintf(tchar, _T("%d"), n); return tchar;}(N)
#define FLOATTOCHAR(N) [](float f) -> TCHAR* {TCHAR tchar[MAXLENGTH];  wsprintf(tchar, _T("%f"), f); return tchar;}(N)

/*
#define PI 3.1415926
#define RADIAN(X) (X) * PI / 180
*/

class Object
{
protected:
	POINT _pos;
	int _size;
	double _angle;
	double _speed;

public:
	Object(int size, double angle, double speed, POINT pos);
	void MoveAngleToDir();

	void Rotate(double);
	void SetAngle(double);

	POINT GetPos() { return _pos; }
	void SetPos(POINT pos) { _pos = pos; }
	int GetSize() { return _size; }
	int GetR() { return _size; }

	virtual bool IsCollide(Object);
	virtual bool IsCollide(Object*);
	virtual void RunOneFrame(int);

	void Draw(HDC hdc);
};

class BulletObj : public Object
{
	int _damage;
public:
	BulletObj(int damage, int speed, int size, double angle, POINT pos);

	int GetDamage() { return _damage; }

	virtual void RunOneFrame(int) override;
};

class MobObj : public Object
{
protected:
	int _hp;
	int _damage;
public:
	MobObj(int hp, int damage, int size, double angle, double speed, POINT pos);

	int GetDamage() { return _damage; }
	void SetDamage(int damage) { _damage = damage; }
	int GetHp() { return _hp; }
	void SetHp(int hp) { _hp = hp; }
};

class Player : public MobObj
{
	int _bulletcooltime, _bulletlasttime;//총알 발사 쿨
	//TODO:게임 관련 변수들 추가
public:
	Player(int hp, int damage, int size, double angle, double speed, int cooltime, POINT pos);

	int GetCooltime() { return _bulletcooltime; }
	int GetLasttime() { return _bulletlasttime; }
	void SetCooltime(double cooltime) { _bulletcooltime = cooltime; }
	void SetLasttime(int lasttime) { _bulletlasttime = lasttime; }
	
	virtual void RunOneFrame(int) override;
};

class Enemy1 : public MobObj
{
public:
	Enemy1(int hp, int damage, int size, double angle, double speed, POINT pos);
	virtual void RunOneFrame(int)  override;
};

class BarrierObj : public Object
{
	int _damage;
public:
	void MovePos(POINT);
	bool IsCollide(Object) override;
	bool IsCollide(Object*) override;
};

/*
==list==
addshot
shotdamage
reload
reduceplayersize
barrier
-damage
-size
gravity wave
charge defense
enemy move down
*/

void Init();
bool RunFrame();
void CheckCollision();
void SpawnEnemy();