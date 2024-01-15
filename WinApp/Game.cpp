#include "Game.h"
#include "WinProc.h"

std::vector <BulletObj* > bullets;
std::vector <EnemyObj*> enemys;
std::vector <ExpObj*> exps;
Player* player;
BarrierStats* barrier = new BarrierStats(1, 100, 100);
std::vector <BarrierObj*> barriers;
RECT rtMainScreen;

clock_t nLastTime;
POINT ptScreenSize;


extern HINSTANCE hInst;
extern std::vector<HWND> hWnds;

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
bool Object::IsCollide(Object* t)
{
	POS tpos = t->GetPos();
	long long int dis = (_pos.x - tpos.x) * (_pos.x - tpos.x) + (_pos.y - tpos.y) * (_pos.y - tpos.y);
		return dis <= (_size + t->GetR())*(_size + t->GetR());
}
void Object::Draw(HDC hdc, RECT rtMapSize)
{
	if ((_screenpos.x + _size) < rtMapSize.left)return;
	if ((_screenpos.y + _size) < rtMapSize.top)return;
	if ((_screenpos.x - _size) > rtMapSize.right)return;
	if ((_screenpos.y - _size) > rtMapSize.bottom)return;
	Ellipse(hdc, (_screenpos.x - _size) - rtMapSize.left, (_screenpos.y - _size) - rtMapSize.top, (_screenpos.x + _size) - rtMapSize.left, (_screenpos.y + _size) - rtMapSize.top);
}


ExpObj::ExpObj(int xp, POS pos) :Object(xp, 0, 0, pos){}
void ExpObj::RunOneFrame(int deltatime){}


MobObj::MobObj(int hp, int damage, int size, double angle, double speed, POS pos):Object(size, angle, speed, pos)
{
	_hp = hp;
	_damage = damage;
	_shield = 0;
}


BulletObj::BulletObj(int hp, int damage, int size, double angle, double speed, POS pos):MobObj(hp,damage, size, angle, speed, pos)
{
	_damage = damage;
}
void BulletObj::RunOneFrame(int deltatime)
{
	MoveToAngle(deltatime);
}
 

