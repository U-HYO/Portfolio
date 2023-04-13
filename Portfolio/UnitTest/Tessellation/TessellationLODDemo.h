#pragma once
#include "Systems/IExecute.h"

class TessellationLODDemo : public IExecute
{
	// IExecute��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:


private:
	Shader* shader;

	//TerrainLOD* terrain;
	Renderer* terrain;
	
};