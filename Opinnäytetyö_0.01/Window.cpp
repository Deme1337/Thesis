#include "common.h"
#include "Window.h"
#include "UserInterface.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (TwEventWin(hWnd, message, wParam, lParam))
	{
		return 0;
	}

	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

Window::Window()
{
}


Window::~Window()
{
}

bool Window::InitializeWindow(int width, int height, LPCWSTR title)
{
	PRINTL("Valmistellaan ikkuna");

	if (!CreateWindowClass(title))
	{
		return false;
	}
	

	DWORD style = WS_CAPTION  | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	BOOL success = AdjustWindowRectEx(&rect, style, false, 0);
	
	if (!success)
		return false;

	HWND window = CreateWindowEx(NULL, title, title, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, 
		rect.bottom - rect.top,
		nullptr, 
		nullptr, 
		GetModuleHandle(0),
		nullptr); 

	
	this->_width = rect.right - rect.left;
	this->_height = rect.bottom - rect.top;

	ShowWindow(window, SW_SHOWDEFAULT);
	this->_hWnd = window;

	PRINTL("Ikkuna luotu, resoluutio: " + std::to_string(_width) + "x" + std::to_string(_height));

	return true;
}

int Window::WindowMessageHandler()
{
	MSG msg;

	if (Keys::key(VKEY_ESCAPE))
	{
		PostQuitMessage(0);
	}

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			return 1;
		}

		
	}
	
	return 0;
}

bool Window::CreateWindowClass(LPCWSTR classname)
{
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = classname;
	auto result = RegisterClassEx(&wc);

	if (!result)
	{
		PRINTL("WNDCLASS Error");
		return false;
	}

	return true;
}
