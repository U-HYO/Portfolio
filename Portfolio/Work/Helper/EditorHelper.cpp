#include "stdafx.h"
#include "EditorHelper.h"

EditorHelper* EditorHelper::instance = NULL;

EditorHelper * EditorHelper::Get()
{
	assert(instance != NULL);

	return instance;
}

void EditorHelper::Create()
{
	assert(instance == NULL);

	instance = Debug_new EditorHelper();
}

void EditorHelper::Update()
{
	for (auto& actor : instance->actors)
	{
		if (!actor->IsActive())
			continue;

		actor->Update();
	}

	for (auto& actor : instance->actors)
	{
		if(!actor->HasComponent<Lighting>())
			continue;

		if (!actor->IsActive())
			continue;

		std::string name = actor->GetName();
		int num = atoi(name.c_str());

		if (name.find("Point") != std::string::npos)
		{
			Lighting::Get()->GetPointLight(num).Position = actor->GetTransform()->GetPosition();
		}
		else if (name.find("Spot") != std::string::npos)
		{
			Lighting::Get()->GetSpotLight(num).Position = actor->GetTransform()->GetPosition();
		}


	}

}

void EditorHelper::Delete()
{
	SAFE_DELETE(instance);
	
	
}

void EditorHelper::PreRender()
{
	//Depth
	{
		//shadow
		instance->shadow->PreRender();
		instance->Pass(0, 1, 2);

		for (auto& actor : instance->actors)
		{
			if(actor->GetTransform()->GetRender() && !actor->HasRender<PostEffect>())
				actor->Render();
		}
	}

	//Reflection
	{
		instance->Pass(11, 12, 13);
		for (auto& actor : instance->actors)
		{
			if (actor->HasRender<Water>())
				actor->GetRender<Water>()->PreRender_Reflection();
		}

		for (auto& actor : instance->actors)
		{
			if (!actor->GetTransform()->GetRender() || actor->HasRender<PostEffect>() || actor->HasRender<Water>())
				continue;
			
			if (actor->HasComponent<Sky>())
				actor->GetComponent<Sky>()->Pass(15);

			if (actor->HasRender<TerrainLOD>())
				actor->GetRender<TerrainLOD>()->Pass(0);

			if (actor->HasRender<Terrain>())
				actor->GetRender<Terrain>()->Pass(14);


				actor->Render();	
		}
	}

	//Refraction
	{
		instance->Pass(3, 4, 5);
		for (auto& actor : instance->actors)
		{
			if (actor->HasRender<Water>())
				actor->GetRender<Water>()->PreRender_Refraction();
		}

		for (auto& actor : instance->actors)
		{
			if (!actor->GetTransform()->GetRender() || actor->HasRender<PostEffect>() || actor->HasRender<Water>())
				continue;

			if (actor->HasComponent<Sky>())
				actor->GetComponent<Sky>()->Pass(7);

			if (actor->HasRender<TerrainLOD>())
				actor->GetRender<TerrainLOD>()->Pass(0);

			if (actor->HasRender<Terrain>())
				actor->GetRender<Terrain>()->Pass(6);


			actor->Render();
		}
	}

	for (auto& actor : instance->actors)
	{
		if (actor->HasComponent<Sky>())
			actor->GetComponent<Sky>()->PreRender();
	}



	///////////RenderTarget//////////////////////////////////
	instance->viewport->RSSetViewport();
	instance->renderTarget->PreRender(instance->depthStencil);
	
	for (auto& actor : instance->actors)
	{
		if (!actor->IsActive())
			continue;

		if (actor->GetComponent<CubeSky>() && actor->GetTransform()->GetRender())
		{
			actor->GetComponent<CubeSky>()->Pass(0);
			actor->Render();
		}

		if (actor->GetComponent<Sky>() && actor->GetTransform()->GetRender())
		{
			actor->GetComponent<Sky>()->Pass(7);
			actor->Render();
		}
	}
	instance->Pass(3, 4, 5);

	for (auto& actor : instance->actors)
	{
		if (!actor->IsActive())
			continue;

		if (!(actor->GetComponent<CubeSky>() || actor->GetComponent<Sky>()) && actor->GetTransform()->GetRender() && !actor->HasRender<PostEffect>())
			actor->Render();
		
	}
	////////////////////////////////////////////////////////



	//Post Effect
	for (auto& actor : instance->actors)
	{
		if (actor->GetName() == "BloomEffect")			{ instance->postEffectType = PostEffectType::Bloom; }
		else if (actor->GetName() == "BlurEffect")		{ instance->postEffectType = PostEffectType::Blur; }
		else if (actor->GetName() == "ColorToneEffect") { instance->postEffectType = PostEffectType::ColorTone; }		
		else											{ instance->postEffectType = PostEffectType::Unknown; }

		if (!actor->IsActive())
			continue;

		auto postEffect = actor->GetRender<PostEffect>();

		switch (instance->postEffectType)
		{
		case PostEffectType::Unknown:			break;
		case PostEffectType::Bloom:
		{
			//Luminosity
			{
				instance->bloomRenderTarget[0]->PreRender(instance->depthStencil);
				postEffect->Pass(1); // 이전 결과를 가져와서 렌더링한 후 1번에 저장 
				postEffect->SRV(instance->renderTarget->GetSRV());
				postEffect->Render();
			}

			{
				float x = 1.0f / D3D::Width();
				float y = 1.0f / D3D::Height();

				instance->GetBlurParams(instance->weightX, instance->offsetX, x, 0, instance->sum, instance->blurCount);
				instance->GetBlurParams(instance->weightY, instance->offsetY, 0, y, instance->sum, instance->blurCount);
			}
			postEffect->GetShader()->AsScalar("SampleCount")->SetInt(instance->blurCount);

			//BlurX
			{
				postEffect->GetShader()->AsScalar("Weights")->SetFloatArray(&instance->weightX[0], 0, instance->weightX.size());
				postEffect->GetShader()->AsVector("Offsets")->SetRawValue(&instance->offsetX[0], 0, sizeof(Vector2) * instance->offsetX.size());
				
				instance->bloomRenderTarget[1]->PreRender(instance->depthStencil);

				postEffect->Pass(2);
				postEffect->SRV(instance->renderTarget->GetSRV());
				postEffect->Render();
			}

			//BlurY
			{
				postEffect->GetShader()->AsScalar("Weights")->SetFloatArray(&instance->weightY[0], 0, instance->weightY.size());
				postEffect->GetShader()->AsVector("Offsets")->SetRawValue(&instance->offsetY[0], 0, sizeof(Vector2) * instance->offsetY.size());
				
				instance->bloomRenderTarget[2]->PreRender(instance->depthStencil);
				
				postEffect->Pass(2);
				postEffect->SRV(instance->bloomRenderTarget[1]->GetSRV());
				postEffect->Render();
			}
	
			//Composite
			{
				postEffect->GetShader()->AsScalar("BlurRatio")->SetFloat(instance->blurRatio);

				postEffect->GetShader()->AsSRV("LuminosityMap")->SetResource(instance->bloomRenderTarget[0]->GetSRV());
				postEffect->GetShader()->AsSRV("BlurMap")->SetResource(instance->bloomRenderTarget[2]->GetSRV());

				instance->bloomRenderTarget[3]->PreRender(instance->depthStencil);

				postEffect->Pass(3);
				postEffect->SRV(instance->renderTarget->GetSRV());
				postEffect->Render();
			}

			//Color Grading
			{
				postEffect->GetShader()->AsScalar("Grading")->SetFloat(instance->grading);
				postEffect->GetShader()->AsScalar("Correlation")->SetFloat(instance->correlation);
				postEffect->GetShader()->AsScalar("Concentration")->SetFloat(instance->concentration);

				instance->bloomRenderTarget[4]->PreRender(instance->depthStencil);

				postEffect->Pass(4);
				postEffect->SRV(instance->bloomRenderTarget[3]->GetSRV());
				postEffect->Render();
			}
			break;
		}
		case PostEffectType::Blur:
		{
			break;
		}
		case PostEffectType::ColorTone:
			break;
		default:			break;
		}




	}





}

