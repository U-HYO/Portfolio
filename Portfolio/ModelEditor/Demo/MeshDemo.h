#pragma once
#include "Systems/IExecute.h"

//���� �о ��ȯ��
class MeshDemo : public IExecute
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

	Material* planeMaterial;
	MeshRender* plane;

	Material* boxMaterial;
	MeshRender* box;

};
