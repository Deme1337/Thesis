#pragma once

#ifndef  RENDERER_H
#define  RENDERER_H
#include "DirectXDevice.h"
#include "FullScreenQuad.h"
#include "Window.h"
#include "Camera.h"
#include "GeoBuffer.h"

#include "GeometryShader.h"
#include "LightShader.h"
#include "Actor.h"




class Renderer
{
public:
	Renderer();
	~Renderer();


	Camera cam;

	bool InitializeRenderer(Window window, DirectXDevice* devclass);

	void GeometryPass(float frametime);
	void LightPass(Settings s);

	void Release();

	void AddObject(Actor* a) { actors.push_back(a); }


	void RendererInput();

	std::vector<Actor*> actors;
	std::vector<PointLight> plights;

	XMFLOAT4 DirectionalLight = XMFLOAT4(2.0f, 1500.0f, 2.0f, 1.0f);

private:
	
	void UpdateShaderPointLightNum(int n);
	
	
	DirectXDevice *devclass;
	GeometryShader* gshader;
	LightShader* lshader;
	GeoBuffer* gbuffer;

	CTexture *cubemap;


	FullScreenQuad* targetWindow;


	Window mainwindow;






	int rtOffSet = -420;
	int rtOffSety = 0;
};

#endif 
