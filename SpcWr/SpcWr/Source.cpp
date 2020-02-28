#include <windows.h> 
#include <cmath>
#include <ctime>
#include <vector>
using namespace std;

HINSTANCE hInst; 	
LPCTSTR szWindowClass = "QWERTY";
LPCTSTR szTitle = "SpaceWar";

double dt = 1;
double G = 0.5;
static bool MFlag;
bool NearSun = 0;
bool NearPlayer = 0;


ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void EndGame() {
	PostQuitMessage(0);
};

class Star {
public:
	double x;
	double y;
	double r = 15;
	double m = 1500;
};

class Bullet {
public:
	double x;
	double y;
	double vx = 0;
	double vy = 0;
	double r = 4;
	double m = 1;
	double LifeTime = 150;
	double Time = 0;

};

class EngineParticle {
public:
	double x;
	double y;
	double vx = 0;
	double vy = 0;
	double r = 3;
	double m = 1;
	double LifeTime = 15;
	double Time = 0;
};

vector <Bullet> Blts;
vector <Bullet> Blts2;

vector<EngineParticle> Prts1;
vector<EngineParticle> Prts2;

class Ship {
public:
	double x = 10;
	double y = 30;
	double vx = 0;
	double vy = 0;
	double r = 5;
	double m = 10;
	POINT angles[5];
	int direction = 315;
	int t = direction;

	void Shoot(vector <Bullet> &bullets) {
		Bullet tBlt;
		tBlt.x = angles[0].x;
		tBlt.y = angles[0].y;
		tBlt.vx =  cos(direction * 3.14 / 180) * 6;
		tBlt.vy = -sin(direction * 3.14 / 180) * 6;
		bullets.push_back(tBlt);
	}

	void EngineParticles(vector <EngineParticle> &particles) {

		EngineParticle tPrt;
		tPrt.x = (angles[2].x + angles[3].x) / 2 + (rand()%11-5);
		tPrt.y = (angles[2].y + angles[3].y) / 2 + (rand()%11-5);
		tPrt.vx = cos(direction * 3.14 / 180 + 180) * 2;
		tPrt.vy = -sin(direction * 3.14 / 180 + 180) * 2;

		particles.push_back(tPrt);
	}

	void GetAngles() {
		direction += 360;
		direction = direction % 360;
		angles[0].x = x + 28 * cos(direction * 3.14 / 180);
		angles[0].y = y - 28 * sin(direction * 3.14 / 180);
		t = direction + 100;
		t %= 360;
		angles[2].x = x + 10 * cos(t * 3.14 / 180);
		angles[2].y = y - 10 * sin(t * 3.14 / 180);
		t = direction - 100;
		t %= 360;
		angles[3].x = x + 10 * cos(t * 3.14 / 180);
		angles[3].y = y - 10 * sin(t * 3.14 / 180);

		angles[1].x = (angles[0].x + angles[2].x + x) / 3;
		angles[1].y = (angles[0].y + angles[2].y + y) / 3;

		angles[4].x = (angles[0].x + angles[3].x + x) / 3;
		angles[4].y = (angles[0].y + angles[3].y + y) / 3;
	}
	
	bool Sun(Star sun) {
		if (sun.x - x < 15 && sun.x - x > -15 && sun.y - y < 15 && sun.y - y > -15)
			return 1;
		else
			return 0;
	}

	bool Killed() {
	}
	
};

class Player : public Ship {
public:
	bool Killed() {
		for (int i = 0; i < Blts2.size(); i++) {
			if ((pow((Blts2[i].x - x), 2) + pow((Blts2[i].y - y), 2)) < 200)
			{
				Blts2.erase(Blts2.cbegin() + i);
				return 1;
			}
		}
		return 0;
	}
};

class Enemy : public Ship{
public:
	int PlayerDirection = 0;
	int SunDirection = 0;
	double delta_x_s, delta_y_s;
	double delta_x_p, delta_y_p;

	void DetectPlayer(Player &MyShip) {
		
		delta_x_p = abs(MyShip.x - x);
		delta_y_p = abs(MyShip.y - y);
		
			if (MyShip.x >= x && MyShip.y <= y) {
				PlayerDirection = (double)90 - double(double(atan(double(delta_x_p / delta_y_p))) * 57.29);
			}
			else if (MyShip.x <= x && MyShip.y <= y) {
				PlayerDirection = (double)90 + double(double(atan(double(delta_x_p / delta_y_p))) * 57.29);
			}
			else if (MyShip.x <= x && MyShip.y >= y) {
				PlayerDirection = (double)270 - double(double(atan(double(delta_x_p / delta_y_p))) * 57.29);
			}
			else if (MyShip.x >= x && MyShip.y >= y) {
				PlayerDirection = (double)270 + double(double(atan(double(delta_x_p / delta_y_p))) * 57.29);
			}
	}

