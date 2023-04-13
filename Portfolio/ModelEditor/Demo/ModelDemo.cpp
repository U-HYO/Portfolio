#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Init()
{
	shader = new Shader(L"32_Instance.fxo");
	//Tank();
	Tower();
	Airplane();
}



void ModelDemo::Update()
{
	if (tank != NULL)
		tank->Update();

	if (tower != NULL)
		tower->Update();

	if (airplane != NULL)
		airplane->Update();

	if (miki != NULL)
		miki->Update();
}

void ModelDemo::Render()
{
	if (tank != NULL)
	{
		tank->Pass(1);
		tank->Render();
	}

	if (tower != NULL)
	{
		tower->Pass(1);
		tower->Render();
	}

	if (airplane != NULL)
	{
		airplane->Pass(1);
		airplane->Render();
	}

	if (miki != NULL)
	{
		miki->Pass(1);
		miki->Render();
	}
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
	tank->ReadMaterial(L"Tank/Tank");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tank->AddTransform();
		tank->SetColor(tank->GetTransforms().size() - 1, Math::RandomColor3());
		transform->SetPosition(Vector3(x, 0.0f, 5.0f));
		transform->SetRotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->SetScale(0.1f, 0.1f, 0.1f);
	}
	tank->UpdateTransforms();
}

void ModelDemo::Tower()
{
	tower = new ModelRender(shader);
	tower->ReadMesh(L"Tower/Tower");
	tower->ReadMaterial(L"Tower/Tower");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tower->AddTransform();

		transform->SetPosition(Vector3(x, 0.0f, 7.5f));
		transform->SetRotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->SetScale(0.003f, 0.003f, 0.003f);
	}
	tower->UpdateTransforms();
}

void ModelDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = airplane->AddTransform();

		transform->SetPosition(Vector3(x, 0.0f, 2.5f));
		transform->SetRotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->SetScale(0.00025f, 0.00025f, 0.00025f);
	}
	airplane->UpdateTransforms();
}

