#pragma once

#ifndef FULLSCREENQUAD_H
#define FULLSCREENQUAD_H

#include "DirectXDevice.h"

class FullScreenQuad
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	FullScreenQuad();
	FullScreenQuad(const FullScreenQuad&);
	~FullScreenQuad();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int m_bitmapWidth, m_bitmapHeight;

	void UpdateWindow(ID3D11DeviceContext* devcon, int w, int h);
	int GetIndexCount();

	int m_vertexCount;
	int m_indexCount;
private:
	bool InitializeBuffers(ID3D11Device*, int, int);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	int m_screenWidth, m_screenHeight;

	int m_previousPosX, m_previousPosY;
};

#endif