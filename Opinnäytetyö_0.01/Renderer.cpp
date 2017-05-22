#include "Renderer.h"
#include "Camera.h"
#include <iostream>
#include <fstream>

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

bool Renderer::InitializeRenderer(Window window, DirectXDevice * devclass)
{
	this->devclass = devclass;
	mainwindow = window;
	gshader = new GeometryShader();
	if (!gshader->InitShader(L"GVertexShader.hlsl",L"GPixelShader.hlsl",devclass))
	{
		PRINT("Cannot init gshader");
		return false;
	}

	lshader = new LightShader();
	if (!lshader->InitializeLightShader(devclass, L"LightVertexShader.hlsl", L"LightPixelShader.hlsl"))
	{
		PRINT("Cannot init light shader");
		return false;
	}

	gbuffer = new GeoBuffer();
	if (!gbuffer->InitializeBuffer(devclass,window._width,window._height,100.0f,0.001f, DXGI_FORMAT_R32G32B32A32_FLOAT))
	{
		PRINT("Cannot init geometry buffer");
		return false;
	}

	targetWindow = new FullScreenQuad();
	if (!targetWindow->Initialize(devclass->GetDevice(), window._width, window._height))
	{
		PRINT("Cannot init render target window");
	}

	cam = Camera(mainwindow._hWnd);

	cam.SetCameraPosition(XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f));

	for (size_t i = 0; i < POINT_LIGHTS;  i++)
	{
		PointLight p = PointLight();
		p.PointlightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		p.PointlightPosition = XMFLOAT4(-20.0 * sin(i)  , 20.0f, 3.0f, 1.0f);
		plights.push_back(p);
	}


	std::vector<std::string> images;
	//Environment map textures
	{

		//std::string im1 = "Textures\\TropicalSunnyDay\\TropicalSunnyDayBack2048.png";
		//std::string im2 = "Textures\\TropicalSunnyDay\\TropicalSunnyDayDown2048.png";
		//std::string im3 = "Textures\\TropicalSunnyDay\\TropicalSunnyDayFront2048.png";
		//std::string im4 = "Textures\\TropicalSunnyDay\\TropicalSunnyDayLeft2048.png";
		//std::string im5 = "Textures\\TropicalSunnyDay\\TropicalSunnyDayRight2048.png";
		//std::string im6 = "Textures\\TropicalSunnyDay\\TropicalSunnyDayUp2048.png";

		std::string im1	= "Textures\\yokohama\\negz.jpg";
		std::string im2 = "Textures\\yokohama\\negy.jpg";
		std::string im3 = "Textures\\yokohama\\posz.jpg";
		std::string im4 = "Textures\\yokohama\\negx.jpg";
		std::string im5 = "Textures\\yokohama\\posx.jpg";
		std::string im6 = "Textures\\yokohama\\posy.jpg";
										 

		//Need to write order down
		images.push_back(im4);
		images.push_back(im5);
		images.push_back(im2);
		images.push_back(im6);
		images.push_back(im3);
		images.push_back(im1);

	}

	cubemap = new CTexture();
	if (!cubemap->LoadCubeMap(devclass, images))
	{
		PRINTL("Cannot load cubemap");
	}


	
	

	return true;
}

void Renderer::GeometryPass(float frametime)
{
	cam.UpdateCamera(frametime);


	devclass->TurnCullingOff();
	devclass->TurnZBufferOn();

	XMMATRIX projection, view;


	gbuffer->SetRenderTargets(devclass);
	gbuffer->ClearRenderTargets(devclass, 0.0f, 0.0f, 0.0f, 1.0f);

	projection = devclass->GetProjectionMatrix();
	view = cam.GetCameraView();

	for (size_t i = 0; i < actors.size(); i++)
	{
		gshader->UpdateShader(actors[i]->GetActorWorldMatrix(), view, projection, false, actors[i]->texOffset);
		actors[i]->DrawActor(gshader);

		projection = devclass->GetProjectionMatrix();
		view = cam.GetCameraView();
	}
}


void Renderer::LightPass(Settings s)
{

	devclass->TurnZBufferOff();

	RendererInput();
	


	targetWindow->UpdateWindow(devclass->GetDeviceContext(), rtOffSet, rtOffSety);

	devclass->ResetViewPort();
	devclass->SetBackBufferRenderTarget();

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	worldMatrix = XMMatrixIdentity();
	orthoMatrix = devclass->GetOrthoMatrix();
	baseViewMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	
	lshader->UpdateTexture(cubemap->cubeGetTexture(), 7);
	lshader->UpdateSettings(s.CookTorrance, s.Phong, s.BLinnPhong, s.na, s.ShowGbuffer, s.ONDiffuse, s.Lambertian);
	lshader->UpdateTexture(cubemap->cubeGetTexture(), 5);
	lshader->UpdateCameraMatrix(cam.GetCameraPosition());
	lshader->UpdateShaderParameters(worldMatrix, baseViewMatrix, orthoMatrix, gbuffer->GetShaderResourceView(0), gbuffer->GetShaderResourceView(1), 
		gbuffer->GetShaderResourceView(2), gbuffer->GetShaderResourceView(3), 
		gbuffer->GetShaderResourceView(4), gbuffer->GetShaderResourceView(5), gbuffer->GetShaderResourceView(6), DirectionalLight, plights);



	targetWindow->Render(devclass->GetDeviceContext());
	lshader->UseShader(targetWindow->GetIndexCount());

}



void Renderer::Release()
{

	for (size_t i = 0; i < actors.size(); i++)
	{
		actors[i]->Release();
	}

	targetWindow->Shutdown();
	gshader->Release();
	lshader->Release();
	gbuffer->Release();
	
}

void Renderer::RendererInput()
{
	if(Keys::key(VKEY_NUM4))
	{
		rtOffSet -= 1;
	}
	if (Keys::key(VKEY_NUM6))
	{
		rtOffSet += 1;
	}
	if (Keys::key(VKEY_NUM2))
	{
		rtOffSety -= 1;
	}
	if (Keys::key(VKEY_NUM8))
	{
		rtOffSety += 1;
	}



	if (Keys::key(VKEY_G))
	{
		plights[1].PointlightPosition.x -= 0.1f;
	}
	if (Keys::key(VKEY_J))
	{
		plights[1].PointlightPosition.x += 0.1f;
	}

	if (Keys::key(VKEY_H))
	{
		plights[1].PointlightPosition.z -= 0.1f;
	}
	if (Keys::key(VKEY_Y))
	{
		plights[1].PointlightPosition.z += 0.1f;
	}
}

void Renderer::UpdateShaderPointLightNum(int n)
{
	std::ofstream myfile;
	myfile.open("plightcount.hlsl");
	myfile << std::to_string(n);
	myfile.close();
}
