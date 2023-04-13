#include "Framework.h"
#include "Lighting.h"

Lighting* Lighting::instance = NULL;

Lighting * Lighting::Get()
{
	assert(instance != NULL);

	return instance;
}

void Lighting::Create()
{
	assert(instance == NULL);

	instance = Debug_new Lighting();
}

void Lighting::Delete()
{
	SAFE_DELETE(instance);
}

Lighting::Lighting()
	:IComponent(nullptr, nullptr)
{
}

Lighting::Lighting(Actor * _actor, Transform * _transform)
	:IComponent(_actor, _transform)
{
}

Lighting::~Lighting()
{
	for (auto t : transformManage)
	{
		SAFE_DELETE(t);
	}
}

void Lighting::Update()
{
	for (UINT i = 0; i < pointLightCount; i++)
	{
		pointLightTransforms[i]->GetPosition(&pointLights[i].Position);
		pointLights[i].Range = pointLightTransforms[i]->GetScale().x;
	}
	
	for (UINT i = 0; i < spotLightCount; i++)
	{
		spotLightTransforms[i]->GetPosition(&spotLights[i].Position);
		//spotLightTransforms[i]->GetRotation(&spotLights[i].Direction);
		spotLights[i].Direction = spotLightTransforms[i]->GetForward();
	}
	


}

UINT Lighting::PointLights(OUT PointLight * _lights)
{
	memcpy(_lights, pointLights, sizeof(PointLight) * pointLightCount);

	return pointLightCount;
}

void Lighting::AddPointLight(PointLight & _light)
{
	pointLights[pointLightCount] = _light;
	pointLightCount++;

	Transform* t = Debug_new Transform();
	t->SetPosition(pointLights[pointLightCount - 1].Position);
	t->SetScale(pointLights[pointLightCount - 1].Range);
	pointLightTransforms.push_back(t);
	transformManage.push_back(t);
}

PointLight & Lighting::GetPointLight(UINT _index)
{
	return pointLights[_index];
}

Transform * Lighting::GetPointLightTransform(UINT _index)
{
	return pointLightTransforms[_index];
}


UINT Lighting::SpotLights(OUT SpotLight * _lights)
{
	memcpy(_lights, spotLights, sizeof(SpotLight) * spotLightCount);

	return spotLightCount;
}

void Lighting::AddSpotLight(SpotLight & _light)
{
	spotLights[spotLightCount] = _light;
	spotLightCount++;

	Transform* t = Debug_new Transform();
	t->SetPosition(spotLights[spotLightCount - 1].Position);
	spotLightTransforms.push_back(t);
	transformManage.push_back(t);
}

SpotLight & Lighting::GetSpotLight(UINT _index)
{
	return spotLights[_index];
}

Transform * Lighting::GetSpotLightTransform(UINT _index)
{
	return spotLightTransforms[_index];
}

void Lighting::OnInit()
{
}

void Lighting::OnStart()
{
}

void Lighting::OnUpdate()
{
	Update();
}

void Lighting::OnRender()
{
	
}

void Lighting::OnStop()
{
}

void Lighting::OnDestroy()
{
}
