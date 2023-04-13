#include "stdafx.h"
#include "CollisionDemo.h"

void CollisionDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(50, 2);

	shader = Debug_new Shader(L"32_Instance.fxo");

	floor = Debug_new Material(shader);
	floor->DiffuseMap(L"White.png");

	plane = Debug_new MeshRender(shader, Debug_new MeshPlane());
	plane->AddTransform();
	plane->UpdateTransforms();

	transform[0] = Debug_new Transform();
	transform[0]->SetPosition(0.0f, 0.5f, 0.0f);
	transform[0]->SetScale(2, 1, 1);
	collider[0] = Debug_new Collider(transform[0]);

	transform[1] = Debug_new Transform();
	transform[1]->SetPosition(2.0f, 0.5f, 0.0f);
	transform[1]->SetScale(2, 1, 1);
	collider[1] = Debug_new Collider(transform[1]);

}

void CollisionDemo::Update()
{
	plane->Update();

	Vector3 position2;
	transform[1]->GetPosition(&position2);

	if (_KEYCODE->Press(VK_RIGHT))
		position2.x += 3.0f * Time::Delta();
	else if (_KEYCODE->Press(VK_LEFT))
		position2.x -= 3.0f * Time::Delta();

	if (_KEYCODE->Press(VK_UP))
		position2.z += 3.0f * Time::Delta();
	else if (_KEYCODE->Press(VK_DOWN))
		position2.z -= 3.0f * Time::Delta();

	transform[1]->SetPosition(position2);


	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 rotation2 = Vector3(0, 0, 0);

	transform[0]->GetRotationDegree(&rotation);
	transform[1]->GetRotationDegree(&rotation2);

	ImGui::SliderFloat3("Rotation", (float *)rotation, -179, 179);
	ImGui::SliderFloat3("Rotation2", (float *)rotation2, -179, 179);

	transform[0]->SetRotationDegree(rotation);
	transform[1]->SetRotationDegree(rotation2);


	collider[0]->Update();
	collider[1]->Update();
}

void CollisionDemo::Render()
{
	floor->Render();
	plane->Render();

	bool b = collider[0]->Intersection(collider[1]);

	Color color = b ? Color(Math::RandomColor4()) : Color(0, 1, 0, 1);
	collider[0]->Render(color);
	collider[1]->Render(color);

}

