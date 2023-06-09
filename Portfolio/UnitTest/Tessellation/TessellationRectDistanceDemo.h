#pragma once
#include "Systems/IExecute.h"

class TessellationRectDistanceDemo : public IExecute
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
	struct Vertex
	{
		Vector3 Position;


	};
private:
	Shader* shader;
	Transform* transform;
	PerFrame* perFrame;

	VertexBuffer* vertexBuffer;
	

};