#include "common.h"
#include "UserInterface.h"
#include "Renderer.h"

void TW_CALL LoadModel(void *data)
{

}


UserInterface::UserInterface()
{
}


UserInterface::~UserInterface()
{
}

void UserInterface::AddObjectToUI(Actor * a)
{

	std::string objPos = "Object position: ";
	std::string objRot = "Object rotation: ";
	std::string objSca = "Object Scale: ";
	std::string objind = "OBJ ";



	
	TwAddVarRO(objBar, objind.c_str(), TW_TYPE_CDSTRING, &a->actorFile, "");
	TwAddVarRO(objBar, objind.c_str(), TW_TYPE_CDSTRING, &objindex, "");
	TwAddVarRW(objBar, objPos.c_str(), TW_TYPE_DIR3F, &a->aMatrix.position, "");
	TwAddVarRW(objBar, objRot.c_str(), TW_TYPE_DIR3F, &a->aMatrix.rotation, "");
	TwAddVarRW(objBar, objSca.c_str(), TW_TYPE_DIR3F, &a->aMatrix.scale, "");
}

void UserInterface::AddLightToUI(XMFLOAT4 &dirlight, int &cook, int &phong, int &blinn, int &ward, int &lamb, int &orennayar)
{
	TwAddVarRW(lightBar, "Directional light", TW_TYPE_DIR3F, &dirlight, "");
	TwAddVarRW(lightBar, "Phong", TW_TYPE_BOOL32, &phong, "");
	TwAddVarRW(lightBar, "CookTorrance", TW_TYPE_BOOL32, &cook, "");
	TwAddVarRW(lightBar, "BlinnPhong", TW_TYPE_BOOL32, &blinn, "");
	TwAddVarRW(lightBar, "Ward", TW_TYPE_BOOL32, &ward, "");
	TwAddVarRW(lightBar, "Lambertian", TW_TYPE_BOOL32, &lamb, "");
	TwAddVarRW(lightBar, "Oren-Nayar", TW_TYPE_BOOL32, &orennayar, "");
}

void UserInterface::AddPointLight1(PointLight &p)
{
	TwAddVarRW(lightBar, "Pointlight Position", TW_TYPE_DIR3F, &p.PointlightPosition, "");
	TwAddVarRW(lightBar, "Pointlight Color", TW_TYPE_DIR3F, &p.PointlightColor, "");
}

void UserInterface::Release()
{
	TwTerminate();
}

bool UserInterface::InitInterface(DirectXDevice * dev, Window w)
{
	TwInit(TW_DIRECT3D11, dev->GetDevice());

	this->dev = dev;

	TwWindowSize(w._width, w._height);

	//ioBar = TwNewBar("Main");
	//TwDefine(" Main label = 'I/O' position = '600 600' size='100 50'");

	objBar = TwNewBar("Object");
	lightBar = TwNewBar("Scene");

	TwDefine(" Object label = 'Object' position = '1200 40' size='400 300'");
	TwDefine(" Scene label = 'Scene' position = '10 40' size='250 450'");

	TwAddButton(ioBar, "Load Model", LoadModel, this, "");

	return true;
}

void UserInterface::Update()
{
	TwDraw();
}