	void ChangeDirection() {

		if (((360 - PlayerDirection) + direction) % 360 < 180)
			direction -= 2;
		else direction += 2;

	}
	
	void ShootPlayer() {
		if (abs(direction - PlayerDirection) < 10)
		if (Blts2.empty())
			Shoot(Blts2);
		else if (Blts2.back().Time > 60)
			Shoot(Blts2);
	}

	void MoveToPlayer(){

		if (sqrt(vx * vx + vy * vy) > 4) {
			vx *= 0.9;
			vy *= 0.9;
		}
			vx += cos(PlayerDirection * 3.14 / 180) * 0.2;
			vy -= sin(PlayerDirection * 3.14 / 180) * 0.2;

			EngineParticles(Prts2);

	}

	bool Killed() {
		for (int i = 0; i < Blts.size(); i++) {
			if ((pow(Blts[i].x - x,2) + pow(Blts[i].y - y, 2)) < 100){
				Blts.erase(Blts.cbegin() + i);
				return 1;
			}
		}
		return 0;
	}

	void DetectSun(Star &MyStar) {
		delta_x_s = abs(MyStar.x - x);
		delta_y_s = abs(MyStar.y - y);

		if (MyStar.x >= x && MyStar.y <= y) {
			SunDirection = (double)90 - double(double(atan(double(delta_x_s / delta_y_s))) * 57.29);
		}
		else if (MyStar.x <= x && MyStar.y <= y) {
			SunDirection = (double)90 + double(double(atan(double(delta_x_s / delta_y_s))) * 57.29);
		}
		else if (MyStar.x <= x && MyStar.y >= y) {
			SunDirection = (double)270 - double(double(atan(double(delta_x_s / delta_y_s))) * 57.29);
		}
		else if (MyStar.x >= x && MyStar.y >= y) {
			SunDirection = (double)270 + double(double(atan(double(delta_x_s / delta_y_s))) * 57.29);
		}
	}

	void RunFromSun() {
		double DistanceToSun = sqrt(delta_x_s * delta_x_s + delta_y_s * delta_y_s);
		if (DistanceToSun < 150) {
			NearSun = 1;
		}
		else NearSun = 0;

		if (NearSun) {
			if (SunDirection - direction == 0)
				direction += 1;

			direction -= (89 / (SunDirection - direction));


			vx += 0.5 * cos(direction * 3.14 / 180);
			vy -= 0.5 * sin(direction * 3.14 / 180);
			if (sqrt(vx * vx + vy * vy) > 5) {
				vx *= 0.9;
				vy *= 0.9;
			}
			EngineParticles(Prts2);
		}
	}

	void RunFromPlayer() {
		double DistanceToPlayer = sqrt(delta_x_p * delta_x_p + delta_y_p * delta_y_p);
		if (DistanceToPlayer < 100) {
			NearPlayer = 1;
		}
		else NearPlayer = 0;

		if (DistanceToPlayer < 200) {
			/*direction -= (89 / (PlayerDirection - direction));
			vx += 0.5 * cos(direction * 3.14 / 180);
			vy -= 0.5 * sin(direction * 3.14 / 180);*/
			direction += (PlayerDirection - direction) / 20;
			vx -= 0.05 * cos(direction * 3.14 / 180);
			vy += 0.05 * sin(direction * 3.14 / 180);
			if (sqrt(vx * vx + vy * vy) > 5) {
				vx *= 0.9;
				vy *= 0.9;
			}
		}
	}

};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	 
	MyRegisterClass(hInstance);

	
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void Step(Star star, Ship& ship) {
	double a, ax, ay, dx, dy, r;
	dx = star.x - ship.x;
	dy = star.y - ship.y;

	r = dx * dx + dy * dy;
	a = G * star.m / r;

	r = sqrt(r);
	ax = a * dx / r; 
	ay = a * dy / r; 

	ship.vx += ax * dt;
	ship.vy += ay * dt;
	
	ship.x += ship.vx * dt;
	ship.y += ship.vy * dt;
}

