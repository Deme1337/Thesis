#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H
#include "DirectXDevice.h"
#include <vector>

class CTexture
{
private:

	unsigned char* texturedata;
	ID3D11Texture2D* texture;

	ID3D11ShaderResourceView* srvCubeMap = 0;
	ID3D11Texture2D* srcTex[6];

public:
	CTexture();
	CTexture(const CTexture&);
	~CTexture();

	void Release();

	bool LoadFreeImage(DirectXDevice* dev, const char* filepath);

	bool LoadCubeMap(DirectXDevice* dev, std::vector<std::string> images);


	
	ID3D11ShaderResourceView* GetTexture() { return textureView; };
	ID3D11ShaderResourceView* textureView;

	ID3D11ShaderResourceView* cubeGetTexture() { return this->srvCubeMap; }

	const char* filename;

};

#endif