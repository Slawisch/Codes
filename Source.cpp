#include <windows.h> // ���������� �������� � ��������� API
#include <cmath>
#include <ctime>
#include <vector>
using namespace std;
// �������� ����:
HINSTANCE hInst; 	//���������� ��������	
LPCTSTR szWindowClass = "QWERTY";
LPCTSTR szTitle = "����� ��������";

double dt = 1;
double G = 0.3;
// ��������� ���� �������

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



// ������� �������� 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;

	// ��������� ����� ���� 
	MyRegisterClass(hInstance);

	// ��������� ���� ��������
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// ���� ������� ����������
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

			r = dx * dx + dy * dy;// ��� R^2
			a = G * star.m / r;

			r = sqrt(r); // ��� R
			ax = a * dx / r; // a * cos
			ay = a * dy / r; // a * sin

			ship.vx += ax * dt;
			ship.vy += ay * dt;
	// ���������� ������ �����, ������ ��� ��� ������ �� ���������� ���������
		ship.x += ship.vx * dt;
		ship.y += ship.vy * dt;
}

void StepBlt(Star star, Bullet& blt) {
	double a, ax, ay, dx, dy, r;
	dx = star.x - blt.x;
	dy = star.y - blt.y;

	r = dx * dx + dy * dy;// ��� R^2
	a = G * star.m / r;

	r = sqrt(r); // ��� R
	ax = a * dx / r; // a * cos
	ay = a * dy / r; // a * sin

	blt.vx += ax * dt;
	blt.vy += ay * dt;
	// ���������� ������ �����, ������ ��� ��� ������ �� ���������� ���������
	blt.x += blt.vx * dt;
	blt.y += blt.vy * dt;
	blt.Time += 1;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 		//����� ����
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//������ ���������
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//���������� ��������
	wcex.hIcon = LoadIcon(NULL, IDI_HAND); 		//���������� ������
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 	//���������� �������
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); //��������� ����
	wcex.lpszMenuName = NULL; 				//���������� ����
	wcex.lpszClassName = szWindowClass; 		//��� �����
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex); 			//��������� ����� ����
}

// FUNCTION: InitInstance (HANDLE, int)
// ������� ���� �������� � ������ ���������� �������� � ����� hInst

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //������ ���������� ������� � ����� hInst
	hWnd = CreateWindow(szWindowClass, 	// ��� ����� ����
		szTitle, 				// ����� ��������
		WS_OVERLAPPEDWINDOW,			// ����� ����
		100, 			// ��������� �� �	
		50,			// ��������� �� Y	
		750, 			// ����� �� �
		750, 			// ����� �� Y
		NULL, 					// ���������� ������������ ����	
		NULL, 					// ���������� ���� ����
		hInstance, 				// ���������� ��������
		NULL); 				// ��������� ���������.

	if (!hWnd) 	//���� ���� �� ���������, ������� ������� FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//�������� ����
	UpdateWindow(hWnd); 				//������� ����
	return TRUE;
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// ³����� ���������. ������ � �������� �� �����������, �� ��������� � �������


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
	case WM_PAINT: 				//������������ ����
		hdc = BeginPaint(hWnd, &ps); 	//������ ��������� ����	
		GetClientRect(hWnd, &rt); 		//������� ���� ��� ���������
		
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
		EndPaint(hWnd, &ps); 		//�������� ��������� ����	
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
	case WM_DESTROY: 				//���������� ������
		PostQuitMessage(0);
		break;
	default:
		if (MFlag) {
			InvalidateRect(hWnd, NULL, 1);
		}
		//������� ����������, �� �� �������� ������������
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