void StepBlt(Star star, Bullet& blt) {
	double a, ax, ay, dx, dy, r;
	dx = star.x - blt.x;
	dy = star.y - blt.y;

	r = dx * dx + dy * dy;
	a = G * star.m / r;

	r = sqrt(r); 
	ax = a * dx / r;
	ay = a * dy / r; 

	blt.vx += ax * dt;
	blt.vy += ay * dt;
	
	blt.x += blt.vx * dt;
	blt.y += blt.vy * dt;
	blt.Time += 1;
	
}

void StepPrt(Star star, EngineParticle& prt) {
	double a, ax, ay, dx, dy, r;
	dx = star.x - prt.x;
	dy = star.y - prt.y;

	r = dx * dx + dy * dy;
	a = G * star.m / r;

	r = sqrt(r);
	ax = a * dx / r;
	ay = a * dy / r;

	prt.vx += ax * dt;
	prt.vy += ay * dt;

	prt.x += prt.vx * dt;
	prt.y += prt.vy * dt;
	prt.Time += 1;

}

void DrawBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart)
{
	BITMAP bm;
	HDC hdcMem;
	DWORD dwSize;
	POINT ptSize, ptOrg;
	hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, hBitmap);
	SetMapMode(hdcMem, GetMapMode(hdc));
	GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(hdc, &ptSize, 1);
	ptOrg.x = 0;
	ptOrg.y = 0;
	DPtoLP(hdcMem, &ptOrg, 1);
	BitBlt(
		hdc, xStart, yStart, ptSize.x, ptSize.y,
		hdcMem, ptOrg.x, ptOrg.y, SRCCOPY
	);
	DeleteDC(hdcMem);
}

void Reset(RECT &rt, Star &MyStar, Enemy &EnemyShip, Player &MyShip) {
	MyStar.x = rt.right/2;
	MyStar.y = rt.bottom/2;
	MyStar.m = 1500;
	MyStar.r = 15;

	MyShip.x = 10;
	MyShip.y = 30;
	MyShip.vx = 0;
	MyShip.vy = 0;
	MyShip.direction = 315;

	EnemyShip.x = rt.right - 20;
	EnemyShip.y = rt.bottom - 20;
	EnemyShip.direction = 135;
	EnemyShip.vx = 0;
	EnemyShip.vy = 0;
	

	Blts.clear();
	Blts2.clear();
	Prts1.clear();
	Prts2.clear();

}

ATOM MyRegisterClass(HINSTANCE hInstance)
{

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			
	wcex.hIcon = LoadIcon(NULL, IDI_HAND); 		
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	
	//wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 2); 
	wcex.hbrBackground = CreatePatternBrush(LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(102)));
	wcex.lpszMenuName = NULL; 				
	wcex.lpszClassName = szWindowClass; 	
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance;
	hWnd = CreateWindow(szWindowClass, 	
		szTitle, 				
		WS_OVERLAPPEDWINDOW,			
		50, 			
		20,			
		1200, 			
		699, 			
		NULL, 					
		NULL, 					
		hInstance, 				
		NULL); 				

	if (!hWnd) 	
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		
	UpdateWindow(hWnd); 				
	return TRUE;
}

void DrawBlt(RECT rt, Star MyStar, HDC &hdc, vector <Bullet> &bullets) {
	for (int i = 0; i < bullets.size(); i++) {
		//if (abs((int)bullets[i].Time % 200)) return;
		if (bullets[i].x > rt.right || bullets[i].x < 0 || bullets[i].y>rt.bottom || bullets[i].y < 0) {
			bullets.erase(bullets.cbegin() + i);
			continue;
		}
		if (bullets[i].Time > bullets[i].LifeTime) {
			bullets.erase(bullets.cbegin() + i);
			continue;
		}

		StepBlt(MyStar, bullets[i]);
		if ((bullets[i].x - MyStar.x) < 8 && (bullets[i].x - MyStar.x) > -8 && (bullets[i].y - MyStar.y) < 8 && (bullets[i].y - MyStar.y) > -8) {
			bullets.erase(bullets.cbegin() + i);
			continue;
		}
		if (bullets[i].x > rt.right) {
			bullets[i].x = 0;
		}
		if (bullets[i].x < 0) {
			bullets[i].x = rt.right;
		}
		if (bullets[i].y > rt.bottom) {
			bullets[i].y = 0;
		}
		if (bullets[i].y < 0) {
			bullets[i].y = rt.bottom;
		}

		HBRUSH hbr;

		hbr = CreateSolidBrush(RGB(70, 220, 180));
		SelectObject(hdc, hbr);
		Ellipse(hdc, bullets[i].x - bullets[i].r, bullets[i].y - bullets[i].r, bullets[i].x + bullets[i].r, bullets[i].y + bullets[i].r);
		DeleteObject(hbr);
	}
}

