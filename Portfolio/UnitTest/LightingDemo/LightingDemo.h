#pragma once
#include "Systems/IExecute.h"

class LightingDemo : public IExecute
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
	void Paladin();
	void Weapon();


	void Pass(UINT _val);

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

	ModelAnimator* paladin;
	ModelRender* weapon;
	Transform* weaponTransform;

	std::vector<MeshRender*> meshes;
	std::vector<ModelRender*> models;
	std::vector<ModelAnimator *> animators;


};

