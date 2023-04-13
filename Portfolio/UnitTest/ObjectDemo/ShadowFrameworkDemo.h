#pragma once
#include "Systems/IExecute.h"

class ShadowFrameworkDemo : public IExecute
{
	// IExecute을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Mesh();
	void Airplane();
	void Weapon();

	void PointLights();
	void SpotLights();


	void Pass(UINT _val);

	/////////////////////////////////////////////////////
	void Paladin();


private:
	Shader* shader;
	Shadow* shadow;

	CubeSky* sky;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* plane;
	MeshRender* cylinder;
	MeshRender* sphere;

	ModelRender* airplane;

	ModelAnimator* paladin;
	ModelRender* weapon;
	Transform* weaponTransform;

	std::vector<MeshRender *> meshes;
	std::vector<ModelRender *> models;
	std::vector<ModelAnimator *> animators;

};