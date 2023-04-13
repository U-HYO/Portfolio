#include "stdafx.h"
#include "MeshDemo.h"
#include "Converter.h"

void MeshDemo::Init()
{
	shader = new Shader(L"32_Instance.fxo");

	planeMaterial = new Material(shader);
	planeMaterial->DiffuseMap("White.png");

	plane = new MeshRender(shader, new MeshPlane());
	plane->AddTransform()->SetScale(10, 1, 2);
	plane->UpdateTransforms(); // update해야 gpu로 값을 넣어줌

	boxMaterial = new Material(shader);
	boxMaterial->DiffuseMap(L"Box.png");

	box = new MeshRender(shader, new MeshCube());
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = box->AddTransform();
		transform->SetScale(0.25f, 0.25f, 0.25f);
		transform->SetPosition(Vector3(x - 1.0f, 0.25f * 0.5f, 0.0f));
		transform->SetRotationDegree(0, Math::Random(-(float)D3DX_PI, (float)D3DX_PI), 0);
	}
	box->UpdateTransforms();
}



void MeshDemo::Update()
{
	plane->Update();
	box->Update();
}

void MeshDemo::Render()
{
	planeMaterial->Render();
	plane->Render();

	boxMaterial->Render();
	box->Render();


}