void EditorHelper::Render()
{
	//instance->Pass(3, 4, 5);

	//for (auto& actor : instance->actors)
	//{
	//	if (actor->GetComponent<CubeSky>() && actor->GetTransform()->GetRender())
	//	{
	//		actor->GetComponent<CubeSky>()->Pass(0);
	//		actor->Render();
	//	}

	//	if (actor->GetComponent<Sky>() && actor->GetTransform()->GetRender())
	//	{
	//		actor->GetComponent<Sky>()->Pass(7);
	//		actor->Render();
	//	}
	//}
	//
	//for (auto& actor : instance->actors)
	//{
	//	if ( (!actor->GetComponent<CubeSky>() || !actor->GetComponent<Sky>()) && actor->GetTransform()->GetRender())
	//		actor->Render();
	//}

	for (auto& actor : instance->actors)
	{
		if (!actor->IsActive() || !actor->HasRender<PostEffect>())
			continue;

		if (actor->GetName() == "BloomEffect") { instance->postEffectType = PostEffectType::Bloom; }
		else if (actor->GetName() == "BlurEffect") { instance->postEffectType = PostEffectType::Blur; }
		else if (actor->GetName() == "ColorToneEffect") { instance->postEffectType = PostEffectType::ColorTone; }
		else { instance->postEffectType = PostEffectType::Unknown; }

		switch (instance->postEffectType)
		{
		case PostEffectType::Unknown:			
		{
			actor->GetRender<PostEffect>()->SRV(instance->renderTarget->GetSRV());
			actor->Render();
			break;
		}
		case PostEffectType::Bloom:		
		{
			actor->GetRender<PostEffect>()->Pass(0);
			actor->GetRender<PostEffect>()->SRV(instance->bloomRenderTarget[instance->srvIndex]->GetSRV());
			actor->Render();
			break;
		}
		case PostEffectType::Blur:		
		{
			//actor->GetRender<PostEffect>()->SRV(instance->bloomRenderTarget[instance->srvIndex]->GetSRV());
			actor->Render();
			break;
		}
		case PostEffectType::ColorTone:		
		{
			actor->Render();
			break;
		}
		default:
			break;
		}
	}

}

