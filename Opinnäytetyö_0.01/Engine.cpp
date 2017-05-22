#include "common.h"
#include "Engine.h"



void Engine::EngineInput()
{
	
	if (Keys::onekey(VKEY_F1))
	{
		settings.CookTorrance = false;
		settings.Phong = false;
		settings.na = false;
		settings.BLinnPhong = true;
	}
	if (Keys::onekey(VKEY_F2))
	{
		settings.na = false;
		settings.CookTorrance = false;
		settings.Phong = true;
		settings.BLinnPhong = false;
	}
	if (Keys::onekey(VKEY_F3))
	{
		settings.na = false;
		settings.CookTorrance = true;
		settings.Phong = false;
		settings.BLinnPhong = false;
	}
	if (Keys::onekey(VKEY_F4))
	{
		settings.na = true;
		settings.CookTorrance = false;
		settings.Phong = false;
		settings.BLinnPhong = false;
	}

	if (Keys::onekey(VKEY_LEFT_ARROW))
	{
		if (settings.ShowGbuffer > 0)
		{
			settings.ShowGbuffer--;
		}
		else
		{
			settings.ShowGbuffer = 0;
		}
	}
	if (Keys::onekey(VKEY_RIGHT_ARROW))
	{
		if (settings.ShowGbuffer < GBUFFER_COUNT)
		{
			settings.ShowGbuffer++;
		}
		else
		{
			settings.ShowGbuffer = 0;
		}
	}

	
	if (Keys::onekey(VKEY_LEFT_BUTTON))
	{
		float rayLength = 0.0f;
		float rayLengthLast = 500.0f;
		PRINTL("---------------------------");
		for (size_t i = 0; i < renderer->actors.size(); i++)
		{
			POINT cPos;
			GetCursorPos(&cPos);
			ScreenToClient(_mainWindow._hWnd, &cPos);
			
			if (renderer->actors[i]->RayHit(renderer->cam, XMFLOAT2(cPos.x, cPos.y), _mainWindow._width, _mainWindow._height, rayLength))
			{
				
				if (rayLength < rayLengthLast)
				{
					objectIndex = i;
					gui->AddObjectToUI(renderer->actors[objectIndex]);
					rayLengthLast = rayLength;
					//MessageBox(_mainWindow._hWnd, L"HIT", std::wstring(std::to_wstring(objectIndex)).c_str(), MB_OK);
					PRINTL("HIT: " + std::string(renderer->actors[i]->actorFile) + "Ray length: " + std::string(std::to_string(rayLength)));
				}
		
			}
		
		}

		PRINTL("---------------------------");
	}

}

Engine::Engine()
{
	renderer = 0;
	devclass = 0;
	gui = 0;
}


Engine::~Engine()
{
}

bool Engine::InitializeEngine(Window mainwindow)
{
	devclass = new DirectXDevice();

	if (!devclass->InitializeDirectXDevice(mainwindow._hWnd, GetModuleHandle(0), mainwindow._width,mainwindow._height))
	{
		PRINTL("Cannot init device class");
		return false;
	}

	if (devclass->GetDevice() == nullptr)
		PRINT("Device is null");

	settings = Settings();
	settings.BLinnPhong = false;
	settings.CookTorrance = true;
	settings.Phong = false;
	settings.na = false;

	this->_mainWindow = mainwindow;

	/*
	Actor *a = new Actor();
	if (!a->InitializeActor(devclass, "Models\\plane.obj"))
		PRINT("Cannot load actor");

	a->SetActorPosition(XMFLOAT4(1.0f, 1.0f, 10.0f, 1.0f));
	a->SetActorScale(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	a->texOffset = 5.0f;


	Actor *a1 = new Actor();
	if (!a1->InitializeActor(devclass, "Models\\cube.obj"))
		PRINT("Cannot load actor");

	a1->SetActorPosition(XMFLOAT4(1.0f, 15.0f, 1.0f, 1.0f));
	a1->SetActorScale(XMFLOAT4(4.0f, 4.0f, 4.0f, 1.0f));
	a1->texOffset = 5.0f;
	*/

	Actor *a = new Actor();
	if (!a->InitializeActor(devclass, "Models\\plane.obj"))
		PRINT("Cannot load actor");

	a->SetActorPosition(XMFLOAT4(1.0f, 1.0f, 10.0f, 1.0f));
	a->SetActorScale(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	a->texOffset = 5.0f;


	Actor *a1 = new Actor();
	if (!a1->InitializeActor(devclass, "Models\\sphere.obj"))
		PRINT("Cannot load actor");

	a1->SetActorPosition(XMFLOAT4(1.0f, 15.0f, 10.0f, 1.0f));
	a1->SetActorScale(XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f));
	a1->texOffset = 1.0f;



	//a->SetTriangles();
	//a1->SetTriangles();
	
	

	renderer = new Renderer();
	if (!renderer->InitializeRenderer(mainwindow, devclass))
	{
		PRINT("Cannot init renderer");
	}

	renderer->AddObject(a);
	renderer->AddObject(a1);



	gui = new UserInterface();
	if (!gui->InitInterface(devclass, mainwindow))
	{
		PRINTL("Cannot init ui");
	}
	else
	{
		PRINTL("AntTweakBar ui initialized");
	}

	gui->AddLightToUI(renderer->DirectionalLight, settings.CookTorrance, settings.Phong, settings.BLinnPhong, settings.na, settings.Lambertian, settings.ONDiffuse);
	gui->AddPointLight1(renderer->plights[1]);
	gui->AddObjectToUI(renderer->actors[1]);

	return true;
}

void Engine::Update(float frametime)
{
	devclass->BeginFrame();
	devclass->AlphaBlendingOff();

	EngineInput();




	renderer->GeometryPass(frametime);

	renderer->LightPass(settings);




	gui->objindex = objectIndex;

	gui->Update();

	devclass->EndFrame();

}

void Engine::Release()
{
	gui->Release();
	devclass->Release();
	renderer->Release();
}
