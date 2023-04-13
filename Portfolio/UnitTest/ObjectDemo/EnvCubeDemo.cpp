#include "stdafx.h"
#include "EnvCubeDemo.h"

void EnvCubeDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(50, 2);

	shader = Debug_new Shader(L"48_EnvCube.fxo");
	envCube = Debug_new EnvCube(shader, 256, 256);

	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);

	Mesh();
	Airplane();
	Paladin();
	//Paladin2();
	Weapon();

	PointLights();
	SpotLights();
}

void EnvCubeDemo::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(envCube);

	SAFE_DELETE(sky);
	SAFE_DELETE(floor);
	SAFE_DELETE(stone);

	SAFE_DELETE(brick);
	SAFE_DELETE(wall);

	SAFE_DELETE(cube);
	SAFE_DELETE(plane);
	SAFE_DELETE(sphere);
	SAFE_DELETE(sphere2);
	SAFE_DELETE(cylinder);

	SAFE_DELETE(airplane);

	SAFE_DELETE(weapon);
	SAFE_DELETE(weaponTransform);

	SAFE_DELETE(paladin);
	//SAFE_DELETE(paladin2);

}

void EnvCubeDemo::Update()
{

	ImGui::InputInt("Type", (int *)&envCube->Type());
	envCube->Type() %= 4;
	
	ImGui::InputFloat("Alpha", &envCube->Alpha(), 0.01f);
	ImGui::InputFloat("RefractAmount", &envCube->RefractAmount(), 0.01f);
	
	ImGui::InputFloat("FresnelAmount", &envCube->FresnelAmount(), 0.01f);
	ImGui::InputFloat("FresnelBias", &envCube->FresnelBias(), 0.01f);
	ImGui::InputFloat("FresnelScale", &envCube->FresnelScale(), 0.01f);


	Vector3 position;
	sphere2->GetTransform(0)->GetPosition(&position);
	//paladin2->GetTransform(0)->GetPosition(&position);

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

	sphere2->GetTransform(0)->SetPosition(position);
	sphere2->UpdateTransforms();

	//paladin2->GetTransform(0)->SetPosition(position);
	//paladin2->UpdateTransforms();


	sky->Update();

	cube->Update();
	plane->Update();
	cylinder->Update();
	sphere->Update();
	sphere2->Update();

	airplane->Update();
	paladin->Update();
	//paladin2->Update();

	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < paladin->GetTransformsCount(); i++)
	{
		paladin->GetAttachBones(i, worlds);
		weapon->GetTransform(i)->SetWorld(weaponTransform->GetWorld() * worlds[40]);
	}
	weapon->UpdateTransforms();
	weapon->Update();

}

void EnvCubeDemo::PreRender()
{
	//EnvCube PreRender
	{
		Vector3 p, s;
		sphere2->GetTransform(0)->GetPosition(&p);
		sphere2->GetTransform(0)->GetScale(&s);
	
		//paladin2->GetTransform(0)->GetPosition(&p);
		//paladin2->GetTransform(0)->GetScale(&s);


		envCube->PreRender(p, s, 0.1f, 1200, 0.5f);

		sky->Pass(0);
		sky->Render();

		Pass(1);

		wall->Render();
		sphere->Render();
		
		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		floor->Render();
		plane->Render();

		airplane->Render();
		paladin->Render();
		weapon->Render();
	}
}

void EnvCubeDemo::Render()
{
	sky->Pass(4);
	sky->Render();

	Pass(5);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();
	plane->Render();

	airplane->Render();
	paladin->Render();
	weapon->Render();


	envCube->Render();
	sphere2->Pass(8);
	sphere2->Render();

	//envCube->Render();
	//paladin2->Pass(10);
	//paladin2->Render();

}

void EnvCubeDemo::Mesh()
{
	{//Create Material
		floor = Debug_new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->NormalMap("Floor_Normal.png");
		floor->Specular(1, 1, 1, 20);
		floor->SpecularMap("Floor_Specular.png");
		floor->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

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

	{//Create Mesh
		Transform* transform = NULL;

		cube = Debug_new MeshRender(shader, Debug_new MeshCube());
		transform = cube->AddTransform();
		transform->SetPosition(0, 5, 0);
		transform->SetScale(20, 10, 20);

		plane = Debug_new MeshRender(shader, Debug_new MeshPlane(2.5f, 2.5f));
		transform = plane->AddTransform();
		transform->SetScale(12, 1, 12);

		cylinder = Debug_new MeshRender(shader, Debug_new MeshCylinder(0.3f, 0.5f, 3.0f, 20, 20));
		sphere = Debug_new MeshRender(shader, Debug_new MeshSphere(0.5f, 20, 20));

		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->SetPosition(-30, 6, (float)i * 15.0f - 15.0f);
			transform->SetScale(5, 5, 5);

			transform = cylinder->AddTransform();
			transform->SetPosition(30, 6, (float)i * 15.0f - 15.0f);
			transform->SetScale(5, 5, 5);

			transform = sphere->AddTransform();
			transform->SetPosition(-30, 15.5f, (float)i * 15.0f - 15.0f);
			transform->SetScale(5, 5, 5);

			transform = sphere->AddTransform();
			transform->SetPosition(30, 15.5f, (float)i * 15.0f - 15.0f);
			transform->SetScale(5, 5, 5);
		}

		sphere2 = Debug_new MeshRender(shader, Debug_new MeshSphere(0.5f, 20, 20));
		transform = sphere2->AddTransform();
		transform->SetPosition(0, 20, 0);
		transform->SetScale(5, 5, 5);

	}

	cube->UpdateTransforms();
	plane->UpdateTransforms();
	cylinder->UpdateTransforms();
	sphere->UpdateTransforms();

	meshes.push_back(cube);
	meshes.push_back(plane);
	meshes.push_back(cylinder);
	meshes.push_back(sphere);
}

