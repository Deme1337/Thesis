#include "common.h"
#include "DirectXDevice.h"
#include <fstream>


void DirectXDevice::OutputErrorMessageD(ID3D10Blob * errorMessage, HWND hWnd, WCHAR * shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

DirectXDevice::DirectXDevice()
{
}


DirectXDevice::~DirectXDevice()
{
}

bool DirectXDevice::InitializeDirectXDevice(HWND hWnd, HINSTANCE hInst, int width, int height)
{
	HRESULT result;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_BLEND_DESC blendStateDescription;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	float fieldOfView, screenAspect;

	_hWnd = hWnd;
	_hInst = hInst;
	viewPortWidth = width;
	viewPortHeight = height;


	if (!CreateGFXFactory())
	{
		PRINTL("DirectX graphics interface creation fail");
		return false;
	}

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Double buffering
	swapChainDesc.BufferCount = 2;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	// Set the refresh rate of the back buffer.
	if (vSyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}


	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hWnd;

	// msaa settings
	swapChainDesc.SampleDesc.Count = sampleCount;
	swapChainDesc.SampleDesc.Quality = sampleQuality;

	// Set to full screen or windowed mode.

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}


	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	
	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(adapter,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&featureLevel,
		1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapchain, &dev, NULL, &devcon);

	if (FAILED(result))
	{
		MessageBox(hWnd, L"Cannot create device or swap chain", L"ERROR", MB_OK);
		PRINTL(std::to_string(result));
		return false;
	}


	// Get the pointer to the back buffer.
	result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = dev->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = sampleCount;
	depthBufferDesc.SampleDesc.Quality = sampleQuality;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = dev->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = dev->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	devcon->OMSetDepthStencilState(m_depthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 1;

	// Create the depth stencil view.
	result = dev->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	devcon->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = enableAA_;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = enableAA_;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = dev->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	devcon->RSSetState(m_rasterState);

	//Disable culling desc
	rasterDesc.CullMode = D3D11_CULL_NONE;

	result = dev->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
	if (FAILED(result))
	{
		return false;
	}

	rasterDesc.CullMode = D3D11_CULL_FRONT;
	result = dev->CreateRasterizerState(&rasterDesc, &m_rasterStateCullFront);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the viewport for rendering.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	devcon->RSSetViewports(1, &viewport);



	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = dev->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}



	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.AlphaToCoverageEnable = FALSE;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = dev->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}


	//Description for disabling alpha blending
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	// Create the blend state using the description.
	result = dev->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)width / (float)height;
	
	this->aspect = screenAspect;
	this->fov = fieldOfView;

	// Create the projection matrix for 3D rendering.
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 1.0f, 10000.0f);
	orthoMatrix = XMMatrixOrthographicLH(width, height, 1.0f, 10.0f);

	Initialized = true;

	return true;
}

void DirectXDevice::BeginFrame()
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(m_renderTargetView, color);
	devcon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectXDevice::EndFrame()
{
	if (vSyncEnabled)
	{
		swapchain->Present(1, 0);
	}
	else
	{
		swapchain->Present(0, 0);
	}
}

void DirectXDevice::SetBackBufferRenderTarget()
{
	devcon->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void DirectXDevice::AlphaBlendingOn()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	devcon->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void DirectXDevice::AlphaBlendingOff()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	devcon->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void DirectXDevice::TurnZBufferOn()
{
	devcon->OMSetDepthStencilState(m_depthStencilState, 1);
}

void DirectXDevice::TurnZBufferOff()
{
	devcon->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void DirectXDevice::TurnCullingBack()
{
	devcon->RSSetState(m_rasterState);
}

void DirectXDevice::TurnCullingOff()
{
	devcon->RSSetState(m_rasterStateNoCulling);
}

void DirectXDevice::TurnCullingFront()
{
	devcon->RSSetState(m_rasterStateCullFront);
}

void DirectXDevice::Release()
{
	SafeRelease(swapchain);
	SafeRelease(dev);
	SafeRelease(devcon);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_depthStencilBuffer);
	SafeRelease(m_rasterState);
	SafeRelease(m_rasterStateNoCulling);
	SafeRelease(m_depthStencilView);
	SafeRelease(m_depthDisabledStencilState);
	SafeRelease(m_alphaEnableBlendingState);
	SafeRelease(m_alphaDisableBlendingState);
	SafeRelease(m_rasterStateCullFront);
}

void DirectXDevice::GetVideoCardInfo(char * cardName, int & memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void DirectXDevice::UpdateViewPort(int w, int h)
{
	if (Initialized)
	{
		// Setup the viewport for rendering.
		viewport.Width = (float)w;
		viewport.Height = (float)h;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
}

void DirectXDevice::ResetViewPort()
{
	UpdateWindow(_hWnd);
	devcon->RSSetViewports(1, &viewport);
}



ID3D10Blob * DirectXDevice::CompileShader(WCHAR * fileName, const char * shaderType, LPCSTR entrypoint)
{
	ID3D10Blob* shaderBuffer = 0;
	ID3D10Blob* errorMessage = 0;
	HRESULT result;

	if (shaderType == "vertexshader")
	{
		// Compile the vertex shader code.
		result = D3DCompileFromFile(fileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
			&shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				OutputErrorMessageD(errorMessage, NULL, fileName);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(NULL, fileName, L"Missing Shader File", MB_OK);
			}

			return nullptr;
		}
		return shaderBuffer;
	}
	if (shaderType == "pixelshader")
	{
		// Compile the pixel shader code.
		result = D3DCompileFromFile(fileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
			&shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				OutputErrorMessageD(errorMessage, NULL, fileName);
			}
			// If there was nothing in the error message then it simply could not find the file itself.
			else
			{
				MessageBox(NULL, fileName, L"Missing Shader File", MB_OK);
			}

			return nullptr;
		}
		return shaderBuffer;
	}
	else
	{
		return nullptr;
	}
}

bool DirectXDevice::CreateGFXFactory()
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	size_t stringLength;


	int error;

	numModes = 0;
	
	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		MessageBox(_hWnd, L"Error with creating DXGI factory", L"ERROR", MB_OK);
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		MessageBox(_hWnd, L"Error with factory adapters", L"ERROR", MB_OK);
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		MessageBox(_hWnd, L"Error with adapter enumeration", L"ERROR", MB_OK);
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		MessageBox(_hWnd, L"Error getting displaymode list", L"ERROR", MB_OK);
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		MessageBox(_hWnd, L"Error creating display modelist", L"ERROR", MB_OK);
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)viewPortWidth)
		{
			if (displayModeList[i].Height == (unsigned int)viewPortHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		MessageBox(_hWnd, L"Error with adapter descriptor", L"ERROR", MB_OK);
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}



	 //Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;
	
	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;
	
	// Release the adapter.
	adapter->Release();
	adapter = 0;
	
	// Release the factory.
	factory->Release();
	factory = 0;


	return true;
}
