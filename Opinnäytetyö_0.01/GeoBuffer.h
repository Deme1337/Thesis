#pragma once

#ifndef GEOBUFFER_H
#define GEOBUFFER_H
#include "DirectXDevice.h"

#define GBUFFER_COUNT 7

class GeoBuffer
{

private:
	int textureBufferWidth, textureBufferHeight;
	
	ID3D11Texture2D* renderTargetArray[GBUFFER_COUNT];
	ID3D11Texture2D* depthBuffer;

	ID3D11RenderTargetView* renderTargetViewArray[GBUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[GBUFFER_COUNT];

	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;

	

public:
	GeoBuffer();
	~GeoBuffer();

	bool InitializeBuffer(DirectXDevice *dev, int width, int height, float screenDepth, float screenNear, DXGI_FORMAT format);

	void Release();

	void SetRenderTargets(DirectXDevice *dev);

	/*floateilla m‰‰ritell‰‰n rendertargettien v‰ri*/
	void ClearRenderTargets(DirectXDevice *dev, float, float, float, float);

	ID3D11ShaderResourceView* GetShaderResourceView(int i);


};

#endif