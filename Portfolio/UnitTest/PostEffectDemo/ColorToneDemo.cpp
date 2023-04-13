#include "stdafx.h"
#include "ColorToneDemo.h"

void ColorToneDemo::Init()
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

	postEffect = Debug_new PostEffect(L"44_ColorTone.fxo");
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

void ColorToneDemo::Release()
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

void ColorToneDemo::Update()
{
	static UINT Pass = postEffect->GetShader()->PassCount() - 1;
	ImGui::InputInt("PostEffect Pass", (int *)&Pass);
	Pass %= postEffect->GetShader()->PassCount();
	postEffect->Pass(Pass);

	static float thick = 1.0f;
	ImGui::InputFloat("Thick", &thick, 0.01f);

	Vector2 PixelSize = Vector2(thick / D3D::Width(), thick / D3D::Height());
	postEffect->GetShader()->AsVector("PixelSize")->SetFloatVector(PixelSize);

	//Saturation
	{
		ImGui::Separator();

		static float Saturation = 0.0f;
		//float Saturation = Math::Random(-5.0f, 5.0f);
		ImGui::InputFloat("Saturation", &Saturation, 0.1f);
		postEffect->GetShader()->AsScalar("Saturation")->SetFloat(Saturation);
	}

	//Sharpness
	{
		ImGui::Separator();
		
		static float Sharpness = 0.5f;
		ImGui::InputFloat("Sharpness", &Sharpness, 0.01f);
		postEffect->GetShader()->AsScalar("Sharpness")->SetFloat(Sharpness);

		static Color edgeColor = Color(1.0, 1.0f, 1.0f, 1.0f);
		ImGui::ColorEdit4("EdgeColor", edgeColor);
		postEffect->GetShader()->AsScalar("colorR")->SetFloat(edgeColor.r);
		postEffect->GetShader()->AsScalar("colorG")->SetFloat(edgeColor.g);
		postEffect->GetShader()->AsScalar("colorB")->SetFloat(edgeColor.b);
	}

	//Wiggle
	{
		ImGui::Separator();

		static float OffsetX = 10;
		ImGui::InputFloat("OffsetX", &OffsetX, 0.1f);

		static float OffsetY = 10;
		ImGui::InputFloat("OffsetY", &OffsetY, 0.1f);
		postEffect->GetShader()->AsVector("WiggleOffset")->SetFloatVector(Vector2(OffsetX, OffsetY));

		static float AmountX = 0.01f;
		ImGui::InputFloat("AmountX", &AmountX, 0.001f);

		static float AmountY = 0.01f;
		ImGui::InputFloat("AmountY", &AmountY, 0.001f);
		postEffect->GetShader()->AsVector("WiggleAmount")->SetFloatVector(Vector2(AmountX, AmountY));
	}

	//Vignette
	{
		ImGui::Separator();

		static float Power = 2.0f;
		ImGui::InputFloat("Power", &Power, 0.05f);
		postEffect->GetShader()->AsScalar("Power")->SetFloat(Power);

		static float ScaleX = 2.0f;
		ImGui::InputFloat("ScaleX", &ScaleX, 0.05f);

		static float ScaleY = 2.0f;
		ImGui::InputFloat("ScaleY", &ScaleY, 0.05f);
		postEffect->GetShader()->AsVector("Scale")->SetFloatVector(Vector2(ScaleX, ScaleY));
	}

	//Interace
	{
		ImGui::Separator();

		static float Strength = 1.0f;
		ImGui::InputFloat("Strength", &Strength, 0.01f);
		postEffect->GetShader()->AsScalar("Strength")->SetFloat(Strength);

		static int InteraceValue = 2;
		ImGui::InputInt("InteraceValue", &InteraceValue);
		postEffect->GetShader()->AsScalar("InteraceValue")->SetInt(InteraceValue);
	}

	//LensDistortion
	{
		ImGui::Separator();

		static float LensPower = 1.0f;
		ImGui::InputFloat("LensPower", &LensPower, 0.01f);
		postEffect->GetShader()->AsScalar("LensPower")->SetFloat(LensPower);

		static float Distortion = -0.02f;
		ImGui::InputFloat("Distortion", &Distortion, 0.001f);

		static float ChromaticDistortion = -0.01f;
		ImGui::InputFloat("ChromaticDistortion", &ChromaticDistortion, 0.001f);

		Vector3 temp = Vector3(Distortion, 0, 0);
		temp.y = Distortion + ChromaticDistortion;
		temp.z = Distortion + 2 * ChromaticDistortion;

		postEffect->GetShader()->AsVector("Distortion")->SetFloatVector(temp);
	}


	sky->Update();

	cube->Update();
	plane->Update();
	sphere->Update();
	cylinder->Update();

	airplane->Update();
	paladin->Update();
	PaladinMove();

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

void ColorToneDemo::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

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

void ColorToneDemo::Render()
{
	postEffect->Render();
	render2D->Render();
	if (_KEYCODE->Down(VK_SPACE))
	{
		renderTarget->SaveTexture(L"../RTVTest.png");
	}
}

void ColorToneDemo::Mesh()
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

void ColorToneDemo::Airplane()
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

void ColorToneDemo::Paladin()
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

void ColorToneDemo::Weapon()
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

void ColorToneDemo::PointLights()
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

void ColorToneDemo::SpotLights()
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

void ColorToneDemo::Billborads()
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

void ColorToneDemo::Pass(UINT _val)
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

void ColorToneDemo::PaladinMove()
{
	_KEYCODE->Toggle(VK_TAB) ? Context::Get()->SetCameraIndex(1) : Context::Get()->SetCameraIndex(0);
	//((FallowCamera*)Context::Get()->GetCamera(1))->GetTransform()->SetParent(paladin->GetTransform(0)->GetParent());
	//((FallowCamera*)Context::Get()->GetCamera(1))->GetTransform()->SetParentS(paladin->GetTransform(0)->GetScaleMat());
	((FallowCamera*)Context::Get()->GetCamera(1))->SetTarget(paladin->GetTransform(0)->GetPosition(), paladin->GetTransform(0)->GetForward());

	Vector3 pos = paladin->GetTransform(0)->GetPosition();
	static float moveSpeed = 100.0f;
	ImGui::SliderFloat("MoveSpeed", &moveSpeed, 100.0f, 500.0f);

	Vector3 f = paladin->GetTransform(0)->GetForward();
	Vector3 u = paladin->GetTransform(0)->GetUp();
	Vector3 r = paladin->GetTransform(0)->GetRight();
	//((FallowCamera*)Context::Get()->GetCamera(1))->SetTarget(pos);

	_KEYCODE->Toggle(VK_TAB) ? Context::Get()->SetCameraIndex(1) : Context::Get()->SetCameraIndex(0);

	//if (Context::Get()->GetCameraIndex() == 1)
	{
		if (_KEYCODE->Press('W'))
			pos -= f * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('S'))
			pos += f * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('D'))
			pos -= r * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('A'))
			pos += r * moveSpeed * Time::Delta();

		paladin->GetTransform(0)->SetPosition(pos);


		{
			Vector3 rot;
			paladin->GetTransform(0)->GetRotation(&rot);

			if (_KEYCODE->Press(VK_LEFT))
				rot.y -= 2.0f * Time::Delta();

			if (_KEYCODE->Press(VK_RIGHT))
				rot.y += 2.0f * Time::Delta();


			paladin->GetTransform(0)->SetRotation(rot);
		}
		for (const auto iter : paladin->GetTransforms())
			iter->Update();
		paladin->UpdateTransforms();
	}
}
