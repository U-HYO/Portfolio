#pragma once
#include "Systems/IExecute.h"

class WeatherDemo : public IExecute
{
	// IExecute을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Release() override {};
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Mesh();
	void Airplane();
	void Kachujin();
	void Weapon();

	void PointLights();
	void SpotLights();

	void Billborads();
	void Weather();


	void Pass(UINT _val);

	/////////////////////////////////////////////////////
	void Paladin();

	void PaladinMove();
private:
	Shader* shader;
	CubeSky* sky;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* plane;
	MeshRender* sphere;
	MeshRender* cylinder;

	ModelRender* airplane;

	ModelAnimator* kachujin;
	ModelRender* weapon;
	Transform* weaponTransform;

	Billboard* billboard;
	
	enum class WeatherType
	{
		None = 0, Rain, Snow, Count,
	} weatherType;

	Rain* rain;
	Snow* snow;

	std::vector<MeshRender*> meshes;
	std::vector<ModelRender*> models;
	std::vector<ModelAnimator *> animators;


	ModelAnimator* paladin;




};

