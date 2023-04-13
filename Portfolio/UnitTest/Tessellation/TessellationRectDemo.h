#pragma once
#include "Systems/IExecute.h"

class TessellationRectDemo : public IExecute
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
	struct Vertex
	{
		Vector3 Position;


	};
private:
	Shader* shader;

	VertexBuffer* vertexBuffer;
	

};