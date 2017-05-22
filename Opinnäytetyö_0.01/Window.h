#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>


class Window
{
private:


	WNDCLASSEX wc;
	HINSTANCE hInst;

public:
	Window();
	~Window();

	bool InitializeWindow(int width, int height, LPCWSTR title);

	int WindowMessageHandler();


	HWND _hWnd;
	
	int _width;
	int _height;

private:

	bool CreateWindowClass(LPCWSTR classname);

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif