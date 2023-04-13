#pragma once
#include "Systems/IExecute.h"

class BillboardDemo : public IExecute
{
	// IExecute을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Release() override;
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

	void Pass(UINT _val);

	/////////////////////////////////////////////////////
	void Paladin();

	void Miki();

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

	std::vector<MeshRender*> meshes;
	std::vector<ModelRender*> models;
	std::vector<ModelAnimator *> animators;


	ModelAnimator* paladin;

	ModelRender* miki;





};

