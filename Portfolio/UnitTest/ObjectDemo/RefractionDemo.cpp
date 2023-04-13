#include "stdafx.h"
#include "RefractionDemo.h"

void RefractionDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(21, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(126, 35, 39);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(100, 2);
	
	shader = Debug_new Shader(L"60_Refraction.fxo");
	shadow = Debug_new Shadow(shader, Vector3(128, 0, 128), 65);


	sky = Debug_new Sky(shader);
	sky->RealTime(false, Math::PI - 1e-6f);

	terrain = Debug_new Terrain(shader, L"Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");


	Mesh();
	Airplane();

	Paladin();
	Weapon();


	PointLights();
	SpotLights();

	refraction = Debug_new Refraction(shader, mirrorPlane->GetTransform(0), L"Environment/Wave.dds");

}

void RefractionDemo::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(shadow);
	SAFE_DELETE(refraction);

	SAFE_DELETE(sky);
	SAFE_DELETE(terrain);

	SAFE_DELETE(floor);
	SAFE_DELETE(stone);
	SAFE_DELETE(brick);
	SAFE_DELETE(wall);

	SAFE_DELETE(cube);
	SAFE_DELETE(cylinder);
	SAFE_DELETE(sphere);
	SAFE_DELETE(mirrorPlane);

	SAFE_DELETE(airplane);

	SAFE_DELETE(paladin);
	SAFE_DELETE(weapon);
	SAFE_DELETE(weaponTransform);
}

void RefractionDemo::Update()
{
	sky->Update();

	cube->Update();
	cylinder->Update();
	sphere->Update();

	airplane->Update();
	paladin->Update();


	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < paladin->GetTransformsCount(); i++)
	{
		paladin->GetAttachBones(i, worlds);
		weapon->GetTransform(i)->SetWorld(weaponTransform->GetWorld() * worlds[40]);
	}
	weapon->UpdateTransforms();
	weapon->Update();

	terrain->Update();
	refraction->Update();
	mirrorPlane->Update();
}

void RefractionDemo::PreRender()
{
	//Depth
	{
		shadow->PreRender();

		Pass(0, 1, 2);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		floor->Render();

		airplane->Render();
		paladin->Render();
		weapon->Render();
	}

	//Refraction
	{
		refraction->PreRender();

		sky->Pass(7);
		sky->Render();

		Pass(3, 4, 5);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		floor->Render();

		airplane->Render();
		paladin->Render();
		weapon->Render();

		terrain->Pass(6);
		terrain->Render();


	}

	sky->PreRender();

}

void RefractionDemo::Render()
{
	sky->Pass(7);
	sky->Render();

	Pass(3, 4, 5);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();

	airplane->Render();
	paladin->Render();
	weapon->Render();

	terrain->Pass(6);
	terrain->Render();

	refraction->Render();
	mirrorPlane->Pass(11);
	mirrorPlane->Render(); // 반투명은 맨 뒤에

}

void RefractionDemo::PostRender()
{
	sky->PostRender();
}

void RefractionDemo::Mesh()
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

		brick = Debug_new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->NormalMap("Bricks_Normal.png");
		brick->Specular(1, 0.3f, 0.3f, 20);
		brick->SpecularMap("Bricks_Specular.png");
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		wall = Debug_new Material(shader);
		wall->DiffuseMap("Wall.png");
		wall->NormalMap("Wall_Normal.png");
		wall->Specular(1, 1, 1, 20);
		wall->SpecularMap("Wall_Specular.png");
		wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}


	//Create Mesh
	{
		Transform* transform = NULL;

		cube = Debug_new MeshRender(shader, Debug_new MeshCube());
		transform = cube->AddTransform();
		transform->SetPosition(128, 0, 128);
		transform->SetScale(20, 10, 20);
		SetTransform(transform);

		cylinder = Debug_new MeshRender(shader, Debug_new MeshCylinder(0.5f, 3.0f, 20, 20));
		sphere = Debug_new MeshRender(shader, Debug_new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->SetPosition(128 - 30, 6, 128 - 15.0f + (float)i * 15.0f);
			transform->SetScale(5, 5, 5);
			SetTransform(transform);

			transform = cylinder->AddTransform();
			transform->SetPosition(128 + 30, 6, 128 - 15.0f + (float)i * 15.0f);
			transform->SetScale(5, 5, 5);
			SetTransform(transform);


			transform = sphere->AddTransform();
			transform->SetPosition(128 - 30, 15.5f, 128 - 15.0f + (float)i * 15.0f);
			transform->SetScale(5, 5, 5);
			SetTransform(transform);

			transform = sphere->AddTransform();
			transform->SetPosition(128 + 30, 15.5f, 128 - 15.0f + (float)i * 15.0f);
			transform->SetScale(5, 5, 5);
			SetTransform(transform);
		}

		mirrorPlane = Debug_new MeshRender(shader, Debug_new MeshPlane());
		transform = mirrorPlane->AddTransform();
		transform->SetPosition(128, 3, 128);
		transform->SetScale(12, 1, 12);
	}

	sphere->UpdateTransforms();
	cylinder->UpdateTransforms();
	cube->UpdateTransforms();
	mirrorPlane->UpdateTransforms();

	meshes.push_back(sphere);
	meshes.push_back(cylinder);
	meshes.push_back(cube);
}