Player::Player(int hp, int damage, int size, double angle, double speed, int cooltime, POS pos)
	:MobObj(hp,damage,size,angle,speed,pos)
{
	_bulletcooltime = cooltime;
	_bulletlasttime = 0;
	_shieldcooltime = INF; 
	_shieldlasttime = 0;
}
void Player::AddDeltaTime(int deltatime) 
{
	_bulletlasttime += deltatime; 
	if (_shieldcooltime != INF)_shieldlasttime += deltatime; 
}
void Player::ChargeShield() 
{ 
	_shield += _shieldlasttime / _shieldcooltime;
	_shieldlasttime %= _shieldcooltime; 
}
void Player::RunOneFrame(int deltatime)
{
	POS psPlayer = player->GetPos();
	double speed = _speed * SECOND(deltatime);
	player->AddDeltaTime(deltatime);

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

	if (psPlayer.x - _size < rtMainScreen.left) {
		MLeft = +rtMainScreen.left - (psPlayer.x - _size);
		psPlayer.x += MLeft - 1;
	}
	if (psPlayer.x + _size > rtMainScreen.right) {
		MRight = (psPlayer.x + _size) - rtMainScreen.right;
		psPlayer.x -= MRight - 1;
	}
	if (psPlayer.y - _size < rtMainScreen.top) {
		MTop = rtMainScreen.top - (psPlayer.y - _size);
		psPlayer.y += MTop - 1;
	}
	if (psPlayer.y + _size > rtMainScreen.bottom) {
		MBottom = (psPlayer.y + _size) - rtMainScreen.bottom;
		psPlayer.y -= MBottom - 1;
	}

	rtMainScreen.left -= MLeft;
	rtMainScreen.top -= MTop;
	rtMainScreen.right += MRight;
	rtMainScreen.bottom += MBottom;

	//TEST
	rtMainScreen.left = 0;
	rtMainScreen.top = 0;
	rtMainScreen.bottom = ptScreenSize.y;
	rtMainScreen.right = ptScreenSize.x;
	
	if (rtMainScreen.top < 0)rtMainScreen.top = 0;
	if (rtMainScreen.bottom > ptScreenSize.y)rtMainScreen.bottom = ptScreenSize.y;
	if (rtMainScreen.left < 0)rtMainScreen.left = 0;
	if (rtMainScreen.right > ptScreenSize.x)rtMainScreen.right = ptScreenSize.x;

	//Shot bullet
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && player->GetLasttime() >= player->GetCooltime()) {
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		bullets.push_back(new BulletObj(10, 50, R / 10, atan2((ptMouse.y - psPlayer.y), (ptMouse.x - psPlayer.x)), 100, psPlayer));
		player->SetLasttime(0);
	}
	player->ChargeShield();
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
	_maxtime = 2000;


	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc_BOSS;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WINAPP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_WINAPP);
	wcex.lpszClassName = _T("BOSS");
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);


	POINT ptscreen = GetScreenPos();
	_hWnd = CreateWindowW(_T("BOSS"), _T("BOSS"), WS_POPUP | WS_VISIBLE,
		ptscreen.x-100, ptscreen.y-100, 200, 200, nullptr, nullptr, hInst, nullptr);
	if (!_hWnd)
	{
		throw("BOSS ERROR");
	}

	hWnds.push_back(_hWnd);
	ShowWindow(_hWnd, 10);
	UpdateWindow(_hWnd);
}
Enemy3::~Enemy3()
{
	exps.push_back(new ExpObj(10, GetPos()));

	for (auto i = hWnds.begin(); i != hWnds.end();) {
		if (*i == _hWnd)i = hWnds.erase(i);
		else ++i;
	}
	SendMessage(_hWnd, WM_CLOSE, 0, 0);
}
void Enemy3::RunOneFrame(int deltatime)
{
	while (deltatime>0)
	{
		deltatime--;
		_lasttime++;
		_lasttime %= _maxtime;
		if(_lasttime == 0) {
			//for (int i = 0; i < 360; i += 45) {
				int i = 0;
				EnemyObj* p = new Enemy4(5, 1, 5, RADIAN(i), 1000, GetPos());
				enemys.push_back(p);
			//}
		}
		if (_lasttime % 50 == 0) {
			Rotate(RADIAN(4.5));
		}
	}
}
void Enemy3::Draw(HDC hdc, RECT rtWindow)
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
	MoveToEx(hdc, pos[0].x + thispos.x - rtWindow.left, pos[0].y + thispos.y - rtWindow.top, nullptr);
	for (int i = 1; i <= 4; i++) {
		LineTo(hdc, pos[i].x + thispos.x - rtWindow.left, pos[i].y + thispos.y - rtWindow.top);
	}
	for (auto &i : pos) {
		POINT p;
		p.x = (i.x * cos(RADIAN(45))) - (i.y * sin(RADIAN(45)));
		p.y = (i.x * sin(RADIAN(45))) + (i.y * cos(RADIAN(45)));
		i = p;
	}
	MoveToEx(hdc, pos[0].x + thispos.x - rtWindow.left, pos[0].y + thispos.y - rtWindow.top, nullptr);
	for (int i = 1; i <= 4; i++) {
		LineTo(hdc, pos[i].x + thispos.x - rtWindow.left, pos[i].y + thispos.y - rtWindow.top);
	}
}

Enemy4::Enemy4(int hp, int damage, int dummysize, double angle, double dummylength, POS pos)
	:EnemyObj(hp, damage, 1, angle, 10000, pos)
{
	_maxtime = 1500;
}
void Enemy4::RunOneFrame(int deltatime)
{
	POS psPlayer = player->GetPos();
	while (deltatime > 0)
	{
		if (_lasttime == 0) {
			_size = 1;
		}
		if (_lasttime < 800) {
			SetAngle(atan2(-(GetPos().y - psPlayer.y), -(GetPos().x - psPlayer.x)));
		}
		if (_lasttime % 10 == 0 && _lasttime > 1400) {
			_size += 2;
		}
		if (_lasttime > _maxtime) {
			Kill();
			return;
		}
		deltatime--;
		_lasttime++;
	}
}
void Enemy4::Draw(HDC hdc, RECT rtWindow)
{
	POINT ptPos = GetScreenPos();
	HPEN hPen = CreatePen(PS_SOLID, _size, RGB(255, 0, 0));
	HGDIOBJ hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, ptPos.x - rtWindow.left, ptPos.y - rtWindow.top, nullptr);
	LineTo(hdc, ptPos.x+cos(_angle)*_speed - rtWindow.left, ptPos.y+sin(_angle)*_speed - rtWindow.top);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}
