#include <windows.h> // підключення бібліотеки з функціями API
#include <cmath>
#include <ctime>
#include <vector>
using namespace std;
// Глобальні змінні:
HINSTANCE hInst; 	//Дескриптор програми	
LPCTSTR szWindowClass = "QWERTY";
LPCTSTR szTitle = "ПЕРША ПРОГРАМА";

double dt = 1;
double G = 0.3;
// Попередній опис функцій

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


class Star {
public:
	double x;
	double y;
	double vx = 0;
	double vy = 0;
	double r = 3;
	double m = 10;
};

class Bullet{
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

class Ship{
public:
	double x = 10;
	double y = 10;
	double vx = 0;
	double vy = 0;
	double r = 5;
	double m = 10;
	POINT angles[5];
	int direction = 0;
	int t = direction ;

	void Shoot() {
		Bullet tBlt;
		tBlt.x = angles[0].x;
		tBlt.y = angles[0].y;
		tBlt.vx = vx + cos(direction * 3.14 / 180) *3;
		tBlt.vy = vy -sin(direction * 3.14 / 180) *3;
		Blts.push_back(tBlt);
	}




	void GetAngles() {
		direction += 360;
		direction = direction % 360;
		angles[0].x = x + 25 * cos(direction * 3.14 / 180);
		angles[0].y = y - 25 * sin(direction * 3.14 / 180);
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

private:

};



// Основна програма 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	// Реєстрація класу вікна 
	MyRegisterClass(hInstance);

	// Створення вікна програми
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// Цикл обробки повідомлень
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void Step(Star star, Ship &ship) {
	double a, ax, ay, dx, dy, r;
			dx = star.x - ship.x;
			dy = star.y - ship.y;

			r = dx * dx + dy * dy;// тут R^2
			a = G * star.m / r;

			r = sqrt(r); // тут R
			ax = a * dx / r; // a * cos
			ay = a * dy / r; // a * sin

			ship.vx += ax * dt;
			ship.vy += ay * dt;
	// координаты меняем позже, потому что они влияют на вычисление ускорения
		ship.x += ship.vx * dt;
		ship.y += ship.vy * dt;
}

void StepBlt(Star star, Bullet& blt) {
	double a, ax, ay, dx, dy, r;
	dx = star.x - blt.x;
	dy = star.y - blt.y;

	r = dx * dx + dy * dy;// тут R^2
	a = G * star.m / r;

	r = sqrt(r); // тут R
	ax = a * dx / r; // a * cos
	ay = a * dy / r; // a * sin

	blt.vx += ax * dt;
	blt.vy += ay * dt;
	// координаты меняем позже, потому что они влияют на вычисление ускорения
	blt.x += blt.vx * dt;
	blt.y += blt.vy * dt;
	blt.Time += 1;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//стиль вікна
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//віконна процедура
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//дескриптор програми
	wcex.hIcon = LoadIcon(NULL, IDI_HAND); 		//визначення іконки
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//визначення курсору
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); //установка фону
	wcex.lpszMenuName = NULL; 				//визначення меню
	wcex.lpszClassName = szWindowClass; 		//ім’я класу
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//реєстрація класу вікна
}

// FUNCTION: InitInstance (HANDLE, int)
// Створює вікно програми і зберігає дескриптор програми в змінній hInst

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
	hWnd = CreateWindow(szWindowClass, 	// ім’я класу вікна
		szTitle, 				// назва програми
		WS_OVERLAPPEDWINDOW,			// стиль вікна
		100, 			// положення по Х	
		50,			// положення по Y	
		750, 			// розмір по Х
		750, 			// розмір по Y
		NULL, 					// дескриптор батьківського вікна	
		NULL, 					// дескриптор меню вікна
		hInstance, 				// дескриптор програми
		NULL); 				// параметри створення.

	if (!hWnd) 	//Якщо вікно не творилось, функція повертає FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//Показати вікно
	UpdateWindow(hWnd); 				//Оновити вікно
	return TRUE;
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомлення, що приходять в додаток


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	srand(time(NULL));
	static bool MFlag = 0;
	static Star MyStar;
	static Ship MyShip;
	static vector <Ship> ship;
	static int count = 20;
	static POINT My_Massiv[3];
	static HPEN MyPen;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	static HBRUSH hbr = CreateSolidBrush(RGB(100,150,150));

	switch (message)
	{
	case WM_CREATE: 
		MyStar.x = 350;
		MyStar.y = 350;
		MyStar.m = 1300;
		MyStar.r = 10;

		break;
	case WM_PAINT: 				//Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); 	//Почати графічний вивід	
		GetClientRect(hWnd, &rt); 		//Область вікна для малювання
		
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
			
				hbr = CreateSolidBrush(RGB(200, 200, 50));
				SelectObject(hdc, hbr);
				Ellipse(hdc, MyStar.x-MyStar.r, MyStar.y - MyStar.r, MyStar.x + MyStar.r, MyStar.y + MyStar.r);
				DeleteObject(hbr);

				hbr = CreateSolidBrush(RGB(100, 150, 100));
				SelectObject(hdc, hbr);
				SelectObject(hdc, MyPen);
				MyShip.GetAngles();
				Polygon(hdc, MyShip.angles, 5);
				DeleteObject(hbr);
				DeleteObject(MyPen);

				for (int i = 0; i < Blts.size(); i++) {
					if (Blts[i].x > rt.right || Blts[i].x < 0 || Blts[i].y>rt.bottom || Blts[i].y < 0) {
						Blts.erase(Blts.cbegin()+i);
						continue;
					}
					if (Blts[i].Time > Blts[i].LifeTime) {
						Blts.erase(Blts.cbegin() + i);
						continue;
					}
					
					StepBlt(MyStar, Blts[i]);

					if (Blts[i].x > rt.right) {
						Blts[i].x = 0;
					}
					if (Blts[i].x < 0) {
						Blts[i].x = rt.right;
					}
					if (Blts[i].y > rt.bottom) {
						Blts[i].y = 0;
					}
					if (Blts[i].y < 0) {
						Blts[i].y = rt.bottom;
					}

					hbr = CreateSolidBrush(RGB(200, 100, 50));
					SelectObject(hdc, hbr);
					Ellipse(hdc, Blts[i].x - Blts[i].r, Blts[i].y - Blts[i].r, Blts[i].x + Blts[i].r, Blts[i].y + Blts[i].r);
					DeleteObject(hbr);
				}
	
				Sleep(20);
				ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps); 		//Закінчити графічний вивід	
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
		case VK_SPACE: {
			if (Blts.empty())
				MyShip.Shoot();
			else if (Blts.back().Time > 15)
				MyShip.Shoot();
			break;
		}
		case VK_UP: {
			MyShip.vx += 0.2 * cos(MyShip.direction * 3.14 / 180);
			MyShip.vy -= 0.2 * sin(MyShip.direction * 3.14 / 180);
			break;
		}
		case VK_RIGHT: {
			MyShip.direction -= 10;
			break;
		}
		case VK_LEFT: {
			MyShip.direction += 10;
			break;
		}
		case VK_DOWN: {
			MyShip.vx -= 0.2 * cos(MyShip.direction * 3.14 / 180);
			MyShip.vy += 0.2 * sin(MyShip.direction * 3.14 / 180);
			break;
		}
		}
		break;
	case WM_LBUTTONDOWN:
		MyShip.x = LOWORD(lParam);
		MyShip.y = HIWORD(lParam);
		break;
	case WM_DESTROY: 				//Завершення роботи
		PostQuitMessage(0);
		break;
	default:
		if (MFlag) {
			InvalidateRect(hWnd, NULL, 1);
		}
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