void RefractionDemo::Airplane()
{
	airplane = Debug_new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->SetPosition(128 + 2.0f, 9.91f, 128 + 2.0f);
	transform->SetScale(0.004f, 0.004f, 0.004f);
	SetTransform(transform);
	airplane->UpdateTransforms();

	models.push_back(airplane);

}

void RefractionDemo::Paladin()
{
	paladin = Debug_new ModelAnimator(shader);
	paladin->ReadMesh(L"Paladin/Paladin");
	paladin->ReadMaterial(L"Paladin/Paladin");

	paladin->ReadClip(L"Paladin/BlockIdle");
	paladin->ReadClip(L"Paladin/Idle");
	paladin->ReadClip(L"Paladin/Rolling");
	paladin->ReadClip(L"Paladin/Run");
	paladin->ReadClip(L"Paladin/Walk");

	Transform* transform = nullptr;

	transform = paladin->AddTransform();
	transform->SetPosition(128 + 0, 0, 128 - 30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	paladin->PlayTweenMode(0, 0, 1.0f);

	transform = paladin->AddTransform();
	transform->SetPosition(128 - 15, 0, 128 - 30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	paladin->PlayTweenMode(1, 1, 1.0f);

	transform = paladin->AddTransform();
	transform->SetPosition(128 - 30, 0, 128 - 30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	paladin->PlayTweenMode(2, 2, 0.75f);

	transform = paladin->AddTransform();
	transform->SetPosition(128 + 15, 0, 128 - 30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	paladin->PlayBlendMode(3, 0, 1, 2);
	paladin->SetBlendAlpha(3, 1.5f);

	transform = paladin->AddTransform();
	transform->SetPosition(128 + 30, 0, 128 - 32.5f);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	paladin->PlayTweenMode(4, 4, 0.75f);

	paladin->UpdateTransforms();

	animators.push_back(paladin);

}


void RefractionDemo::Weapon()
{
	weapon = Debug_new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");
	//auto a = weapon->GetModel()->Materials();
	
	UINT count = paladin->GetTransformsCount();
	for (UINT i = 0; i < count; i++)
	{
		weapon->AddTransform();
	}

	weapon->UpdateTransforms();
	models.push_back(weapon);

	weaponTransform = Debug_new Transform();
	weaponTransform->SetPosition(-2.9f, 1.45f, -6.45f);
	weaponTransform->SetScale(0.5f, 0.5f, 0.75f);
	weaponTransform->SetRotation(0, 0, 1);
}

void RefractionDemo::PointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.0f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(-30, 10, -30), 
		5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.6f, 0.2f, 0.0f, 1.0f),
		Color(0.6f, 0.3f, 0.0f, 1.0f),
		Vector3(15, 10, -30), 10.0f, 0.3f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 1.0f, 0.0f, 1.0f), //Diffuse
		Color(0.0f, 0.7f, 0.0f, 1.0f), //Specular
		Color(0.0f, 0.7f, 0.0f, 1.0f), //Emissive
		Vector3(-5, 1, -17.5f), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(0.0f, 0.0f, 1.0f, 1.0f),
		Color(0.0f, 0.0f, 0.7f, 1.0f),
		Color(0.0f, 0.0f, 0.7f, 1.0f),
		Vector3(-10, 1, -17.5f), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);
}

void RefractionDemo::SpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.7f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Vector3(-15, 20, -30), 25.0f,
		Vector3(0, -1, 0), 30.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);

	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Vector3(0, 20, -30), 30.0f,
		Vector3(0, -1, 0), 40.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);
}

void RefractionDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}

void RefractionDemo::SetTransform(Transform * transform)
{
	Vector3 position;
	transform->GetPosition(&position);

	Vector3 scale;
	transform->GetScale(&scale);

	float y = terrain->GetHeight(position);
	position.y += y + scale.y * 0.5f;
	transform->SetPosition(position);
}