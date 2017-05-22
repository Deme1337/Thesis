#pragma once
#ifndef CAMERA_H
#define CAMERA_H


#include <directxmath.h>
#include "Timer.h"
#include <Windows.h>
using namespace DirectX;



class Camera
{
public:
	Camera(HWND hWnd);
	Camera(const Camera&);
	Camera();
	~Camera();



	void UpdateCamera(float frametime);
	//sets camera position
	void SetCameraPosition(XMVECTOR pos);

	XMVECTOR GetCameraPosition() { return position; }
	//returns camera view matrix
	XMMATRIX GetCameraView();
	XMVECTOR LookAt;
	XMMATRIX GetBaseViewMatrix();

	XMFLOAT4 CameraPositionXF;


private:
	HWND viewPort;
	float horizontalangle = 0.0f;
	float verticalangle = 0.0f;
	float mousespeed = 0.005f;
	float speed = 100.0f;
	int width = 0;
	int height = 0;

	POINT pLastPos, pInitialPos;

	XMVECTOR position;
	XMVECTOR up;
	XMVECTOR right;

	Timer *timer;

public:
	float GetCameraSpeed() { return speed; }
	XMMATRIX camView;
};

#endif