bool Enemy4::IsCollide(Object* t)
{
	if (_lasttime < 1400)return false;
	POS ptPosT = t->GetPos();
	POS ptPos = GetPos();
	double d = pow(tan(_angle) * (ptPosT.x - ptPos.x) + ptPos.y - ptPosT.y, 2) / (pow(tan(_angle), 2) + 1);
	return d < pow(_size+t->GetSize(),2);
}

BarrierObj::BarrierObj(POS pos)
	:Object(0, 0, 0, pos)
{
	_hWnd = InitInstance_Barrier(hInst, pos, barrier->GetSize(), 10);
	hWnds.push_back(_hWnd);
}
void BarrierObj::Draw(HDC hdc, RECT rtWindow)
{
	POINT screenpos = this->GetScreenPos();

	Rectangle(hdc, (screenpos.x - barrier->GetSize()/2) - rtWindow.left, (screenpos.y - barrier->GetSize() / 2) - rtWindow.top, (screenpos.x + barrier->GetSize()/2) - rtWindow.left, (screenpos.y + barrier->GetSize()/2) - rtWindow.top);
}
bool BarrierObj::IsCollide(Object* t)
{

	POS tpos = t->GetPos();
	POS thispos = this->GetPos();
	int size = this->GetSize();

	return (pow(abs((((thispos.x - size / 2 <= tpos.x ? 2 : 0) | ((thispos.x - size / 2 > tpos.x || thispos.x + size / 2 < tpos.x) ? 1 : 0)) - 2)) * (thispos.x + size * (((thispos.x - size / 2 <= tpos.x ? 2 : 0) | ((thispos.x - size / 2 > tpos.x || thispos.x + size / 2 < tpos.x) ? 1 : 0)) - 2) - tpos.x), 2)
		+ pow(abs((((thispos.y - size / 2 <= tpos.y ? 2 : 0) | ((thispos.y - size / 2 > tpos.y || thispos.y + size / 2 < tpos.y) ? 1 : 0)) - 2)) * (thispos.y + size * (((thispos.y - size / 2 <= tpos.y ? 2 : 0) | ((thispos.y - size / 2 > tpos.y || thispos.y + size / 2 < tpos.y) ? 1 : 0)) - 2) - tpos.y), 2)
		<= pow(t->GetSize(), 2));
	/*
	POINT tpos = t->GetScreenPos(), vertex;
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
void BarrierObj::RunOneFrame(int deltatime)
{
	RECT rtMain;
	GetClientRect(_hWnd, &rtMain);
	
	ClientToScreen(_hWnd, reinterpret_cast<POINT*>(&rtMain.left)); // convert top-left
	ClientToScreen(_hWnd, reinterpret_cast<POINT*>(&rtMain.right)); // convert bottom-right
	/*
	GetWindowRect(_hWnd, &rtMain);
	RECT rtn = { 0, 0, barrier->GetSize() + 100, barrier->GetSize() + 100 };
	AdjustWindowRect(&rtn, WS_OVERLAPPEDWINDOW, FALSE);
	POS debug = { (double)rtMain.left - rtn.left + barrier->GetSize() / 2 + 50, (double)rtMain.top - rtn.top + barrier->GetSize() / 2 + 50 };*/
	POS debug = { (double)rtMain.left + barrier->GetSize()/2 + 50, (double)rtMain.top + barrier->GetSize() / 2 + 50 };
	this->SetPos({ (double)rtMain.left + barrier->GetSize()/2 + 50, (double)rtMain.top + barrier->GetSize() / 2 + 50 });
}

