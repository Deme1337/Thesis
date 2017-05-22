#include "common.h"
#include "GeoBuffer.h"



GeoBuffer::GeoBuffer()
{
	int i;

	for (i = 0; i< GBUFFER_COUNT; i++)
	{
		renderTargetArray[i] = 0;
		renderTargetViewArray[i] = 0;
		shaderResourceViewArray[i] = 0;
	}

	depthBuffer = 0;
	depthStencilView = 0;
}


GeoBuffer::~GeoBuffer()
{
}

bool GeoBuffer::InitializeBuffer(DirectXDevice *dev, int width, int height, float screenDepth, float screenNear, DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC textureDesc, textureDesc1;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	int i;


	// Store the width and height of the render texture.
	textureBufferWidth = width;
	textureBufferHeight = height;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));


	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;



	// Create the render target textures.
	for (i = 0; i< GBUFFER_COUNT; i++)
	{
		result = dev->GetDevice()->CreateTexture2D(&textureDesc, NULL, &renderTargetArray[i]);
		if (FAILED(result))
		{
			PRINTL("Cannot init gbuffer");
			return false;
		}

	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	for (i = 0; i< GBUFFER_COUNT; i++)
	{
		result = dev->GetDevice()->CreateRenderTargetView(renderTargetArray[i], &renderTargetViewDesc, &renderTargetViewArray[i]);
		if (FAILED(result))
		{
			PRINTL("Cannot create rtvs");
			return false;
		}
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (i = 0; i< GBUFFER_COUNT; i++)
	{
		result = dev->GetDevice()->CreateShaderResourceView(renderTargetArray[i], &shaderResourceViewDesc, &shaderResourceViewArray[i]);
		if (FAILED(result))
		{
			PRINTL("Cannot create srvs");
			return false;
		}
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = dev->GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &depthBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = dev->GetDevice()->CreateDepthStencilView(depthBuffer, &depthStencilViewDesc, &depthStencilView);
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


	return true;
}

void GeoBuffer::Release()
{
	int i;


	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if (depthBuffer)
	{
		depthBuffer->Release();
		depthBuffer = 0;
	}

	for (i = 0; i< GBUFFER_COUNT; i++)
	{
		if (shaderResourceViewArray[i])
		{
			shaderResourceViewArray[i]->Release();
			shaderResourceViewArray[i] = 0;
		}

		if (renderTargetViewArray[i])
		{
			renderTargetViewArray[i]->Release();
			renderTargetViewArray[i] = 0;
		}

		if (renderTargetArray[i])
		{
			renderTargetArray[i]->Release();
			renderTargetArray[i] = 0;
		}
	}

}

void GeoBuffer::SetRenderTargets(DirectXDevice *dev)
{
	dev->GetDeviceContext()->OMSetRenderTargets(GBUFFER_COUNT, renderTargetViewArray, depthStencilView);

	
	dev->GetDeviceContext()->RSSetViewports(1, &viewport);
}

void GeoBuffer::ClearRenderTargets(DirectXDevice *dev, float red, float green, float blue, float alpha)
{
	float color[4];
	int i;


	
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;


	for (i = 0; i< GBUFFER_COUNT; i++)
	{
		dev->GetDeviceContext()->ClearRenderTargetView(renderTargetViewArray[i], color);
	}

	
	dev->GetDeviceContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView * GeoBuffer::GetShaderResourceView(int i)
{
	return shaderResourceViewArray[i];
}
