#include "Actor.h"



Actor::Actor()
{
	ModelMeshes = 0;
}


Actor::~Actor()
{
}

bool Actor::InitializeActor(DirectXDevice * dev, char * filepath)
{
	devclass = dev;
	ModelMeshes = new Model(filepath, dev);

	actorFile = filepath;

	Triangulate();

	PrepareActorMatrix();



	return true;
}

void Actor::SetTriangles()
{

	XMVECTOR pos, rot, siz;
	pos = XMLoadFloat4(&aMatrix.position);
	rot = XMLoadFloat4(&aMatrix.rotation);
	siz = XMLoadFloat4(&aMatrix.scale);
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(pos);

	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYawFromVector(rot);
	XMMATRIX ScaleMatrix = XMMatrixScalingFromVector(siz);

	XMMATRIX scaler = RotationMatrix  * ScaleMatrix * TranslationMatrix;
	
	for (size_t i = 0; i < meshTriangles.size(); i++)
	{
		XMVector3Transform(meshTriangles[i].v1, scaler);
		XMVector3Transform(meshTriangles[i].v2, scaler);
		XMVector3Transform(meshTriangles[i].v3, scaler);
	
	}
}

void Actor::DrawActor(GeometryShader * gshader)
{
	PrepareActorMatrix();

	for (size_t i = 0; i < ModelMeshes->meshes.size(); i++)
	{
		
		gshader->SetObjectData(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), ModelMeshes->meshes[i].mprop.kd, ModelMeshes->meshes[i].mprop.ks);
		ModelMeshes->meshes[i].DrawMeshGeometry(gshader);
		gshader->SetObjectData(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
}

bool Actor::RayHit(Camera &cam, XMFLOAT2 mpos, int width, int height, float &raylength)
{

	XMVECTOR P0 = cam.GetCameraPosition();
	float scale = std::tan(devclass->fov * 0.5);
	float aspectRatio = devclass->aspect;

	float x = (2 * (mpos.x + 0.5) / (float)width - 1) * aspectRatio * scale;
	float y = (1 - 2 * (mpos.y + 0.5) / (float)height) * scale;

	XMVECTOR dir = XMVectorAdd(XMVectorSet(x, y, -1, 1.0f), cam.LookAt);

	dir = XMVector3Normalize(dir);

	float kEpsilon = 1e-8;


	for (size_t i = 0; i < meshTriangles.size(); i++)
	{

		XMVECTOR v0v2 = XMVectorSubtract(meshTriangles[i].v3, meshTriangles[i].v1);
		XMVECTOR v0v1 = XMVectorSubtract(meshTriangles[i].v2, meshTriangles[i].v1);
		XMVECTOR v0v3 = XMVectorSubtract(meshTriangles[i].v3, meshTriangles[i].v2);

		XMVECTOR tNormal = XMVector3Normalize(XMVector3Cross(v0v1, v0v2));

		XMVECTOR pvec = XMVector3Cross(dir, v0v2);
		XMFLOAT3 xdet;  XMStoreFloat3(&xdet, XMVector3Dot(v0v1, pvec));



		if (xdet.x > kEpsilon)
		{
			i++;
		}

		float invDet = 1 / xdet.x;

		XMVECTOR tvec = XMVectorSubtract(P0, meshTriangles[i].v1);

		XMFLOAT3 tvecpvecDot;  XMStoreFloat3(&tvecpvecDot, XMVector3Dot(tvec, pvec));

		float u = tvecpvecDot.x * invDet;
		if (u < 0 || u > 1)
		{
			i++;
		}

		XMVECTOR qvec = XMVector3Cross(tvec, v0v1);

		XMFLOAT3 dqvecDot;  XMStoreFloat3(&dqvecDot, XMVector3Dot(dir, qvec));

		float v = dqvecDot.x * invDet;

		if (v < 0 || u + v > 1)
		{
			i++;
		}
		

		XMFLOAT3 rayL; XMStoreFloat3(&rayL, XMVector3Length(XMVectorSubtract(P0, pvec)));
		PRINTL("Ray length: " + std::string(std::to_string(rayL.x)));
		raylength = rayL.x;
		return true;
		
	}

}

void Actor::Release()
{
	ModelMeshes->Release(); 
}

void Actor::Triangulate()
{

	for (size_t i = 0; i < ModelMeshes->meshes.size(); i++)
	{
		for (size_t j = 0;  j < ModelMeshes->meshes[i].vertices.size();  j += 3)
		{
			Triangle t1 = Triangle();
			XMVECTOR v1 = XMLoadFloat3(&ModelMeshes->meshes[i].vertices[j].Position);
			XMVECTOR v2 = XMLoadFloat3(&ModelMeshes->meshes[i].vertices[j + 1].Position);
			XMVECTOR v3 = XMLoadFloat3(&ModelMeshes->meshes[i].vertices[j + 2].Position);

			t1.v1 = v1;
			t1.v2 = v2;
			t1.v3 = v3;

			XMVECTOR v0v2 = XMVectorSubtract(v3, v1);
			XMVECTOR v0v1 = XMVectorSubtract(v2, v1);
			XMVECTOR v0v3 = XMVectorSubtract(v3, v2);

			t1.v0v1 = v0v1;
			t1.v0v2 = v0v2;
			t1.v0v3 = v0v3;

			t1.tNormal = XMVector3Normalize(XMVector3Cross(v0v1, v0v2));

			meshTriangles.push_back(t1);
		}
	}
	PRINTL("Model: " + std::string(actorFile) + " Triangle count: " + std::string(std::to_string(meshTriangles.size())))


}

void Actor::PrepareActorMatrix()
{
	worldMatrix = XMMatrixIdentity();
	XMMATRIX ScaleMatrix = XMMatrixIdentity();
	XMMATRIX TranslationMatrix = XMMatrixIdentity();
	XMMATRIX RotationMatrix = XMMatrixIdentity();

	XMVECTOR pos, rot, siz;
	pos = XMLoadFloat4(&aMatrix.position);
	rot = XMLoadFloat4(&aMatrix.rotation);
	siz = XMLoadFloat4(&aMatrix.scale);


	TranslationMatrix = XMMatrixTranslationFromVector(pos);

	RotationMatrix = XMMatrixRotationRollPitchYawFromVector(rot);
	ScaleMatrix = XMMatrixScalingFromVector(siz);


	worldMatrix = RotationMatrix  * ScaleMatrix *TranslationMatrix;


}