BarrierStats::BarrierStats(int damage, int attackcooltime, int size)
{
	_size = size;
	_damage = damage;
	_attackcooltime = attackcooltime;
	_attacklasttime = 0;
	_cnt = 0;
}
void BarrierStats::RunOneFrame(int deltatime)
{
	_attacklasttime += deltatime;
	_cnt += _attacklasttime / _attackcooltime;
	_attacklasttime %= _attackcooltime;
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
	barrier->RunOneFrame(deltatime);
	for (auto i : barriers) {
		i->RunOneFrame(deltatime);
	}

	DELETEINVECTOR(enemys)
	CheckCollision();
	
	nLastTime = clock();
	if (player->GetHp() <= 0)return false;
	return true;
}
void CheckCollision()
{
	//enemy vs player
	for (auto& i : enemys) {
		if (i->IsCollide(player)) {
			player->SetHp(player->GetHp() - i->GetDamage());
		}
	}

	//bullet vs enemy
	for (auto& i : bullets) {
		for (auto& j : enemys) {
			if (j->IsCollide(i)) {
				/*int hp = j->GetHp();
				j->SetHp(hp - (i->GetDamage()));
				i->Kill();*/
				j->Attacked(i->GetDamage());
				break;
			}
		}
		DELETEINVECTOR(enemys)
	}
	DELETEINVECTOR(bullets)

	for (auto& i : barriers) {
		for (auto& j : enemys) {
			if (i->IsCollide(j)) {
				/*int hp = j->GetHp();
				j->SetHp(hp - barrier->GetDamage() * barrier->GetCnt());*/
				j->Attacked(barrier->GetDamage() * barrier->GetCnt());
			}
		}
		DELETEINVECTOR(enemys)
	}

	//Bullet vs Screen
	for (auto& i : bullets) {
		POINT ptBullet = i->GetScreenPos();
		int size = i->GetSize();
		int damage = i->GetDamage();
		if (ptBullet.x - size < rtMainScreen.left) {
			rtMainScreen.left -= damage;
			i->Kill();
			continue;
		}
		if (ptBullet.x + size > rtMainScreen.right) {
			rtMainScreen.right += damage;
			i->Kill();
			continue;
		}
		if (ptBullet.y - size < rtMainScreen.top) {
			rtMainScreen.top -= damage;
			i->Kill();
			continue;
		}
		if (ptBullet.y + size > rtMainScreen.bottom) {
			rtMainScreen.bottom += damage;
			i->Kill();
			continue;
		}
	}
	DELETEINVECTOR(bullets)

	//Enemy vs Screen
	for (auto& i : enemys) {
		POINT ptEnemy = i->GetScreenPos();
		int size = i->GetSize();
		if (ptEnemy.x + size < 0) {
			i->Kill();
			continue;
		}
		if (ptEnemy.x - size > ptScreenSize.x) {
			i->Kill();
			continue;
		}
		if (ptEnemy.y + size < 0) {
			i->Kill();
			continue;
		}
		if (ptEnemy.y - size > ptScreenSize.y) {
			i->Kill();
			continue;
		}
	}
	DELETEINVECTOR(enemys)
}
void SpawnEnemy()
{
	POS psEnemy = player->GetPos();
	psEnemy.x += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);
	psEnemy.y += (200 + rand() % 100) * (rand() % 2 == 1 ? 1 : -1);
	//enemys.push_back((EnemyObj*)new Enemy3(1, 2, R / 2, 0, 10, psEnemy));
}
void DrawGame(HDC hdc, RECT rtWindow)
{
	HPEN myPen = CreatePen(PS_SOLID, 7, RGB(0, 255, 0));
	HGDIOBJ oldPen = SelectObject(hdc, myPen);
	

	player->Draw(hdc, rtWindow);

	SelectObject(hdc, oldPen);
	DeleteObject(myPen);

	for (auto i : enemys) {
		i->Draw(hdc, rtWindow);
	}
	for (auto i : bullets) {
		i->Draw(hdc, rtWindow);
	}

	myPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 255));
	SelectObject(hdc, myPen);
	for (auto i : exps) {
		i->Draw(hdc, rtWindow);
	}
	SelectObject(hdc, oldPen);
	DeleteObject(myPen);

	///brush and pen need
	for (auto i : barriers) {
		i->Draw(hdc, rtWindow);
	}
}