void DrawPrt(RECT rt, Star MyStar, HDC& hdc, vector <EngineParticle>& particles) {
	for (int i = 0; i < particles.size(); i++) {
		
		if (particles[i].x > rt.right || particles[i].x < 0 || particles[i].y>rt.bottom || particles[i].y < 0) {
			particles.erase(particles.cbegin() + i);
			continue;
		}
		if (particles[i].Time > particles[i].LifeTime) {
			particles.erase(particles.cbegin() + i);
			continue;
		}

		StepPrt(MyStar, particles[i]);

		if ((particles[i].x - MyStar.x) < 8 && (particles[i].x - MyStar.x) > -8 && (particles[i].y - MyStar.y) < 8 && (particles[i].y - MyStar.y) > -8) {
			particles.erase(particles.cbegin() + i);
			continue;
		}

		HBRUSH hbr;

		
		hbr = CreateSolidBrush(RGB(rand()%100+150, 200, 200));
		SelectObject(hdc, hbr);
		Ellipse(hdc, particles[i].x - particles[i].r, particles[i].y - particles[i].r, particles[i].x + particles[i].r, particles[i].y + particles[i].r);
		DeleteObject(hbr);
	}
}

void Stars(HDC& hdc, POINT arr[]) {
	for (int i = 0; i < 150; i++) {
		int tx = arr[i].x;
		int ty = arr[i].y;
		HBRUSH hbr = CreateSolidBrush(RGB(250, 250, 250));
		SelectObject(hdc, hbr);
		Ellipse(hdc, tx - 2, ty - 2, tx + 2, ty + 2);
		DeleteObject(hbr);
	}
}

void DrawSun(HDC &hdc, RECT rt) {
	HBITMAP hBit = LoadBitmap(GetModuleHandle(0),MAKEINTRESOURCE(102));
	DrawBitmap(hdc, hBit, rt.right/2-20, rt.bottom/2-20 );
	DeleteObject(hBit);
}

void CheckButtons(bool UP, bool DOWN, bool LEFT, bool RIGHT, bool SPACE, Player &MyShip, int randNum1000) {
	if (sqrt(MyShip.vx * MyShip.vx + MyShip.vy * MyShip.vy) > 4) {
		MyShip.vx *= 0.9;
		MyShip.vy *= 0.9;
	}
	if(UP){
		MyShip.vx += 0.05 * cos(MyShip.direction * 3.14 / 180);
		MyShip.vy -= 0.05 * sin(MyShip.direction * 3.14 / 180);
		if(randNum1000 > 600)
		MyShip.EngineParticles(Prts1);
	}
	if(RIGHT) {
		MyShip.direction -= 2;
	}
	if(LEFT) {
		MyShip.direction += 2;
	}
	if(DOWN) {
		MyShip.vx -= 0.05 * cos(MyShip.direction * 3.14 / 180);
		MyShip.vy += 0.05 * sin(MyShip.direction * 3.14 / 180);
	}
	if (SPACE) {
		if (Blts.empty())
			MyShip.Shoot(Blts);
		else if (Blts.back().Time > 50)
			MyShip.Shoot(Blts);
	}
}

