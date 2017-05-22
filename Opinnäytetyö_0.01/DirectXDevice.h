#pragma once
#ifndef DXDEVICE_H
#define DXDEVICE_H

#include <d3d11_2.h>
#include <d3dcompiler.h>
#include <directxmath.h>
using namespace DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


class DirectXDevice
{
private:
	
	HWND _hWnd;
	HINSTANCE _hInst;

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	D3D11_VIEWPORT viewport;


	ID3D11Resource *dstbbResource;

	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context


	ID3D11RenderTargetView *backbuffer;    // global declaration
	ID3D11Texture2D *pBackBuffer;

	ID3D11RenderTargetView* m_renderTargetView;

	ID3D11Texture2D* m_depthStencilBuffer;

	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthDisabledStencilState;

	ID3D11RasterizerState* m_rasterState;
	ID3D11RasterizerState* m_rasterStateNoCulling;
	ID3D11RasterizerState* m_rasterStateCullFront;


	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;


	int viewPortWidth, viewPortHeight;
	int sampleCount = 1;
	int sampleQuality = 0;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	unsigned int numModes, i, numerator, denominator;

	static void OutputErrorMessageD(ID3D10Blob * errorMessage, HWND hWnd, WCHAR * shaderFilename);

public:

	bool vSyncEnabled = false;
	bool fullscreen = false;
	bool enableAA_ = false;
	bool Initialized = false;
	float fov = 0.0f;
	float aspect = 0.0f;

public:
	DirectXDevice();
	~DirectXDevice();

	bool InitializeDirectXDevice(HWND hWnd, HINSTANCE hInst, int width, int height);


	void BeginFrame();
	void EndFrame();

	void SetBackBufferRenderTarget();

	void AlphaBlendingOn();
	void AlphaBlendingOff();

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnCullingBack();
	void TurnCullingOff();
	void TurnCullingFront();

	void Release();
	
	void GetVideoCardInfo(char* cardName, int& memory);

	void UpdateViewPort(int w, int h);
	void ResetViewPort();

	IDXGISwapChain* GetSwapChain() { return swapchain; }
	ID3D11Device* GetDevice() { return dev; }
	ID3D11DeviceContext* GetDeviceContext() { return devcon; }

	XMMATRIX GetProjectionMatrix() { return projectionMatrix; }
	XMMATRIX GetOrthoMatrix() { return orthoMatrix; }
	XMMATRIX projectionMatrix, orthoMatrix;


	static ID3D10Blob *CompileShader(WCHAR* fileName, const char* shaderType, LPCSTR entrypoint);

private:
	bool CreateGFXFactory();

};

#endif