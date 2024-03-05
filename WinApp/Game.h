#pragma once

#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

#define R 50
#define SPEED 10
#define MAXLENGTH 80

#define DELETEINVECTOR(V) for (auto P = (V).begin(); P != (V).end();) {if ((*P)->GetHp()<=0)delete *P,P = (V).erase(P);else ++P;}
#define SECOND(N) (N)/100.0

#define INTCAT(T,N) [](TCHAR* text, int n) -> void {TCHAR tchar[MAXLENGTH];  wsprintf(tchar, _T("%d"), n); _tcscat(text, tchar);}(T,N)
#define FLOATCAT(T,N) [](TCHAR* text, float f) -> void {TCHAR tchar[MAXLENGTH];  _stprintf(tchar,_T("%.2f"),f); _tcscat(text, tchar);}(T,N)


#define PI 3.1415926
#define RADIAN(X) (X) * PI / 180.0
#define INF 2000000000


struct POS { double x, y; };

class Object
{
private:
	POINT _screenpos;
	POS _pos;
protected:
	int _size;
	double _angle;
	double _speed;

public:
	Object(int size, double angle, double speed, POS pos);
	virtual ~Object() {};

	void MoveToAngle(int);
	void Rotate(double);
	void SetAngle(double);
	void LookAt(POS);

	POINT GetScreenPos() { return _screenpos; }
	POS GetPos() { return _pos; }
	void SetPos(POS pos) { _pos = pos; _screenpos = { (LONG)pos.x ,(LONG)pos.y }; }
	int GetSize() { return _size; }
	int GetR() { return _size; }

	virtual bool IsCollide(Object*);
	virtual void RunOneFrame(int) = 0;

	virtual void Draw(HDC, RECT);
};


class MobObj : public Object
{
protected:
	int _hp;
	int _damage;
	int _shield, _notdamagetime;
	int _shieldcooltime, _shieldlasttime;
public:
	MobObj(int hp, int damage, int size, double angle, double speed, POS pos);

	int GetDamage() { return _damage; }
	void SetDamage(int damage) { _damage = damage; }
	int GetHp() { return _hp; }
	void SetHp(int hp) { _hp = hp; }
	int GetShield() { return _shield; }
	void SetShieldCooltime(int cooltime) { _shieldcooltime = cooltime; }
	int GetShieldCooltime() { return _shieldcooltime; }
	void ChargeShield();
	void Attacked(int damage)
	{
		if(_shield > damage){
			_shield -= damage;
			damage = 0;
		}
		else{
			damage -= _shield;
			_shield = 0;
		}
		_hp -= damage;
		if( _hp <= 0 )Kill();
	}
	void Kill() { _hp = -1; }

};


class BulletObj : public MobObj
{
public:
	BulletObj(int hp, int damage, int size, double angle, double speed, POS pos);

	int GetDamage() { return _damage; }

	virtual void RunOneFrame(int) override;

	virtual void Draw(HDC, RECT);
};

class Player : public MobObj
{
	int _bulletcooltime, _bulletlasttime;
	int _shot;
	int _xp;
public:
	Player(int hp, int damage, int size, double angle, double speed, int cooltime, POS pos);

	int GetCooltime() { return _bulletcooltime; }
	void SetCooltime(int cooltime) { _bulletcooltime = cooltime; }
	int GetLasttime() { return _bulletlasttime; }
	void SetLasttime(int lasttime) { _bulletlasttime = lasttime; }
	int GetShot() { return _shot; }
	int AddShot() { return _shot++; }
	int GetXp() { return _xp; }
	void AddXp(int xp) { _xp += xp; }
	void AddDeltaTime(int deltatime);
	
	virtual void RunOneFrame(int) override;

	virtual void Draw(HDC, RECT);
};

class EnemyObj : public MobObj
{
protected:
	int _maxtime, _lasttime;
public:
	EnemyObj(int hp, int damage, int size, double angle, double speed, POS pos) : MobObj(hp, damage, size, angle, speed, pos) { _lasttime = 0; }
};

//basic Enemy
class Enemy1 : public EnemyObj
{
public:
	Enemy1(int hp, int damage, int size, double angle, double speed, POS pos);
	~Enemy1();
	virtual void RunOneFrame(int)  override;
};

//enemy bullet
class Enemy2 : public EnemyObj
{
public:
	Enemy2(int hp, int damage, int size, double angle, double speed, POS pos);
	virtual void RunOneFrame(int)  override;
};

//boss
class Enemy3 : public EnemyObj
{
	HWND _hWnd;
public:
	Enemy3( int hp, int damage, int size, double angle, double speed, POS pos);
	~Enemy3();

	HWND GethWnd() { return _hWnd; }
	virtual void RunOneFrame(int)  override;
	virtual void Draw(HDC, RECT) override;
};

//Laser
class Enemy4 : public EnemyObj
{
public:
	Enemy4(int hp, int damage, int dummysize, double angle, double dummylength, POS pos);
	virtual void RunOneFrame(int)  override;
	virtual void Draw(HDC hdc, RECT rtWindow);
	virtual bool IsCollide(Object*);
};


class BarrierObj : public Object
{
	HWND _hWnd;
public:
	BarrierObj(POS pos);

	HWND GetHwnd() { return _hWnd; }
	virtual void RunOneFrame(int)  override;
	virtual bool IsCollide(Object*) override;
	virtual void Draw(HDC, RECT) override;
};

class BarrierStats
{
	int _size;
	int _damage;
	int _attackcooltime, _attacklasttime;
	int _cnt; 
public:
	BarrierStats(int damage, int attackcooltime, int size);

	int GetCnt() { return _cnt; }
	int GetSize() { return _size; }
	int GetDamage() { return _damage; }
	void SetDamage(int damage) { _damage = damage; }
	void RunOneFrame(int);
};


class ExpObj : public MobObj
{
public:
	ExpObj(int xp, POS pos);
	virtual void RunOneFrame(int) override;
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
void DrawGame(HDC,  RECT);