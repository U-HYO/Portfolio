#include "Framework.h"
#include "PostEffect.h"

PostEffect::PostEffect(std::wstring shaderFile)
	: Renderer(shaderFile)
{
	Vertex vertices[6];
	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertexBuffer = Debug_new VertexBuffer(vertices, 6, sizeof(Vertex));
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

PostEffect::PostEffect(Actor * _actor, Transform * _transform, Shader * _shader)
	:Renderer(_actor, _transform, _shader)
{

}

PostEffect::~PostEffect()
{
	
}

void PostEffect::Update()
{
	perFrame->Update();
	transform->Update();


}

void PostEffect::Render()
{
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Render();

		if (indexBuffer != NULL)
			indexBuffer->Render();
	}

	D3D::GetDC()->IASetPrimitiveTopology(topology);
	perFrame->Render();
	transform->Render();

	
	shader->Draw(0, GetPass(), 6);
}

void PostEffect::SRV(ID3D11ShaderResourceView * srv)
{
	sDiffuseMap->SetResource(srv);
}

void PostEffect::Temp()
{
	Vertex vertices[6];
	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertexBuffer = Debug_new VertexBuffer(vertices, 6, sizeof(Vertex));
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}