void EnvCubeDemo::Airplane()
{
	airplane = Debug_new ModelRender(shader);
	airplane->ReadMesh(L"Airplane/Airplane");
	airplane->ReadMaterial(L"Airplane/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->SetPosition(2.0f, 9.91f, 2.0f);
	transform->SetScale(0.004f, 0.004f, 0.004f);
	airplane->UpdateTransforms();

	models.push_back(airplane);

}

void EnvCubeDemo::Paladin()
{
	paladin = Debug_new ModelAnimator(shader);
	paladin->ReadMesh(L"Paladin/Paladin");
	paladin->ReadMaterial(L"Paladin/Paladin");

	paladin->ReadClip(L"Paladin/BlockIdle");
	paladin->ReadClip(L"Paladin/Idle");
	paladin->ReadClip(L"Paladin/Rolling");
	paladin->ReadClip(L"Paladin/Run");
	paladin->ReadClip(L"Paladin/Walk");

	Transform* transform = NULL;

	transform = paladin->AddTransform();
	transform->SetPosition(0, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin->PlayTweenMode(0, 0, 1.0f);

	transform = paladin->AddTransform();
	transform->SetPosition(-15, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin->PlayTweenMode(1, 1, 1.0f);

	transform = paladin->AddTransform();
	transform->SetPosition(-30, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin->PlayTweenMode(2, 2, 0.75f);

	transform = paladin->AddTransform();
	transform->SetPosition(15, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin->PlayBlendMode(3, 0, 1, 2);
	paladin->SetBlendAlpha(3, 1.75f);

	transform = paladin->AddTransform();
	transform->SetPosition(30, 0, -32.5f);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin->PlayTweenMode(4, 4, 0.75f);

	paladin->UpdateTransforms();

	animators.push_back(paladin);

}

void EnvCubeDemo::Paladin2()
{
	paladin2 = Debug_new ModelAnimator(shader);
	paladin2->ReadMesh(L"Paladin/Paladin");
	paladin2->ReadMaterial(L"Paladin/Paladin");

	paladin2->ReadClip(L"Paladin/BlockIdle");
	paladin2->ReadClip(L"Paladin/Idle");
	paladin2->ReadClip(L"Paladin/Rolling");
	paladin2->ReadClip(L"Paladin/Run");
	paladin2->ReadClip(L"Paladin/Walk");

	Transform* transform = NULL;

	transform = paladin2->AddTransform();
	transform->SetPosition(0, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin2->PlayTweenMode(0, 0, 1.0f);

	transform = paladin2->AddTransform();
	transform->SetPosition(-15, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin2->PlayTweenMode(1, 1, 1.0f);

	transform = paladin2->AddTransform();
	transform->SetPosition(-30, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin2->PlayTweenMode(2, 2, 0.75f);

	transform = paladin2->AddTransform();
	transform->SetPosition(15, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin2->PlayBlendMode(3, 0, 1, 2);
	paladin2->SetBlendAlpha(3, 1.75f);

	transform = paladin2->AddTransform();
	transform->SetPosition(30, 0, -32.5f);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin2->PlayTweenMode(4, 4, 0.75f);

	paladin2->UpdateTransforms();

	animators.push_back(paladin2);
}


void EnvCubeDemo::Weapon()
{
	weapon = Debug_new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	UINT count = paladin->GetTransformsCount();
	for (UINT i = 0; i < count; i++)
		weapon->AddTransform();

	weapon->UpdateTransforms();
	models.push_back(weapon);


	weaponTransform = Debug_new Transform();
	weaponTransform->SetPosition(-2.9f, 1.45f, -6.45f);
	weaponTransform->SetScale(0.5f, 0.5f, 0.75f);
	weaponTransform->SetRotation(0, 0, 1);
}

void EnvCubeDemo::PointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.0f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(-30, 10, -30), 5.0f, 0.9f
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

void EnvCubeDemo::SpotLights()
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

void EnvCubeDemo::Pass(UINT _val)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(_val);

	_val++;
	for (ModelRender* model : models)
		model->Pass(_val);

	_val++;
	for (ModelAnimator* animation : animators)
		animation->Pass(_val);
}