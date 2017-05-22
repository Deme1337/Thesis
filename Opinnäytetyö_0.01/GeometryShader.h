#pragma once
#ifndef GSHADER_H
#define GSHADER_H
#include "DirectXDevice.h"


class GeometryShader
{
private:

	DirectXDevice *devclass;

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		float texOffSet;
		int hasAlpha;
	};

	struct ObjectData
	{
		XMFLOAT4 objColor;
		XMFLOAT4 objColorSpec;
		int useTextures;
		float roughnessOffset;
		float metallic;
	};

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampleStateWrap;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* ObjDataBuffer;

public:
	GeometryShader();
	~GeometryShader();


	bool InitShader(WCHAR *vshader, WCHAR *pshader, DirectXDevice *dev);


	void UpdateTexture(ID3D11ShaderResourceView* texture, int texindex);

	void SetObjectData(XMFLOAT4 data, XMFLOAT3 objcolor, XMFLOAT3 objcolorspec);
	
	void UpdateShader(XMMATRIX& world, XMMATRIX& view, XMMATRIX& projection, bool HasAlpha, float texoffset);


	void UseShader(int icount);
	
	void Release();
};

#endif