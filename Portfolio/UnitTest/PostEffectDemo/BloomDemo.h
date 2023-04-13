#pragma once
#include "Systems/IExecute.h"

class BloomDemo : public IExecute
{
	// IExecute��(��) ���� ��ӵ�
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
	void GetBlurParams(std::vector<float>& _weights, std::vector<Vector2>& _offsets, float _x, float _y);

private:
	Shader* shader;
	Shader* shader2;
	Shadow* shadow;

	RenderTarget* renderTarget[6];
	DepthStencil* depthStencil;
	Viewport* viewport;

	Render2D* render2D;
	PostEffect* postEffect;

	float threshold = 0.05f;
	UINT blurCount = 15;

	std::vector<float> weightX;
	std::vector<float> weightY;

	std::vector<Vector2> offsetX;
	std::vector<Vector2> offsetY;
	float sum = 0.0f;


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

