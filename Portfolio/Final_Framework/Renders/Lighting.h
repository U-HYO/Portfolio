#pragma once
#include "Component/IComponent.h"

#define MAX_POINT_LIGHTS 256
struct PointLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector3 Position;
	float Range;

	float Intensity;
	float Padding[3];
};

#define MAX_SPOT_LIGHTS 256
struct SpotLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector3 Position;
	float Range;

	Vector3 Direction;
	float Angle;

	float Intensity;
	float Padding[3];
};

class Lighting : public IComponent
{
public:
	static Lighting* Get();
	static void Create();
	static void Delete();
	
	Lighting(class Actor* _actor, class Transform* _transform);

private:
	Lighting();
//	Lighting(class Actor* _actor, class Transform* _transform);
	~Lighting();

public:
	void Update();

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnStop() override;
	void OnDestroy() override;

public:
	Color& GetAmbient() { return ambient; }
	Color& GetSpecular() { return specular; }
	Vector3& GetDirection() { return direction; }
	Vector3& GetPosition() { return position; }

public:
	UINT PointLightCount() { return pointLightCount; }
	UINT PointLights(OUT PointLight* _lights);
	void AddPointLight(PointLight& _light);
	PointLight& GetPointLight(UINT _index);
	Transform* GetPointLightTransform(UINT _index);

public:
	UINT SpotLightCount() { return spotLightCount; }
	UINT SpotLights(OUT SpotLight* _lights);
	void AddSpotLight(SpotLight& _light);
	SpotLight& GetSpotLight(UINT _index);
	Transform* GetSpotLightTransform(UINT _index);

public:
	Color& FogColor() { return fogColor; }
	Vector2& FogDistance() { return fogDistance; }
	float& FogDensity() { return fogDensity; }
	UINT& FogType() { return fogType; }

	void SetFogColor(Color& color) { fogColor = color; }
	void SetFogDistance(Vector2& distance) {fogDistance = distance; }
	void SetFogDensity(float& density) { fogDensity = density; }
	void SetFogType(UINT& type) { fogType = type; }

private:
	static Lighting* instance;

	Color ambient = Color(0, 0, 0, 1);
	Color specular = Color(1, 1, 1, 1);
	Vector3 direction = Vector3(-1, -1, +1);
	Vector3 position = Vector3(0, 0, 0);

private: //PointLight
	UINT pointLightCount = 0;
	PointLight pointLights[MAX_POINT_LIGHTS];
	std::vector<Transform*> pointLightTransforms;


private: //SpotLight
	UINT spotLightCount = 0;
	SpotLight spotLights[MAX_SPOT_LIGHTS];
	std::vector<Transform *> spotLightTransforms;

	std::vector<Transform*> transformManage;

private: // Fog Light
	Color fogColor = Color(0.75f, 0.75f, 0.75f, 1);
	Vector2 fogDistance = Vector2(1, 105);
	float fogDensity = 0.5f;
	UINT fogType = 2;

};