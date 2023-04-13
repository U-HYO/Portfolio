#include "Framework.h"
#include "Context.h"
#include "Viewer/Viewport.h"
#include "Viewer/Perspective.h"


Context* Context::instance = NULL;

Context * Context::Get()
{
	//assert(instance != NULL);

	return instance;
}

void Context::Create()
{
	assert(instance == NULL);

	instance = Debug_new Context();
}

void Context::Delete()
{
	SAFE_DELETE(instance);
}

Context::Context()
{
	D3DDesc desc = D3D::GetDesc();

	perspective = Debug_new Perspective(desc.Width, desc.Height);
	viewport = Debug_new Viewport(desc.Width, desc.Height);
	class Camera* camera = Debug_new FreeCamera();
	class Camera* fallowCamera = Debug_new FallowCamera();
	cameras.emplace_back(camera);
	cameras.emplace_back(fallowCamera);
	currentCameraIndex = 0;

	ZeroMemory(culling, sizeof(Plane) * 4);
	clipping = Plane(0, 0, 0, 0);
}

Context::~Context()
{
	//SAFE_DELETE(camera);
	for (class Camera* camera : cameras)
		SAFE_DELETE(camera);
	SAFE_DELETE(perspective);
	SAFE_DELETE(viewport);
}

void Context::ResizeScreen()
{
	perspective->Set(D3D::Width(), D3D::Height());
	viewport->Set(D3D::Width(), D3D::Height());
}

void Context::Update()
{
	for (class Camera* camera : cameras)
		camera->Update();
}

void Context::Render()
{
	viewport->RSSetViewport();
	clipping = Plane(0, 0, 0, 0);

	//std::string str = "";

	//str = std::string("FrameRate : ") + std::to_string(ImGui::GetIO().Framerate);
	////str = std::string("FrameRate : ") + std::to_string(Time::Get()->FPS());
	//Gui::Get()->RenderText(5, 5, 1, 1, 1, str);

	//Vector3 R;
	//cameras[currentCameraIndex]->GetTransform()->GetRotationDegree(&R);

	//Vector3 P;
	//P = cameras[currentCameraIndex]->GetTransform()->GetWorldPosition();

	//str = "Camera Rotation : ";
	//str += std::to_string((int)R.x) + ", " + std::to_string((int)R.y) + ", " + std::to_string((int)R.y);
	//Gui::Get()->RenderText(5, 20, 1, 1, 1, str);

	//str = "Camera Position : ";
	//str += std::to_string((int)P.x) + ", " + std::to_string((int)P.y) + ", " + std::to_string((int)P.y);
	//Gui::Get()->RenderText(5, 35, 1, 1, 1, str);

	//UINT i;
	//i = currentCameraIndex;
	//str = "Camrea Index : ";
	//str += std::to_string(i);
	//Gui::Get()->RenderText(5, 50, 1, 1, 1, str);

}

Matrix Context::View()
{
	Matrix view;
	cameras[currentCameraIndex]->GetMatrix(&view);

	return view;
}

Matrix Context::Projection()
{
	Matrix projection;
	perspective->GetMatrix(&projection);

	return projection;
}
