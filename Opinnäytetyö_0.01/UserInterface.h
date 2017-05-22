#pragma once

#ifndef GUI_H
#define GUI_H
#include "DirectXDevice.h"
#include "Model.h"
#include "Window.h"
#include "Actor.h"
#include "LightShader.h"

#include <AntTweakBar.h>
#include <vector>



class UserInterface
{
private:
	TwBar* ioBar;
	TwBar* objBar;
	TwBar* lightBar;
	DirectXDevice* dev;

public:
	UserInterface();
	~UserInterface();

	void AddObjectToUI(Actor* a);
	void AddLightToUI(XMFLOAT4 &dirlight, int &cook, int &phong, int &blinn, int &ward, int &lamb, int &orennayar);
	void AddPointLight1(PointLight &p);

	unsigned int objindex = 0;

	void Release();
	bool InitInterface(DirectXDevice *dev, Window w);


	void Update();
};

#endif