void EditorHelper::PostRender()
{
}

void EditorHelper::Init()
{
	Vector3 zero = Vector3(0, 0, 0);
	modelShader = Debug_new Shader(L"62_Water.fxo");
//	terrainLODShader = Debug_new Shader(L"57_TerrainLOD.fxo");
	shadow = Debug_new Shadow(modelShader, zero, 65);

	postEffectType = PostEffectType::Unknown;

	float width = D3D::Width(), height = D3D::Height();

	renderTarget = Debug_new RenderTarget(width, height);

	bloomRenderTarget[0] = Debug_new RenderTarget(width, height);
	bloomRenderTarget[1] = Debug_new RenderTarget(width, height);
	bloomRenderTarget[2] = Debug_new RenderTarget(width, height);
	bloomRenderTarget[3] = Debug_new RenderTarget(width, height);
	bloomRenderTarget[4] = Debug_new RenderTarget(width, height);
	depthStencil = Debug_new DepthStencil(width, height);
	viewport = Debug_new Viewport(width, height);

	PostEffectInit();

}

void EditorHelper::LoadModel(const std::wstring & path)
{
	//Thread::Get()->AddTask([this, path]()
	//{
	const auto name = FileSystem::GetFileNameFromPath(FileSystem::ToString(path));

	for (const auto actor : actors)
	{
		//같은 모델을 불러왔을 때
		if (actor->GetName() == name)
		{
			Transform* transform = nullptr;
			auto count = actor->GetComponent<ModelAnimator>()->GetTransformsCount();
			transform = actor->GetComponent<ModelAnimator>()->AddTransform();
			transform->SetPosition(0 + (5 * count), 0, 10 * actors.size());
			transform->SetScale(0.075f, 0.075f, 0.075f);
			actor->GetComponent<ModelAnimator>()->PlayTweenMode(0, 0, 1.0f);
			actor->GetComponent<ModelAnimator>()->UpdateTransforms();
			//actor->Init(transform);
			//actors.emplace_back(actor);
			return;
		}
	}

	//});

	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->SetName(name);
	actor->AddComponent<ModelAnimator>();
	actor->GetComponent<ModelAnimator>()->SetShader(modelShader);
	actor->GetComponent<ModelAnimator>()->ReadMesh(path);
	actor->GetComponent<ModelAnimator>()->ReadMaterial(path);
	actor->GetComponent<ModelAnimator>()->ReadClip(FileSystem::ToWString(name) + L"/Idle");
	Transform* transform = nullptr;
	transform = actor->GetComponent<ModelAnimator>()->AddTransform();
	transform->SetPosition(0, 0, 10 * actors.size());
	transform->SetScale(0.075f, 0.075f, 0.075f);
	actor->GetComponent<ModelAnimator>()->PlayTweenMode(0, 0, 1.0f);
	actor->GetComponent<ModelAnimator>()->UpdateTransforms();
	//actor->Init(transform);

	actors.emplace_back(actor);

}

void EditorHelper::LoadSkycube(const std::wstring & path)
{
	const auto name = FileSystem::GetFileNameFromPath(FileSystem::ToString(path));
	
	for (const auto actor : actors)
	{
		//이미 큐브스카이가 있을때
		//if (actor->GetName() == name)
		if(actor->HasComponent<CubeSky>())
		{
			actor->SetName(name);
			actor->GetComponent<CubeSky>()->SetCubeSky(FileSystem::ToWString(name));
			return;
		}
	}

	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->SetName(name);
	actor->AddComponent<CubeSky>();
	actor->GetComponent<CubeSky>()->SetShader(nullptr);
	actor->GetComponent<CubeSky>()->SetCubeSky(FileSystem::ToWString(name));
	actor->Init(actor->GetComponent<CubeSky>()->GetTransform());
	actors.emplace_back(actor);
}

void EditorHelper::LoadSky()
{
	for (auto a : actors)
	{
		if (a->HasComponent<Sky>())
			return;
	}

	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->SetName("Sky");
	actor->AddComponent<Sky>();
	actor->GetComponent<Sky>()->SetShader(modelShader);
	actor->GetComponent<Sky>()->RealTime(false, Math::PI - 1e-6f);
	actor->Init(actor->GetComponent<Sky>()->GetTransform());
	actors.emplace_back(actor);

}

void EditorHelper::LoadMesh(Mesh* mesh)
{
	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->AddComponent<MeshRender>();
	actor->GetComponent<MeshRender>()->SetMesh(mesh, modelShader);
	auto name = actor->GetComponent<MeshRender>()->GetMaterialName();
	int a = 0;
	for (auto actor : actors)
	{
		if (actor->GetName() == name + std::to_string(a))
		{
			a++;
			continue;
		}
	}
	name += std::to_string(a);
	actor->SetName(name);
	

	Transform* transform = nullptr;
	transform = actor->GetComponent<MeshRender>()->AddTransform();
	transform->SetPosition(0, 0, 10);
	transform->SetScale(5.0f);
	actor->GetComponent<MeshRender>()->UpdateTransforms();
	actor->Init(transform);

	actors.emplace_back(actor);
}

