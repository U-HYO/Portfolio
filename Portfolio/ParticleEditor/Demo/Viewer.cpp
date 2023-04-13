#include "stdafx.h"
#include "Viewer.h"

void Viewer::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(40, 2);

	//particle = Debug_new Particle(L"Fire");
	particle = Debug_new Particle(L"Star");

	shader = Debug_new Shader(L"38_AreaLighting.fxo");
	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);

	Mesh();
}

void Viewer::Update()
{
	sky->Update();

	grid->Update();
	sphere->Update();

	Vector3 position;
	sphere->GetTransform(0)->GetPosition(&position);

	if (Keyboard::Get()->Press('L'))
		position.x += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('J'))
		position.x -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('I'))
		position.z += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('O'))
		position.y += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('U'))
		position.y -= 20 * Time::Delta();

	sphere->GetTransform(0)->SetPosition(position);
	sphere->UpdateTransforms();

	Vector3 temp = Vector3
	(
		sphere->GetTransform(0)->GetWorld()._41,
		sphere->GetTransform(0)->GetWorld()._42,
		sphere->GetTransform(0)->GetWorld()._43
	);

	particle->Add(temp);
	particle->Add(position);

	
	particle->Update();


}

void Viewer::Render()
{
	sky->Render();

	stone->Render();
	floor->Render();

	grid->Render();
	sphere->Render();

	particle->Render();

}

void Viewer::Mesh()
{
	//Create Material
	{
		floor = Debug_new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->NormalMap("Floor_Normal.png");
		floor->Specular(1, 1, 1, 20);
		floor->SpecularMap("Floor_Specular.png");

		stone = Debug_new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->NormalMap("Stones_Normal.png");
		stone->Specular(1, 1, 1, 20);
		stone->SpecularMap("Stones_Specular.png");
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}


	//Create Mesh
	{
		Transform* transform = NULL;

		grid = Debug_new MeshRender(shader, Debug_new MeshPlane(5, 5));
		transform = grid->AddTransform();
		transform->SetPosition(0, 0, 0);
		transform->SetScale(12, 1, 12);

		sphere = Debug_new MeshRender(shader, Debug_new MeshSphere(0.5f, 20, 20));
		transform = sphere->AddTransform();
		transform->SetPosition(0, 5, 0);
		transform->SetScale(5, 5, 5);
	}

	sphere->UpdateTransforms();
	grid->UpdateTransforms();
}