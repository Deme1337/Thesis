#pragma once


#ifndef MODEL_H
#define MODEL_H
#include "DirectXDevice.h"

#include "Ext\Assimp\include\assimp\Importer.hpp"
#include "Ext\Assimp\include\assimp\scene.h"
#include "Ext\Assimp\include\assimp\postprocess.h"

#include "Mesh.h"
#include "Texture.h"
#include <map>
#include <thread>
#include <future>

class Model
{
public:
	Model(const char* path, DirectXDevice *DevClass);


	//for debugging
	const char* path_;

	~Model();

	void Release();
	std::vector<Mesh> meshes;

private:




	//Converts aiVector to XMVECTOR
	XMFLOAT3 XMSubstract(XMFLOAT3 v1, XMFLOAT3 v2);
	XMFLOAT2 XMSubstract(XMFLOAT2 v1, XMFLOAT2 v2);
	XMFLOAT3 aiToXM(aiVector3D v);
	XMVECTOR CalculateSurfaceNormal(XMVECTOR p1, XMVECTOR p2, XMVECTOR p3);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	CTexture *LoadTexturesForAssimp(const char* pathforfile, std::string directorys);
	/*  Model Data  */
	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	std::string directory;
	std::vector<Texture> textures_loaded;
	void loadModel(std::string path);


	DirectXDevice* DevClass;

public:


};

#endif