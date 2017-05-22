#include "common.h"
#include "Camera.h"





Camera::Camera(HWND hWnd)
{
	viewPort= hWnd;
	LookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	position = XMVectorSet(0.0f, 20.0f, -50.0f, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	camView = XMMatrixLookAtLH(position, position + LookAt, up);
}

Camera::Camera(const Camera &)
{
}

Camera::Camera()
{
}

Camera::~Camera()
{
}



XMMATRIX Camera::GetBaseViewMatrix()
{
	XMMATRIX baseViewMatrix = XMMatrixIdentity();
	XMFLOAT3 upb = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR positionb = position;
	XMFLOAT3 LookAtB; XMFLOAT3 positionFloat;

	XMStoreFloat3(&positionFloat, position);
	XMStoreFloat3(&LookAtB, LookAt);

	LookAtB.y = positionFloat.y;
	XMVECTOR LookAtBV = XMLoadFloat3(&LookAtB);

	baseViewMatrix = XMMatrixLookAtLH(positionb, LookAtBV, up);

	return baseViewMatrix;
}

void Camera::SetCameraPosition(XMVECTOR pos)
{
	this->position = pos;
}


XMMATRIX Camera::GetCameraView()
{
	return this->camView;
}


// to give the mouse some area to move
static POINT GetScreenCenter()
{
	POINT ret;

	ret.x = GetSystemMetrics(SM_CXSCREEN) / 2;
	ret.y = GetSystemMetrics(SM_CYSCREEN) / 2;

	return ret;
}


void Camera::UpdateCamera(float frametime)
{

	//Capture mouse position each frame even if mouse right btn is not pressed
	POINT cPos;
	GetCursorPos(&cPos);
	ScreenToClient(viewPort, &cPos);

	XMStoreFloat4(&CameraPositionXF, position);

	if (Keys::key(VKEY_RIGHT_BUTTON))
	{

		RECT rRect; GetClientRect(viewPort, &rRect);
		width = rRect.right;
		height = rRect.bottom;


		int centx = width / 2;
		int centy = height / 2;


		//Rotate camera only if mouse is moved outside "deadzone"
		if (centx - cPos.x > 5 || centx - cPos.x < -5 || centy - cPos.y > 5 || centy - cPos.y < -5)
		{
			horizontalangle -= (mousespeed * float(pLastPos.x - cPos.x)) * 1000.0f * Timer::GetDeltaTime();
			verticalangle += (mousespeed * float(pLastPos.y - cPos.y)) * 1000.0f * Timer::GetDeltaTime();

		}



		float xCos = std::cos(verticalangle) * std::sin(horizontalangle);
		float ySin = std::sin(verticalangle);
		float zCos = std::cos(verticalangle) * std::cos(horizontalangle);

		XMFLOAT3 dir = XMFLOAT3(xCos, ySin, zCos);

		LookAt = XMLoadFloat3(&dir);

		right = XMVectorSet(
			std::sin(horizontalangle - XM_PI / 2.0f),
			0.0f,
			std::cos(horizontalangle - XM_PI / 2.0f), 1.0f);

		up = XMVector3Cross(right, LookAt);

		if (Keys::key(VKEY_W)) position += LookAt  * speed * Timer::GetDeltaTime();
		if (Keys::key(VKEY_S)) position -= LookAt  * speed * Timer::GetDeltaTime();
		if (Keys::key(VKEY_A)) position += right   * speed * Timer::GetDeltaTime();
		if (Keys::key(VKEY_D)) position -= right   * speed * Timer::GetDeltaTime();

		camView = XMMatrixLookAtLH(position, position + LookAt, up);
	}

	//set cursor last position 
	pLastPos = cPos;

}