void EditorHelper::LoadMeshModel(const std::wstring & path)
{
	const auto name = FileSystem::GetFileNameFromPath(FileSystem::ToString(path));
	
	for (const auto actor : actors)
	{
		//같은 모델을 불러왔을 때
		if (actor->GetName() == name)
		{
			Transform* transform = nullptr;
			auto count = actor->GetComponent<ModelRender>()->GetTransformCount();
			transform = actor->GetComponent<ModelRender>()->AddTransform();
			transform->SetPosition(0 + (5 * count), 0, 10 * actors.size());
			transform->SetScale(0.075f, 0.075f, 0.075f);
			actor->GetComponent<ModelRender>()->UpdateTransforms();
			//actor->Init(transform);
			//actors.emplace_back(actor);
			return;
		}
	}

	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->SetName(name);
	actor->AddComponent<ModelRender>();
	actor->GetComponent<ModelRender>()->SetShader(modelShader);
	actor->GetComponent<ModelRender>()->ReadMesh(path);
	actor->GetComponent<ModelRender>()->ReadMaterial(path);
	
	Transform* transform = nullptr;
	transform = actor->GetComponent<ModelRender>()->AddTransform();
	transform->SetPosition(0, 0, 10 * actors.size());
	transform->SetScale(0.075f, 0.075f, 0.075f);
	actor->GetComponent<ModelRender>()->UpdateTransforms();

	//actor->Init(transform);

	actors.emplace_back(actor);

}

void EditorHelper::LoadMaterial(const std::wstring & path)
{

}

void EditorHelper::LoadTerrain(const std::wstring & path)
{
	const auto name = "Terrain";

	for (auto actor : actors)
	{
		//같은 모델을 불러왔을 때
		if (actor->GetName() == name)
		{
			actor->GetRender<Terrain>()->SetTerrainMap(path);
			actor->GetRender<Terrain>()->brush = nullptr;
			actor->GetRender<Terrain>()->brush = Debug_new Brush(modelShader, actor->GetRender<Terrain>());
			return;
		}
	}
	//Shader* temp = Debug_new Shader(L"55_Brush.fxo");

	auto actor = Debug_new Actor();
	actor->SetName("Terrain");
	actor->AddRender<Terrain>(modelShader);
	actor->GetRender<Terrain>()->SetTerrainMap(path);
	actor->GetRender<Terrain>()->brush = Debug_new Brush(modelShader, actor->GetRender<Terrain>());
	actor->Init(actor->GetRender<Terrain>()->GetTransform());
	actors.emplace_back(actor);

}

void EditorHelper::LoadTerrainLOD(const std::wstring & path)
{
	const auto name = "TerrainLOD";

	for (auto actor : actors)
	{
		//같은 모델을 불러왔을 때
		if (actor->GetName() == name)
		{
			actor->GetRender<TerrainLOD>()->SetTerrainMap(path);
			return;
		}
	}

	auto actor = Debug_new Actor();
	actor->SetName("TerrainLOD");
	actor->AddRender<TerrainLOD>(terrainLODShader);
	actor->GetRender<TerrainLOD>()->SetTerrainMap(path);
	actor->Init(actor->GetRender<TerrainLOD>()->GetTransform());

	actors.emplace_back(actor);
}

void EditorHelper::LoadParticle(const std::wstring & path)
{
	auto name = FileSystem::GetFileNameFromPath(FileSystem::ToString(path));
	
	auto actor = Debug_new Actor();
	
	int a = 0;
	for (auto actor : actors)
	{
		if (actor->GetName() == name + std::to_string(a))
		{
			a++;
			continue;
		}
	}
	name += std::to_string(a);
	actor->SetName(name);

	actor->AddRender<Particle>(nullptr);
	actor->GetRender<Particle>()->GetParticleFile(path);
	auto transform = actor->GetRender<Particle>()->GetTransform();
	actor->Init(transform);

	actors.emplace_back(actor);
}

