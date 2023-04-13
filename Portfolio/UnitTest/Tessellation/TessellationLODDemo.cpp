#include "stdafx.h"
#include "TessellationLODDemo.h"

void TessellationLODDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(0, 0, -100);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(500, 2);
	

	//Perspective* perspective = Context::Get()->GetPerspective();
	//perspective->Set(D3D::Width(), D3D::Height(), 0.1f, 1000.0f, Math::PI * 0.3f);

	shader = Debug_new Shader(L"57_TerrainLOD.fxo");
	terrain = Debug_new TerrainLOD(L"../../_Textures/Terrain/Gray2048.dds");
	//terrain->BaseMap(L"Terrain/Dirt3.png");
	

}

void TessellationLODDemo::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(terrain);
}

void TessellationLODDemo::Update()
{
	terrain->Update();
	
}


void TessellationLODDemo::Render()
{
	terrain->Render();

}