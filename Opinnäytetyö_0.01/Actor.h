#pragma once
#ifndef  ACTOR_H
#define ACTOR_H
#include "DirectXDevice.h"
#include "GeometryShader.h"
#include "Model.h"
#include "Camera.h"

struct ActorMatrix
{
	XMFLOAT4 position;
	XMFLOAT4 rotation;
	XMFLOAT4 scale;
};

struct Triangle
{
	XMVECTOR v1, v2, v3;
	XMVECTOR tNormal;
	XMVECTOR e0, e1, e2;
	XMVECTOR v0v1, v0v2, v0v3;

	Triangle()
	{

	}

	Triangle(XMVECTOR v11, XMVECTOR v12, XMVECTOR v13)
	{
		v1 = v11;
		v2 = v12;
		v3 = v13;
	}
	
};

class Actor
{
public:
	ActorMatrix aMatrix;

	float texOffset = 1.0f;




private:

	std::vector<Triangle> meshTriangles;
	DirectXDevice *devclass;

	XMMATRIX worldMatrix;

public:
	Actor();
	~Actor();
	Model *ModelMeshes;
	bool InitializeActor(DirectXDevice *dev, char* filepath);

	void SetActorPosition(XMFLOAT4 pos) { aMatrix.position = pos; }
	void SetActorScale(XMFLOAT4 size) { aMatrix.scale = size; }
	void SetActorRotation(XMFLOAT4 rot) { aMatrix.rotation = rot; }

	void SetTriangles();

	void DrawActor(GeometryShader* gshader);

	XMMATRIX GetActorWorldMatrix() { return worldMatrix; }

	const char* actorFile;

	bool RayHit(Camera &cam, XMFLOAT2 mpos, int width, int height, float &length);

	void Release();

private:

	void Triangulate();
	void PrepareActorMatrix();


};

#endif // ! ACTOR_H