void EditorHelper::LoadRain(const std::wstring& path)
{
	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->AddRender<Rain>(nullptr);
	actor->GetRender<Rain>()->SetRainShader(path);
	auto name = FileSystem::GetFileNameFromPath(FileSystem::ToString(path));
	int a = 0;
	for (auto actor : actors)
	{
		if (actor->GetName() == name + std::to_string(a))
		{
			a++;
			continue;
		}
	}
	name += std::to_string(a);
	actor->SetName(name);

	auto transform = actor->GetRender<Rain>()->GetTransform();
	actor->Init(transform);

	actors.emplace_back(actor);

}

void EditorHelper::LoadSnow(const std::wstring & path)
{
	auto actor = Debug_new Actor();
	actor->CreateActor();
	actor->AddRender<Snow>(nullptr);
	actor->GetRender<Snow>()->SetSnowShader(path);
	auto name = FileSystem::GetFileNameFromPath(FileSystem::ToString(path));
	int a = 0;
	for (auto actor : actors)
	{
		if (actor->GetName() == name + std::to_string(a))
		{
			a++;
			continue;
		}
	}
	name += std::to_string(a);
	actor->SetName(name);

	auto transform = actor->GetRender<Snow>()->GetTransform();
	actor->Init(transform);

	actors.emplace_back(actor);

}

void EditorHelper::LoadSpotLight()
{
	SpotLight light;

	auto actor = Debug_new Actor();
	std::string name = "SpotLight";
	int a = 0;
	for (auto actor : actors)
	{
		if (actor->GetName() == name + std::to_string(a))
		{
			a++;
			continue;
		}
	}
	name += std::to_string(a);
	actor->SetName(name);
	actor->AddComponent<Lighting>();
	auto transform = Debug_new Transform();
	transform->SetRotationFromDirection(0, 0, 0);
	actor->Init(transform);
	actor->GetTransform()->SetPosition(0.0f, 2.0f, 0.0f);

	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),						//Ambient
		Color(0.7f, 1.0f, 0.0f, 1.0f),						//Diffuse
		Color(0.3f, 1.0f, 0.0f, 1.0f),						//Specular
		Color(0.3f, 1.0f, 0.0f, 1.0f),						//Emissive
		actor->GetTransform()->GetPosition(), 25.0f,		//Position, Range
		actor->GetTransform()->GetForward(), 30.0f, 0.9f	//Direction, Angle, Intensity
	};
	Lighting::Get()->AddSpotLight(light);

	
	actors.emplace_back(actor);

}

void EditorHelper::LoadPointLight()
{
	PointLight light;

	auto actor = Debug_new Actor();
	std::string name = "PointLight";
	int a = 0;
	for (auto actor : actors)
	{
		if (actor->GetName() == name + std::to_string(a))
		{
			a++;
			continue;
		}
	}
	name += std::to_string(a);
	actor->SetName(name);
	actor->AddComponent<Lighting>();
	auto transform = Debug_new Transform();
	actor->Init(transform);
	actor->GetTransform()->SetPosition(0.0f, 2.0f, 0.0f);
	actor->GetTransform()->SetScale(25.0f, 25.0f, 25.0f);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),			//Ambient
		Color(0.0f, 0.0f, 1.0f, 1.0f),			//Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f),			//Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f),			//Emissive
		actor->GetTransform()->GetPosition(),	//Poistion
		25.0f, 0.3f								//Range, Intensity
	};
	Lighting::Get()->AddPointLight(light);


	actors.emplace_back(actor);


}

void EditorHelper::LoadWater(const std::wstring & path)
{
	const auto name = "Water";

	for (auto actor : actors)
	{
		//같은 모델을 불러왔을 때
		if (actor->GetName() == name)
		{
			actor->GetRender<Water>()->SetWater(path);
			return;
		}
	}

	auto actor = Debug_new Actor();
	actor->SetName(name);
	actor->AddRender<Water>(modelShader);
	actor->GetRender<Water>()->SetWater(path);
	actor->Init(actor->GetRender<Water>()->GetTransform());
	actors.emplace_back(actor);

}

