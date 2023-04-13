#pragma once
#include "Systems/IExecute.h"

class WaterDemo : public IExecute
{
	virtual void Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	void Mesh();
	void Airplane();

	void Paladin();
	void Weapon();

	void PointLights();
	void SpotLights();

	void Pass(UINT mesh, UINT model, UINT anim);
	void SetTransform(Transform* transform);

private:
	Shader* shader;
	Shadow* shadow;
	Water* water;

	Sky* sky;
	Terrain* terrain;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* cylinder;
	MeshRender* sphere;

	ModelRender* airplane = NULL;

	ModelAnimator* paladin = NULL;
	ModelRender* weapon = NULL;
	Transform* weaponTransform;

	std::vector<MeshRender *> meshes;
	std::vector<ModelRender *> models;
	std::vector<ModelAnimator *> animators;

	Shader* billboardShader;
	Billboard* billboard;

};