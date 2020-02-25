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
	double r = 3;
	double m = 10;
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

vector <Bullet> Blts;
vector <Bullet> Blts2;

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
		if (PlayerDirection - direction < 10)
			direction = PlayerDirection;
		direction += (PlayerDirection - direction) / 15;
	}
	
	void ShootPlayer() {
		if (abs(direction - PlayerDirection) < 20)
		if (Blts2.empty())
			Shoot(Blts2);
		else if (Blts2.back().Time > 40)
			Shoot(Blts2);
	}

	void MoveToPlayer(){

		if (sqrt(vx * vx + vy * vy) > 5) {
			vx *= 0.9;
			vy *= 0.9;
		}
			vx += cos(PlayerDirection * 3.14 / 180) * 0.2;
			vy -= sin(PlayerDirection * 3.14 / 180) * 0.2;

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
		if (DistanceToSun < 180) {
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

void Reset(RECT &rt, Star &MyStar, Enemy &EnemyShip, Player &MyShip) {
	MyStar.x = rt.right/2;
	MyStar.y = rt.bottom/2;
	MyStar.m = 1300;
	MyStar.r = 10;

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
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); 
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
		1000, 			
		800, 			
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

		hbr = CreateSolidBrush(RGB(200, 100, 50));
		SelectObject(hdc, hbr);
		Ellipse(hdc, bullets[i].x - bullets[i].r, bullets[i].y - bullets[i].r, bullets[i].x + bullets[i].r, bullets[i].y + bullets[i].r);
		DeleteObject(hbr);
	}
}

void CheckButtons(bool UP, bool DOWN, bool LEFT, bool RIGHT, bool SPACE, Player &MyShip) {
	if (sqrt(MyShip.vx * MyShip.vx + MyShip.vy * MyShip.vy) > 5) {
		MyShip.vx *= 0.9;
		MyShip.vy *= 0.9;
	}
	if(UP){
		MyShip.vx += 0.05 * cos(MyShip.direction * 3.14 / 180);
		MyShip.vy -= 0.05 * sin(MyShip.direction * 3.14 / 180);
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
		else if (Blts.back().Time > 25)
			MyShip.Shoot(Blts);
	}
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	srand(time(NULL));
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
	static bool LEFT, UP, DOWN, RIGHT, SPACE;
	static int PlayerScore = 0, EnemyScore = 0;
	static HBRUSH hbr = CreateSolidBrush(RGB(100, 150, 150));
	
	LEFT = GetAsyncKeyState(VK_LEFT);
	RIGHT = GetAsyncKeyState(VK_RIGHT);
	UP = GetAsyncKeyState(VK_UP);
	DOWN = GetAsyncKeyState(VK_DOWN);
	SPACE = GetAsyncKeyState(VK_SPACE);

	CheckButtons(UP, DOWN, LEFT, RIGHT, SPACE, MyShip);


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
		MyStar.m = 1300;
		MyStar.r = 10;

		EnemyShip.x = rt.right - 20;
		EnemyShip.y = rt.bottom - 20;
		EnemyShip.direction = 135;
		break;
	case WM_PAINT: 				
		hdc = BeginPaint(hWnd, &ps); 	
		GetClientRect(hWnd, &rt); 		

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

		Sleep(20);
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