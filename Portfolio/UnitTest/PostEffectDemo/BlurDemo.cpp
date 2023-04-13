#include "stdafx.h"
#include "BlurDemo.h"

void BlurDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(50, 2);

	shader = Debug_new Shader(L"42_Billboard.fxo");

	float width = D3D::Width(), height = D3D::Height();

	renderTarget = Debug_new RenderTarget(width, height);

	depthStencil = Debug_new DepthStencil(width, height);
	viewport = Debug_new Viewport(width, height);


	render2D = Debug_new Render2D(); // 원본표시용
	render2D->GetTransform()->SetPosition(200, 120, 0);
	render2D->GetTransform()->SetScale(355, 200, 1);
	render2D->SRV(renderTarget->GetSRV());

	postEffect = Debug_new PostEffect(L"46_Blur.fxo");
	postEffect->SRV(renderTarget->GetSRV());

	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);

	Mesh();
	Airplane();
	Paladin();
	Weapon();

	PointLights();
	SpotLights();
	Billborads();
}

void BlurDemo::Release()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(renderTarget);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(postEffect);
	SAFE_DELETE(viewport);
	SAFE_DELETE(render2D);

	SAFE_DELETE(sky);
	SAFE_DELETE(floor);
	SAFE_DELETE(stone);
	SAFE_DELETE(sphere);

	SAFE_DELETE(brick);
	SAFE_DELETE(wall);

	SAFE_DELETE(cube);
	SAFE_DELETE(plane);
	SAFE_DELETE(sphere);
	SAFE_DELETE(cylinder);

	SAFE_DELETE(airplane);

	SAFE_DELETE(weapon);
	SAFE_DELETE(weaponTransform);

	SAFE_DELETE(billboard);

	SAFE_DELETE(paladin);

}

void BlurDemo::Update()
{
	static UINT Pass = postEffect->GetShader()->PassCount() - 1;
	ImGui::InputInt("Blur Pass", (int *)&Pass);
	Pass %= postEffect->GetShader()->PassCount();
	postEffect->Pass(Pass);


	Vector2 PixelSize = Vector2(1.0f / D3D::Width(), 1.0f / D3D::Height());
	postEffect->GetShader()->AsVector("PixelSize")->SetFloatVector(PixelSize);

	//Blur
	{
		static UINT BlurCount = 1;
		ImGui::InputInt("Blur Count", (int *)&BlurCount);
		BlurCount %= 32;

		postEffect->GetShader()->AsScalar("BlurCount")->SetInt(BlurCount);
	}

	//RadialBlur
	{
		ImGui::Separator();

		static UINT BlurCount = 8;
		ImGui::InputInt("RadialBlurCount", (int *)&BlurCount);
		BlurCount %= 32;
		postEffect->GetShader()->AsScalar("RadialBlurCount")->SetInt(BlurCount);

		static float Radius = 0.6f;
		ImGui::InputFloat("RadialBlurRadius", &Radius, 0.01f);
		postEffect->GetShader()->AsScalar("RadialBlurRadius")->SetFloat(Radius);

		static float Amount = 0.04f;
		ImGui::InputFloat("RadialBlurAmount", &Amount, 0.001f);
		postEffect->GetShader()->AsScalar("RadialBlurAmount")->SetFloat(Amount);


		static float CenterX = 0.5f;
		ImGui::InputFloat("CenterX", &CenterX, 0.01f);

		static float CenterY = 0.5f;
		ImGui::InputFloat("CenterY", &CenterY, 0.01f);
		postEffect->GetShader()->AsVector("RadialCenter")->SetFloatVector(Vector2(CenterX, CenterY));
	}


	sky->Update();

	cube->Update();
	plane->Update();
	sphere->Update();
	cylinder->Update();

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

	billboard->Update();

	render2D->Update();
	postEffect->Update();
}

void BlurDemo::PreRender()
{
	viewport->RSSetViewport();

	//Main Render Target
	{
		renderTarget->PreRender(depthStencil);

		sky->Render();

		Pass(0);

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

		billboard->Pass(4);
		billboard->Render();
	}
}

void BlurDemo::Render()
{
	postEffect->Render();
	render2D->Render();
}

void BlurDemo::Mesh()
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

void BlurDemo::Airplane()
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

void BlurDemo::Paladin()
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

void BlurDemo::Weapon()
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

void BlurDemo::PointLights()
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

void BlurDemo::SpotLights()
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

void BlurDemo::Billborads()
{
	billboard = Debug_new Billboard(shader);
	billboard->AddTexture(L"Terrain/grass_14.tga");
	billboard->AddTexture(L"Terrain/grass_07.tga");
	billboard->AddTexture(L"Terrain/grass_11.tga");


	for (UINT i = 0; i < 2400; i++)
	{
		Vector2 scale = Math::RandomVec2(1, 3);
		Vector2 position = Math::RandomVec2(-60, 60);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale, 0);
	}

	for (UINT i = 0; i < 2400; i++)
	{
		Vector2 scale = Math::RandomVec2(2, 4);
		Vector2 position = Math::RandomVec2(-60, 60);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale, 1);
	}

	for (UINT i = 0; i < 2400; i++)
	{
		Vector2 scale = Math::RandomVec2(3, 5);
		Vector2 position = Math::RandomVec2(-60, 60);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale, 2);
	}
}

void BlurDemo::Pass(UINT _val)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(_val);

	_val = 1;
	for (ModelRender* model : models)
		model->Pass(_val);

	_val = 2;
	for (ModelAnimator* animation : animators)
		animation->Pass(_val);
}