#pragma once
#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "Ext\Assimp\include\assimp\Importer.hpp"
#include "Ext\Assimp\include\assimp\scene.h"
#include "Ext\Assimp\include\assimp\postprocess.h"

#include "Texture.h"
#include "DirectXDevice.h"
#include "GeometryShader.h"
#include <vector>



struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoords;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT3 BiTangent;
};

struct Texture
{
	CTexture *tex;
	std::string type;
	aiString path;



};

struct MeshProperties
{
	XMFLOAT3 ka = XMFLOAT3(0.2f, 0.2f, 0.2f);
	XMFLOAT3 kd = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3  ks = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float metallic = 0.03f;
	float Roughness = 1.0f;
};


class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<uint32> indices, std::vector<Texture> textures, DirectXDevice* devc, MeshProperties mprop);
	~Mesh();
	Mesh();
	std::vector<Vertex> vertices;
	std::vector<uint32> indices;
	std::vector<Texture> textures;

	MeshProperties mprop;

	uint32 GetIndices() { return this->indices.size(); }

	void Mesh::DrawMeshGeometry(GeometryShader* gshader);


	void Release();
	bool UseMeshMaterials = true;
private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	DirectXDevice* DevClass;
	void SetupMesh();

};

#endif