#pragma once
#include "Systems/IExecute.h"

//���� �о ��ȯ��
class Viewer : public IExecute
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
	void Mesh();

private:
	Particle* particle;

	Shader* shader;
	CubeSky* sky;

	Material* floor;
	Material* stone;

	MeshRender* sphere;
	MeshRender* grid;
};