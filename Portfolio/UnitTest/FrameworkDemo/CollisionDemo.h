#pragma once
#include "Systems/IExecute.h"

class CollisionDemo : public IExecute
{
	// IExecute��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Release() override {};
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	Shader* shader;

	Material* floor;
	MeshRender* plane;

	Transform* transform[2];
	Collider* collider[2];



};

