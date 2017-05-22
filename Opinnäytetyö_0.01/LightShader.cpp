#include "common.h"
#include "LightShader.h"



LightShader::LightShader()
{
}


LightShader::~LightShader()
{
}

bool LightShader::InitializeLightShader(DirectXDevice * dev, WCHAR * vshader, WCHAR * pshader)
{
	devclass = dev;

	HRESULT result;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC pointLightBufferDesc;
	D3D11_BUFFER_DESC settingsBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	vertexShaderBuffer = DirectXDevice::CompileShader(L"LightVertexShader.hlsl", "vertexshader", "LightVertexShader");
	pixelShaderBuffer = DirectXDevice::CompileShader(L"LightPixelShader.hlsl", "pixelshader", "LightPixelShader");

	//Vertex varjostimen luonti
	result = devclass->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Pikselivarjostimen luonti
	result = devclass->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = devclass->GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = devclass->GetDevice()->CreateSamplerState(&samplerDesc, &sampleState);
	if (FAILED(result))
	{
		return false;
	}



	// Create a clamp texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Create the texture sampler state.
	result = devclass->GetDevice()->CreateSamplerState(&samplerDesc, &sampleClamp);
	if (FAILED(result))
	{
		return false;
	}


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = devclass->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = devclass->GetDevice()->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(result))
	{
		return false;
	}


	//point light buffer desc
	pointLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointLightBufferDesc.ByteWidth = sizeof(PointLightBuffer);
	pointLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointLightBufferDesc.MiscFlags = 0;
	pointLightBufferDesc.StructureByteStride = 0;
	
	result = devclass->GetDevice()->CreateBuffer(&pointLightBufferDesc, NULL, &pointLightBuffer);
	if (FAILED(result))
	{
		return false;
	}


	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBuffer);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = devclass->GetDevice()->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
	if (FAILED(result))
	{
		PRINTL("Failed camerabuffer");
		return false;
	}




	settingsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	settingsBufferDesc.ByteWidth = sizeof(SettingsBuffer);
	settingsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	settingsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	settingsBufferDesc.MiscFlags = 0;
	settingsBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = devclass->GetDevice()->CreateBuffer(&settingsBufferDesc, NULL, &settingsBuffer);
	if (FAILED(result))
	{
		PRINTL("Failed settingsbuffer");
		return false;
	}



	return true;
}

void LightShader::UpdateTexture(ID3D11ShaderResourceView * tex, int index)
{
	devclass->GetDeviceContext()->PSSetShaderResources(index, 1, &tex);
}


void LightShader::UpdateSettings(bool cook, bool phong, bool blinn, bool na, int gbuffer, bool onayar, bool lambert)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	SettingsBuffer* dataPtr;

	result = devclass->GetDeviceContext()->Map(settingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);


	dataPtr = (SettingsBuffer*)mappedResource.pData;
	dataPtr->BLinnPhongs = blinn;
	dataPtr->Phongs = phong;
	dataPtr->CookTorrances = cook;
	dataPtr->Wards = na;
	dataPtr->ShowGBuffer = gbuffer;
	dataPtr->Lambertian = lambert;
	dataPtr->ONDiffuse = onayar;
	dataPtr->vvv = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	devclass->GetDeviceContext()->Unmap(settingsBuffer, 0);
	devclass->GetDeviceContext()->PSSetConstantBuffers(2, 1, &settingsBuffer);
}


void LightShader::UpdateShaderParameters(XMMATRIX & worldMatrix, XMMATRIX & viewMatrix, XMMATRIX & projectionMatrix, 
	ID3D11ShaderResourceView * colorTexture, ID3D11ShaderResourceView * normalTexture,
	ID3D11ShaderResourceView* specularTexture, ID3D11ShaderResourceView* positionTexture,
	ID3D11ShaderResourceView* roughnessTexture, ID3D11ShaderResourceView* tangentTexture,
	ID3D11ShaderResourceView* binormalTexture, XMFLOAT4 &LightPosition, std::vector<PointLight> plights)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBuffer* dataPtr;
	LightBuffer* dataPtr2;
	PointLightBuffer* dataPtr3;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = devclass->GetDeviceContext()->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	devclass->GetDeviceContext()->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	devclass->GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);



	// Set shader texture resources in the pixel shader.
	devclass->GetDeviceContext()->PSSetShaderResources(0, 1, &colorTexture);
	devclass->GetDeviceContext()->PSSetShaderResources(1, 1, &normalTexture);
	devclass->GetDeviceContext()->PSSetShaderResources(2, 1, &specularTexture);
	devclass->GetDeviceContext()->PSSetShaderResources(3, 1, &positionTexture);
	devclass->GetDeviceContext()->PSSetShaderResources(4, 1, &roughnessTexture);
	devclass->GetDeviceContext()->PSSetShaderResources(5, 1, &tangentTexture);
	devclass->GetDeviceContext()->PSSetShaderResources(6, 1, &binormalTexture);

	// Lock the light constant buffer so it can be written to.
	result = devclass->GetDeviceContext()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBuffer*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	XMVECTOR pos = XMLoadFloat4(&LightPosition);
	XMVECTOR col = XMVectorSet(1.0f,1.0f,1.0f,1.0);
	dataPtr2->LightPosition = -XMVector4Normalize(pos);
	dataPtr2->LightColor = col;
	//dataPtr2->GlobalAmbient = 0.2f;

	// Unlock the constant buffer.
	devclass->GetDeviceContext()->Unmap(lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	devclass->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);



	//Pointlights
	result = devclass->GetDeviceContext()->Map(pointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (PointLightBuffer*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	for (size_t i = 0; i < POINT_LIGHTS; i++)
	{
		dataPtr3->PointLightColor[i] = XMLoadFloat4(&plights[i].PointlightColor);
		dataPtr3->PointLightPosition[i] = XMLoadFloat4(&plights[i].PointlightPosition);
	}

	// Unlock the constant buffer.
	devclass->GetDeviceContext()->Unmap(pointLightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	devclass->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &pointLightBuffer);

}

void LightShader::UpdateCameraMatrix(XMVECTOR cameraposition)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CameraBuffer* dataPtr;

	result = devclass->GetDeviceContext()->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);


	dataPtr = (CameraBuffer*)mappedResource.pData;
	dataPtr->CameraPosition = cameraposition;

	devclass->GetDeviceContext()->Unmap(cameraBuffer, 0);
	devclass->GetDeviceContext()->VSSetConstantBuffers(1, 1, &cameraBuffer);
}

void LightShader::UseShader(int icount)
{
	// Set the vertex input layout.
	devclass->GetDeviceContext()->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render.
	devclass->GetDeviceContext()->VSSetShader(vertexShader, NULL, 0);
	devclass->GetDeviceContext()->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	devclass->GetDeviceContext()->PSSetSamplers(0, 1, &sampleState);
	devclass->GetDeviceContext()->PSSetSamplers(1, 1, &sampleClamp);

	// Render the geometry.
	devclass->GetDeviceContext()->DrawIndexed(icount, 0, 0);
}

void LightShader::Release()
{
	SafeRelease(cameraBuffer);
	SafeRelease(vertexShader);
	SafeRelease(pixelShader);
	SafeRelease(layout);
	SafeRelease(disneyBuffer);
	SafeRelease(lightBuffer);
	SafeRelease(sampleClamp);
	SafeRelease(sampleState);
}
