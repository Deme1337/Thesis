#pragma once
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H
#include "DirectXDevice.h"
#include "FullScreenQuad.h"
#include "Window.h"
#include "Renderer.h"
#include "UserInterface.h"

class Engine
{
private:
	unsigned int objectIndex = 0;

	void EngineInput();

public:
	Engine();
	~Engine();

	bool InitializeEngine(Window mainwindow);

	void Update(float frametime);

	void Release();


private:
	DirectXDevice* devclass;
	Renderer* renderer;
	UserInterface* gui;

	Window _mainWindow;

	Settings settings;

};

#endif