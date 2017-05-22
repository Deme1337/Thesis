#include "common.h"


#include "Texture.h"

#include "Ext\FreeImage\Dist\x64\FreeImage.h"


#pragma comment(lib, "FreeImage.lib")
#include <iostream>


CTexture::CTexture()
{
	texturedata = 0;
	texture = 0;
	textureView = 0;
}

CTexture::CTexture(const CTexture &)
{
}


CTexture::~CTexture()
{
}

void CTexture::Release()
{
	SafeRelease(texture);
	SafeRelease(srvCubeMap);
	
	for (size_t i = 0; i < 6; i++)
	{
		SafeRelease(srcTex[i]);
	}
}

bool CTexture::LoadFreeImage(DirectXDevice * dev, const char * filepath)
{
	int height, width;
	
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	filename = filepath;

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);


	PRINTL(filepath);

	FreeImage_FlipVertical(dib);

	fif = FreeImage_GetFileType(filepath, 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filepath);

	if (fif == FIF_UNKNOWN)
	{
		PRINT(" FIF UNKNOWN");
	}

	// Check if the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filepath);
	if (!dib)
	{
		PRINT(" DIB FAILED IN : ");
		PRINT(filepath);
		return false;
	}
		

	FIBITMAP* temp = dib;
	dib = FreeImage_ConvertTo32Bits(dib);
	FreeImage_Unload(temp);

	height = FreeImage_GetHeight(dib);
	width = FreeImage_GetWidth(dib);
	texturedata = FreeImage_GetBits(dib);
	int bPP = FreeImage_GetBPP(dib);



	// Setup the description of the CTexture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	const char *format = FreeImage_GetFormatFromFIF(fif);
	if (format == "BMP" || format == "TGA")
	{
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else
	{

		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	}

	if (textureDesc.Format != 0)
	{
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		rowPitch = FreeImage_GetPitch(dib);

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = texturedata;
		initData.SysMemPitch = width * 4;
		initData.SysMemSlicePitch = width * height * 4;

		// Create the empty CTexture.
		hResult = dev->GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
		if (FAILED(hResult))
		{
			PRINT(" Texture creation failed");
			return false;
		}


		// Copy the targa image data into the CTexture.
		dev->GetDeviceContext()->UpdateSubresource(texture, 0, NULL, texturedata, rowPitch, 0);


		memset(&srvDesc, 0, sizeof(srvDesc));
		// Setup the shader resource view description.
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create the shader resource view for the CTexture.
		hResult = dev->GetDevice()->CreateShaderResourceView(texture, &srvDesc, &textureView);
		if (FAILED(hResult))
		{
			PRINT(" CAnnot create shrv");
			return false;
		}


		// Generate mipmaps for this CTexture.
		dev->GetDeviceContext()->GenerateMips(textureView);

		FreeImage_Unload(dib);
		return true;
	}
	else
	{
		FreeImage_Unload(dib);
		PRINT("Error loading image");
		return false;
	}
}

bool CTexture::LoadCubeMap(DirectXDevice * dev, std::vector<std::string> images)
{
	CTexture* tex[6];
	for (size_t i = 0; i < 6; i++)
	{
		tex[i] = new CTexture();
		if (!tex[i]->LoadFreeImage(dev, images[i].c_str()))
		{
			MessageBox(NULL, L"Texture path incorrect", L"ERROR", MB_OK);
			PRINTL("Cannot load cubemap image ");
			return false;
		}
		srcTex[i] = tex[i]->texture;

	}



	D3D11_TEXTURE2D_DESC texElementDesc;
	((ID3D11Texture2D*)srcTex[0])->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = 6;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* texArray = 0;
	if (FAILED(dev->GetDevice()->CreateTexture2D(&texArrayDesc, 0, &texArray)))
		return false;

	// Copy individual texture elements into texture array.
	D3D11_BOX sourceRegion;

	//Here i copy the mip map levels of the textures
	for (UINT x = 0; x < 6; x++)
	{
		for (UINT mipLevel = 0; mipLevel < texArrayDesc.MipLevels; mipLevel++)
		{
			sourceRegion.left = 0;
			sourceRegion.right = (texArrayDesc.Width >> mipLevel);
			sourceRegion.top = 0;
			sourceRegion.bottom = (texArrayDesc.Height >> mipLevel);
			sourceRegion.front = 0;
			sourceRegion.back = 1;

			//test for overflow
			if (sourceRegion.bottom == 0 || sourceRegion.right == 0)
				break;

			dev->GetDeviceContext()->CopySubresourceRegion(texArray, D3D11CalcSubresource(mipLevel, x, texArrayDesc.MipLevels), 0, 0, 0, srcTex[x], mipLevel, &sourceRegion);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MostDetailedMip = 0;
	viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

	if (FAILED(dev->GetDevice()->CreateShaderResourceView(texArray, &viewDesc, &srvCubeMap)))
		return false;


	for (int i = 0; i < 6; i++)
	{
		tex[i]->Release();
		delete tex[i];
	}


	return true;
}
