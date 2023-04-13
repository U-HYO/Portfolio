#pragma once
#include "Systems/IExecute.h"

enum class PaladinAnim : UINT
{
	BlockIdle = 0,
	Idle,
	Rolling,
	Run,
	Walk,
};

class PlayerDemo : public IExecute
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
	void Pass(UINT _val);

	/////////////////////////////////////////////////////
	void Paladin();
	void PaladinMove();


	//postEffect
	void PostEffectInit();

private:
	Shader* shader;
	CubeSky* sky;

	Material* floor;

	MeshRender* plane;

	//ModelRender* weapon;
	//Transform* weaponTransform;

	std::vector<MeshRender*> meshes;
	std::vector<ModelRender*> models;
	std::vector<ModelAnimator *> animators;


	ModelAnimator* paladin;

	/////////////////////////////
	Transform* dummyTransform; // Fallow Camera Target Dummy

	PaladinAnim modelState = PaladinAnim::Idle;

	PaladinAnim modelCurrState = PaladinAnim::Idle;
	PaladinAnim modelNextState = PaladinAnim::Idle;

private://PostEffect
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	PostEffect* postEffect;


};

