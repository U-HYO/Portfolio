#pragma once
#include "Systems/IExecute.h"

class MRTDemo : public IExecute
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

	void Billborads();

	void Pass(UINT _val);

	/////////////////////////////////////////////////////
	void Paladin();

private:
	Shader* shader;

	RenderTarget* renderTarget;
	RenderTarget* mrt[3];

	DepthStencil* depthStencil;
	Viewport* viewport;

	Render2D* render2D;
	PostEffect* postEffect;


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

	ModelRender* weapon;
	Transform* weaponTransform;

	Billboard* billboard;

	std::vector<MeshRender*> meshes;
	std::vector<ModelRender*> models;
	std::vector<ModelAnimator *> animators;


	ModelAnimator* paladin;






};

