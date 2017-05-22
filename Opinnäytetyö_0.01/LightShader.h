#pragma once

#ifndef LIGHTSHADER_H
#define LIGHTSHADER_H

#include "DirectXDevice.h"
#include "GeoBuffer.h"
#include "Camera.h"

#include <vector>

struct PointLight
{
	XMFLOAT4 PointlightPosition;
	XMFLOAT4 PointlightColor;
};


#define POINT_LIGHTS 4

class LightShader
{
private:
	DirectXDevice* devclass;


	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBuffer
	{
		XMVECTOR LightPosition;
		XMVECTOR LightColor;
		//float GlobalAmbient;
	};

	struct SettingsBuffer
	{
		int BLinnPhongs;
		int Phongs;
		int CookTorrances;
		int Wards;
		int ShowGBuffer;
		int ONDiffuse;
		int Lambertian;
		XMVECTOR vvv;
	};


	struct DisneyParam
	{
		XMFLOAT4 subspectintani;
		XMFLOAT4 sheentintcleargloss;
	};

 public:
	struct PointLightBuffer
	{
		XMVECTOR PointLightPosition[POINT_LIGHTS];
		XMVECTOR PointLightColor[POINT_LIGHTS];
	};

private:
	struct CameraBuffer
	{
		XMVECTOR CameraPosition;
	};

public:
	LightShader();
	~LightShader();

	bool InitializeLightShader(DirectXDevice* dev, WCHAR* vshader, WCHAR* pshader);

	void UpdateTexture(ID3D11ShaderResourceView* tex, int index);

	void UpdateSettings(bool cook, bool phong, bool blinn, bool na, int gbuffer, bool onayar, bool lambert);

	void UpdateShaderParameters(XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix,
		ID3D11ShaderResourceView * colorTexture, ID3D11ShaderResourceView * normalTexture,
		ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* positionTexture,
		ID3D11ShaderResourceView* roughnessTexture, ID3D11ShaderResourceView* tangentTexture,
		ID3D11ShaderResourceView* binormalTexture, XMFLOAT4 &LightPosition, std::vector<PointLight> plights);

	void UpdateCameraMatrix(XMVECTOR cameraposition);

	void UseShader(int icount);

	void Release();

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleClamp;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* pointLightBuffer;
	ID3D11Buffer* settingsBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* disneyBuffer;
};

#endif