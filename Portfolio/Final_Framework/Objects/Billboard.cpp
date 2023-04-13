#include "Framework.h"
#include "Billboard.h"

Billboard::Billboard(Shader* shader)
	: Renderer(shader)
{
	sDiffuseMap = shader->AsSRV("BillboardMaps");
	//sDiffuseMap = shader->AsSRV("DiffuseMap");
}


Billboard::~Billboard()
{
	SAFE_DELETE(textureArray);
}

void Billboard::Render()
{
	if (textureArray == nullptr)
		textureArray = Debug_new TextureArray(textureFiles);

	if (vertices.size() != vertexCount)
	{
		vertexCount = vertices.size();

		SAFE_DELETE(vertexBuffer);
		vertexBuffer = Debug_new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexBillboard));
	}


	if (vertexBuffer != NULL)
	{
		vertexBuffer->Render();

		if (indexBuffer != NULL)
			indexBuffer->Render();
	}

	D3D::GetDC()->IASetPrimitiveTopology(topology);
	perFrame->Render();
	transform->Render();

	sDiffuseMap->SetResource(textureArray->SRV());
	shader->Draw(0, GetPass(), vertexCount);
}

void Billboard::Update()
{
	perFrame->Update();
	transform->Update();
}

void Billboard::Add(Vector3 & _position, Vector2 & _scale, UINT mapIndex)
{
	vertices.push_back({ _position, _scale, mapIndex });
}

void Billboard::AddTexture(std::wstring file)
{
	SAFE_DELETE(textureArray);

	textureFiles.push_back(file);
}