void FillStarsArray(RECT rt, POINT arr[]) {
	for (int i = 0; i < 150; i++) {
		arr[i].x = rand() % rt.right;
		arr[i].y = rand() % rt.bottom;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//srand(time(NULL));
	static bool MFlag = 0;
	static Star MyStar;
	static Player MyShip;
	static Enemy EnemyShip;
	//static vector <Ship> ship;
	static int count = 20;
	static POINT My_Massiv[3];
	static HPEN MyPen;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	char buffer[50];
	static POINT starsArr[150];
	static bool LEFT, UP, DOWN, RIGHT, SPACE;
	static int PlayerScore = 0, EnemyScore = 0;
	static HBRUSH hbr = CreateSolidBrush(RGB(100, 150, 150));
	
	LEFT = GetAsyncKeyState(VK_LEFT);
	RIGHT = GetAsyncKeyState(VK_RIGHT);
	UP = GetAsyncKeyState(VK_UP);
	DOWN = GetAsyncKeyState(VK_DOWN);
	SPACE = GetAsyncKeyState(VK_SPACE);

	CheckButtons(UP, DOWN, LEFT, RIGHT, SPACE, MyShip, rand()%1000+1);


	GetClientRect(hWnd, &rt);
	hdc = GetDC(hWnd);
	if (MyShip.Killed() && Blts2.empty() != 1 || MyShip.Sun(MyStar)) {
		Reset(rt, MyStar, EnemyShip, MyShip);
		MFlag = 0;
		EnemyScore += 1;
	}

	if (EnemyShip.Killed() || EnemyShip.Sun(MyStar)) {
		Reset(rt, MyStar, EnemyShip, MyShip);
		MFlag = 0;
		PlayerScore += 1;
	}
	ReleaseDC(hWnd, hdc);

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rt);
		MyStar.x = rt.right/2;
		MyStar.y = rt.bottom/2;

		EnemyShip.x = rt.right - 20;
		EnemyShip.y = rt.bottom - 20;
		EnemyShip.direction = 135;
		FillStarsArray(rt, starsArr);
		break;
	case WM_PAINT: 				
		hdc = BeginPaint(hWnd, &ps); 	
		GetClientRect(hWnd, &rt); 		

	//	DrawSun(hdc, rt);

	//	Stars(hdc, starsArr);

		Step(MyStar, MyShip);
		if (MyShip.x > rt.right) {
			MyShip.x = 0;
		}
		if (MyShip.x < 0) {
			MyShip.x = rt.right;
		}
		if (MyShip.y > rt.bottom) {
			MyShip.y = 0;
		}
		if (MyShip.y < 0) {
			MyShip.y = rt.bottom;
		}

		if (EnemyShip.x > rt.right) {
			EnemyShip.x = 0;
		}
		if (EnemyShip.x < 0) {
			EnemyShip.x = rt.right;
		}
		if (EnemyShip.y > rt.bottom) {
			EnemyShip.y = 0;
		}
		if (EnemyShip.y < 0) {
			EnemyShip.y = rt.bottom;
		}

		
		EnemyShip.DetectSun(MyStar);
		EnemyShip.RunFromSun();

		if (!NearSun) {
				EnemyShip.ChangeDirection();
			if(!NearPlayer)
				EnemyShip.MoveToPlayer();
		}


		EnemyShip.DetectPlayer(MyShip);
		EnemyShip.RunFromPlayer();


		EnemyShip.ShootPlayer();
		Step(MyStar, EnemyShip);

		hbr = CreateSolidBrush(RGB(200, 200, 50));
		SelectObject(hdc, hbr);
		Ellipse(hdc, MyStar.x - MyStar.r, MyStar.y - MyStar.r, MyStar.x + MyStar.r, MyStar.y + MyStar.r);
		DeleteObject(hbr);

		hbr = CreateSolidBrush(RGB(100, 150, 100));
		SelectObject(hdc, hbr);
		SelectObject(hdc, MyPen);
		MyShip.GetAngles();
		Polygon(hdc, MyShip.angles, 5);
		DeleteObject(hbr);
		DeleteObject(MyPen);

		hbr = CreateSolidBrush(RGB(150, 100, 100));
		SelectObject(hdc, hbr);
		SelectObject(hdc, MyPen);
		EnemyShip.GetAngles();
		Polygon(hdc, EnemyShip.angles, 5);
		DeleteObject(hbr);
		DeleteObject(MyPen);

		TextOut(hdc, 5, 5, buffer, wsprintf(buffer, "Player: %d", PlayerScore));
		TextOut(hdc, rt.right - 70, 5, buffer, wsprintf(buffer, "Enemy: %d", EnemyScore));

		DrawBlt(rt, MyStar, hdc, Blts);
		DrawBlt(rt, MyStar, hdc, Blts2);

		DrawPrt(rt, MyStar, hdc, Prts1);
		DrawPrt(rt, MyStar, hdc, Prts2);

		Sleep(12);

		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps); 		
		break;
	case WM_KEYDOWN:
		if (wParam == VK_UP && wParam == VK_RIGHT) {
			MyShip.direction -= 10;
			MyShip.vx += 0.2 * cos(MyShip.direction * 3.14 / 180);
			MyShip.vy -= 0.2 * sin(MyShip.direction * 3.14 / 180);
		}
		switch (wParam) {
		case VK_ESCAPE: {

			if (!MFlag)
				MFlag = 1;
			else MFlag = 0;

			break; }
		}
		break;
	case WM_LBUTTONDOWN:
		MyShip.x = LOWORD(lParam);
		MyShip.y = HIWORD(lParam);
		break;
	case WM_DESTROY: 				
		PostQuitMessage(0);
		break;
	default:
		if (MFlag) {
			InvalidateRect(hWnd, NULL, 1);
		}
		
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}