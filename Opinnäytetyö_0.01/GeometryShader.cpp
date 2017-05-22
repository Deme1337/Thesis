#include "common.h"
#include "GeometryShader.h"



GeometryShader::GeometryShader()
{
	vertexShader = 0;
	pixelShader = 0;
}


GeometryShader::~GeometryShader()
{
}

bool GeometryShader::InitShader(WCHAR * vshader, WCHAR * pshader, DirectXDevice * dev)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC objDataDesc;

	devclass = dev;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	vertexShaderBuffer = DirectXDevice::CompileShader(L"GVertexShader.hlsl", "vertexshader", "GeometryVertexShader");
	pixelShaderBuffer = DirectXDevice::CompileShader(L"GPixelShader.hlsl", "pixelshader", "GeometryPixelShader");

	// Create the vertex shader from the buffer.
	result = devclass->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

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

	// Create a wrap texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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
	result = devclass->GetDevice()->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
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

	//Set obj data desc
	objDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	objDataDesc.ByteWidth = sizeof(ObjectData) * 4;
	objDataDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objDataDesc.MiscFlags = 0;
	objDataDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = devclass->GetDevice()->CreateBuffer(&objDataDesc, NULL, &ObjDataBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void GeometryShader::UpdateTexture(ID3D11ShaderResourceView * texture, int texindex)
{
	//Annetaan textuuri shaderille
	devclass->GetDeviceContext()->PSSetShaderResources(texindex, 1, &texture);
}



void GeometryShader::SetObjectData(XMFLOAT4 data, XMFLOAT3 objcolor, XMFLOAT3 objcolorspec)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	ObjectData* dataPtr;



	result = devclass->GetDeviceContext()->Map(ObjDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	dataPtr = (ObjectData*)mappedResource.pData;

	dataPtr->useTextures = data.x;
	dataPtr->roughnessOffset = data.y;
	dataPtr->metallic = data.z;
	dataPtr->objColor = XMFLOAT4(objcolor.x, objcolor.y, objcolor.z, 1.0f);
	dataPtr->objColorSpec = XMFLOAT4(objcolorspec.x, objcolorspec.y, objcolorspec.z, 1.0f);

	devclass->GetDeviceContext()->Unmap(ObjDataBuffer, 0);

	bufferNumber = 0;


	devclass->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &ObjDataBuffer);

	dataPtr = 0;
	delete dataPtr;
}

void GeometryShader::UpdateShader(XMMATRIX & world, XMMATRIX & view, XMMATRIX & projection, bool HasAlpha, float texoffset)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;


	// Transpose the matrices to prepare them for the shader.
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	result = devclass->GetDeviceContext()->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;
	dataPtr->hasAlpha = HasAlpha;
	dataPtr->texOffSet = texoffset;
	//dataPtr->projection = projection;

	// Unlock the constant buffer.
	devclass->GetDeviceContext()->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	devclass->GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);
}

void GeometryShader::UseShader(int icount)
{
	// Set the vertex input layout.
	devclass->GetDeviceContext()->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render.
	devclass->GetDeviceContext()->VSSetShader(vertexShader, NULL, 0);
	devclass->GetDeviceContext()->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler states in the pixel shader.
	devclass->GetDeviceContext()->PSSetSamplers(0, 1, &sampleStateWrap);

	// Render the geometry.
	devclass->GetDeviceContext()->DrawIndexed(icount, 0, 0);
}

void GeometryShader::Release()
{
	SafeRelease(ObjDataBuffer);
	SafeRelease(matrixBuffer);
	SafeRelease(sampleStateWrap);
	SafeRelease(layout);
	SafeRelease(pixelShader);
	SafeRelease(vertexShader);
}
