#pragma once
#include "Systems/IExecute.h"

//파일 읽어서 변환만
class AnimationDemo : public IExecute
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
	void Kachujin();
	void Paladin();

private:
	Shader* shader;

	ModelAnimator* kachujin;
	ColliderObject** colliderObject;

	Transform* weaponInitTransform;
	ModelRender* weapon;

	ModelAnimator* paladin;

};
