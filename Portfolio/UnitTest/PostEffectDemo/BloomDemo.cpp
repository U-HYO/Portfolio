#include "stdafx.h"
#include "BloomDemo.h"

void BloomDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(50, 2);

	shader = Debug_new Shader(L"42_Billboard.fxo");

	float width = D3D::Width(), height = D3D::Height();

	renderTarget[0] = Debug_new RenderTarget(width, height);
	renderTarget[1] = Debug_new RenderTarget(width, height);
	renderTarget[2] = Debug_new RenderTarget(width, height);
	renderTarget[3] = Debug_new RenderTarget(width, height);
	renderTarget[4] = Debug_new RenderTarget(width, height);
	renderTarget[5] = Debug_new RenderTarget(width, height);

	depthStencil = Debug_new DepthStencil(width, height);
	viewport = Debug_new Viewport(width, height);


	render2D = Debug_new Render2D(); // 원본표시용
	render2D->GetTransform()->SetPosition(200, 120, 0);
	render2D->GetTransform()->SetScale(355, 200, 1);
	render2D->SRV(renderTarget[0]->GetSRV());

	postEffect = Debug_new PostEffect(L"47_Bloom.fxo");
	postEffect->SRV(renderTarget[0]->GetSRV());

	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);

	Mesh();
	Airplane();
	Paladin();
	Weapon();

	PointLights();
	SpotLights();
	Billborads();
}

void BloomDemo::Release()
{
	SAFE_DELETE(shader);

	for(auto a : renderTarget)
	SAFE_DELETE(a);


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

void BloomDemo::Update()
{
	Vector2 PixelSize = Vector2(1.0f / D3D::Width(), 1.0f / D3D::Height());
	postEffect->GetShader()->AsVector("PixelSize")->SetFloatVector(PixelSize);


	ImGui::InputFloat("Threshold", &threshold, 0.01f);
	postEffect->GetShader()->AsScalar("Threshold")->SetFloat(threshold);


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

void BloomDemo::PreRender()
{
	viewport->RSSetViewport();

	//Main Render Target
	{
		renderTarget[0]->PreRender(depthStencil);

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

	//Luminosity
	{
		renderTarget[1]->PreRender(depthStencil);

		postEffect->Pass(1); // 이전 결과를 가져와서 렌더링한 후 1번에 저장 
		postEffect->SRV(renderTarget[0]->GetSRV());
		postEffect->Render();
	}

	{
		float x = 1.0f / D3D::Width();
		float y = 1.0f / D3D::Height();

		GetBlurParams(weightX, offsetX, x, 0);
		GetBlurParams(weightY, offsetY, 0, y);
	}

	ImGui::InputInt("BlurCount", (int *)&blurCount, 2);
	blurCount = Math::Clamp<UINT>(blurCount, 1, 15);
	postEffect->GetShader()->AsScalar("SampleCount")->SetInt(blurCount);

	//BlurX
	{
		postEffect->GetShader()->AsScalar("Weights")->SetFloatArray(&weightX[0], 0, weightX.size());
		postEffect->GetShader()->AsVector("Offsets")->SetRawValue(&offsetX[0], 0, sizeof(Vector2) * offsetX.size());

		renderTarget[2]->PreRender(depthStencil);

		postEffect->Pass(2);
		postEffect->SRV(renderTarget[0]->GetSRV());
		postEffect->Render();
	}

	//BlurY
	{
		postEffect->GetShader()->AsScalar("Weights")->SetFloatArray(&weightY[0], 0, weightY.size());
		postEffect->GetShader()->AsVector("Offsets")->SetRawValue(&offsetY[0], 0, sizeof(Vector2) * offsetY.size());

		renderTarget[3]->PreRender(depthStencil);

		postEffect->Pass(2);
		postEffect->SRV(renderTarget[2]->GetSRV());
		postEffect->Render();
	}

	//Composite
	{
		static float BlurRatio = 0.5f;
		ImGui::SliderFloat("BlurRatio", &BlurRatio, 0, 1);
		postEffect->GetShader()->AsScalar("BlurRatio")->SetFloat(BlurRatio);

		postEffect->GetShader()->AsSRV("LuminosityMap")->SetResource(renderTarget[1]->GetSRV());
		postEffect->GetShader()->AsSRV("BlurMap")->SetResource(renderTarget[3]->GetSRV());


		renderTarget[4]->PreRender(depthStencil);

		postEffect->Pass(3);
		postEffect->SRV(renderTarget[0]->GetSRV());
		postEffect->Render();
	}

	//Color Grading
	{
		static float Grading = 0.5f;
		ImGui::InputFloat("Grading", &Grading, 0.01f);
		postEffect->GetShader()->AsScalar("Grading")->SetFloat(Grading);

		static float Correlation = 0.5f;
		ImGui::InputFloat("Correlation", &Correlation, 0.01f);
		postEffect->GetShader()->AsScalar("Correlation")->SetFloat(Correlation);

		static float Concentration = 2.0f;
		ImGui::InputFloat("Concentration", &Concentration, 0.01f);
		postEffect->GetShader()->AsScalar("Concentration")->SetFloat(Concentration);


		renderTarget[5]->PreRender(depthStencil);

		postEffect->Pass(4);
		postEffect->SRV(renderTarget[4]->GetSRV());
		postEffect->Render();
	}

}

void BloomDemo::Render()
{
	static UINT SrvIndex = 5;
	ImGui::InputInt("SrvIndex", (int *)&SrvIndex);
	SrvIndex %= 6;

	postEffect->Pass(0);
	postEffect->SRV(renderTarget[SrvIndex]->GetSRV());
	postEffect->Render();
	render2D->Render();
}

void BloomDemo::Mesh()
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

void BloomDemo::Airplane()
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

void BloomDemo::Paladin()
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

void BloomDemo::GetBlurParams(std::vector<float>& _weights, std::vector<Vector2>& _offsets, float _x, float _y)
{
	_weights.clear();
	_weights.assign(blurCount, float());

	_offsets.clear();
	_offsets.assign(blurCount, Vector2());

	_weights[0] = Math::Gaussian(0, blurCount); //1
	_offsets[0] = Vector2(0, 0);

	 sum = _weights[0];
	for (UINT i = 0; i < blurCount / 2; i++)
	{
		float weight = Math::Gaussian(float(i+1), blurCount);
		_weights[i * 2 + 1] = weight;
		_weights[i * 2 + 2] = weight;
		sum += weight * 2;

		Vector2 offset = Vector2(_x, _y) * (i * 2.0f + 1.0f);
		_offsets[i * 2 + 1] = offset;
		_offsets[i * 2 + 2] = -offset;
	}

	for (UINT i = 0; i < blurCount; i++)
		_weights[i] /= sum;
	
	ImGui::Text("%f", sum);


}

void BloomDemo::Weapon()
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

void BloomDemo::PointLights()
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

void BloomDemo::SpotLights()
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

void BloomDemo::Billborads()
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

void BloomDemo::Pass(UINT _val)
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