void EditorHelper::PostEffectInit()
{
	//
	auto actor = Debug_new Actor();
	actor->SetName("Default");
	actor->AddRender<PostEffect>(nullptr);
	actor->GetRender<PostEffect>()->SetShader(L"47_Bloom.fxo");
	actor->GetRender<PostEffect>()->Temp();
	actor->Init(actor->GetRender<PostEffect>()->GetTransform());
	actor->GetRender<PostEffect>()->SRV(renderTarget->GetSRV());
	actors.emplace_back(actor);

	actor = Debug_new Actor();
	actor->SetName("BloomEffect");
	actor->AddRender<PostEffect>(nullptr);
	actor->GetRender<PostEffect>()->SetShader(L"47_Bloom.fxo");
	actor->GetRender<PostEffect>()->Temp();
	actor->Init(actor->GetRender<PostEffect>()->GetTransform());
	actor->GetRender<PostEffect>()->SRV(renderTarget->GetSRV());
	actors.emplace_back(actor);

	actor = Debug_new Actor();
	actor->SetName("BlurEffect");
	actor->AddRender<PostEffect>(nullptr);
	actor->GetRender<PostEffect>()->SetShader(L"46_Blur.fxo");
	actor->GetRender<PostEffect>()->Temp();
	actor->Init(actor->GetRender<PostEffect>()->GetTransform());
	actor->GetRender<PostEffect>()->SRV(renderTarget->GetSRV());
	actors.emplace_back(actor);

	actor = Debug_new Actor();
	actor->SetName("ColorTone");
	actor->AddRender<PostEffect>(nullptr);
	actor->GetRender<PostEffect>()->SetShader(L"44_ColorTone.fxo");
	actor->GetRender<PostEffect>()->Temp();
	actor->Init(actor->GetRender<PostEffect>()->GetTransform());
	actor->GetRender<PostEffect>()->SRV(renderTarget->GetSRV());
	actors.emplace_back(actor);




}

EditorHelper::EditorHelper()
{
	Init();
}

EditorHelper::~EditorHelper()
{
	SAFE_DELETE(modelShader);
	
	for (auto model : actors)
		SAFE_DELETE(model);
	actors.clear();

	for (auto brt : bloomRenderTarget)
		SAFE_DELETE(brt);

	SAFE_DELETE(renderTarget);
	SAFE_DELETE(shadow);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(viewport);
	
	
	
	
	


}

void EditorHelper::Pass(UINT mesh, UINT model, UINT anim)
{
	for (auto actor : actors)
	{
		if(actor->HasComponent<MeshRender>())
			actor->GetComponent<MeshRender>()->Pass(mesh);

		if (actor->HasComponent<ModelRender>())
			actor->GetComponent<ModelRender>()->Pass(model);

		if (actor->HasComponent<ModelAnimator>())
			actor->GetComponent<ModelAnimator>()->Pass(anim);

		if (actor->HasRender<Terrain>())
			actor->GetRender<Terrain>()->Pass(6);

		if (actor->HasRender<Water>())
			actor->GetRender<Water>()->Pass(19);
	}

}

void EditorHelper::GetBlurParams(std::vector<float>& _weights, std::vector<Vector2>& _offsets, float _x, float _y, float _sum, UINT _blurCount)
{
	_weights.clear();
	_weights.assign(_blurCount, float());

	_offsets.clear();
	_offsets.assign(_blurCount, Vector2());

	_weights[0] = Math::Gaussian(0, _blurCount); //1
	_offsets[0] = Vector2(0, 0);

	_sum = _weights[0];
	for (UINT i = 0; i < _blurCount / 2; i++)
	{
		float weight = Math::Gaussian(float(i + 1), _blurCount);
		_weights[i * 2 + 1] = weight;
		_weights[i * 2 + 2] = weight;
		_sum += weight * 2;

		Vector2 offset = Vector2(_x, _y) * (i * 2.0f + 1.0f);
		_offsets[i * 2 + 1] = offset;
		_offsets[i * 2 + 2] = -offset;
	}

	for (UINT i = 0; i < _blurCount; i++)
		_weights[i] /= _sum;
}

