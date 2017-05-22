#include "common.h"
#include "Window.h"
#include "Engine.h"

#include "Timer.h"

Window mainWindow = Window();
Engine engine;



int MessageLoop()
{
	int frameCount = 0;
	int fps = 0;
	double frameTime = 0;


	bool running = true;
	while (running)
	{

		if (mainWindow.WindowMessageHandler() == 1)
		{
			running = false;
			break;
		}
		
		frameCount++;

		if (Timer::GetTime() >= 1.0f)
		{
			Timer::SetDeltaTime(frameTime);
			fps = 0;
			frameCount = 0;
			Timer::StartTimer();
		}
		frameTime = Timer::GetFrameTime();
		int fps = 1 / frameTime;
		std::wstring sd = L"::::";
		SetWindowText(mainWindow._hWnd, std::to_wstring(fps).c_str());

		engine.Update(frameTime);
	

	}
	
	engine.Release();
	return 0;
}

bool enterFullscreen(HWND hwnd, int fullscreenWidth, int fullscreenHeight, int colourBits, int refreshRate) {
	DEVMODE fullscreenSettings;
	bool isChangeSuccessful;
	RECT windowBoundary;

	EnumDisplaySettings(NULL, 0, &fullscreenSettings);
	fullscreenSettings.dmPelsWidth = fullscreenWidth;
	fullscreenSettings.dmPelsHeight = fullscreenHeight;
	fullscreenSettings.dmBitsPerPel = colourBits;
	fullscreenSettings.dmDisplayFrequency = refreshRate;
	fullscreenSettings.dmFields = DM_PELSWIDTH |
		DM_PELSHEIGHT |
		DM_BITSPERPEL |
		DM_DISPLAYFREQUENCY;

	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
	SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	SetWindowPos(hwnd, HWND_TOP, 0, 0, fullscreenWidth, fullscreenHeight, SWP_SHOWWINDOW);
	isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
	ShowWindow(hwnd, SW_MAXIMIZE);

	return isChangeSuccessful;
}



int main(int argc, char *argv[])
{

	if (!mainWindow.InitializeWindow(1920, 1080, L"Oppari"))
	{
		PRINTL("Ikkunan alustus ei onnistunut")
	}

	engine = Engine();

	if (!engine.InitializeEngine(mainWindow))
	{
		PRINTL("Cannot init graphics engine");
	}


	
	if (!enterFullscreen(mainWindow._hWnd, 1920, 1080, 32, 70))
	{
		PRINTL("Fullscreen epäonnistui");
	}

	return MessageLoop();
}