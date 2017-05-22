#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <time.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32> indices, std::vector<Texture> textures, DirectXDevice* devc, MeshProperties mp)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->DevClass = devc;
	this->SetupMesh();
	mprop = mp;

	UseMeshMaterials = true;
}
Mesh::Mesh()
{

}


void Mesh::DrawMeshGeometry(GeometryShader* gshader)
{
	unsigned int stride;
	unsigned int offset;

	if (UseMeshMaterials)
	{
		for (uint32 i = 0; i < this->textures.size(); i++)
		{

			// Retrieve texture number (the N in diffuse_textureN)
			std::stringstream ss;
			std::string number;
			std::string name = this->textures[i].type;

			if (name == "texture_diffuse")
			{
				gshader->UpdateTexture(textures[i].tex->textureView, 0);
			}

			if (name == "texture_specular")
			{
				gshader->UpdateTexture(textures[i].tex->textureView, 1);
			}

			if (name == "texture_bump")
			{
				gshader->UpdateTexture(textures[i].tex->textureView, 2);
			}


			if (name == "texture_rough")
			{
				gshader->UpdateTexture(textures[i].tex->textureView, 3);
			}

		}
	}

	

	
	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	DevClass->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	DevClass->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	DevClass->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	gshader->UseShader(indices.size());

	if (UseMeshMaterials)
	{
		//cleanup textures
		gshader->UpdateTexture(nullptr, 0);
		gshader->UpdateTexture(nullptr, 1);
		gshader->UpdateTexture(nullptr, 3);
		gshader->UpdateTexture(nullptr, 4);
	}
	

	

}



Mesh::~Mesh()
{
}
/*
void Mesh::DrawIndicesToShader(CTextureRenderShader *shader)
{
unsigned int stride;
unsigned int offset;


// Set vertex buffer stride and offset.
stride = sizeof(Vertex);
offset = 0;

// Set the vertex buffer to active in the input assembler so it can be rendered.
//Dx11Call(deviceContext, &m_vertexBuffer, &stride, &offset); // Testing stuff
DevClass->GetDevCon()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

// Set the index buffer to active in the input assembler so it can be rendered.
DevClass->GetDevCon()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
DevClass->GetDevCon()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

shader->RenderShader(DevClass->GetDevCon(), indices.size());

}

void Mesh::DrawShadow(CDepthShader *shader)
{

	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	//Dx11Call(deviceContext, &m_vertexBuffer, &stride, &offset); // Testing stuff
	DevClass->GetDevCon()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	DevClass->GetDevCon()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	DevClass->GetDevCon()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->RenderShader(DevClass->GetDevCon(), indices.size());

}
*/
void Mesh::Release()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void Mesh::SetupMesh()
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) *  this->vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0; vertexBufferDesc.Usage;

	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = DevClass->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32)* this->indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = DevClass->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return;
	}

}




