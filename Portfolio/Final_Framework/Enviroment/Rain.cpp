#include "Framework.h"
#include "Rain.h"

Rain::Rain(Vector3 & _extent, UINT _count, std::wstring _file)
	:Renderer(L"40_Rain.fxo"), drawCount(_count)
{
	desc.Extent = _extent;
	desc.DrawDistance = desc.DrawDistance;

	texture = Debug_new Texture(_file);
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Rain");

	vertices = Debug_new VertexRain[drawCount * 4];
	for (UINT i = 0; i < drawCount * 4; i += 4)
	{
		Vector2 scale;
		scale.x = Math::Random(0.1f, 0.4f);
		scale.y = Math::Random(2.0f, 6.0f);

		Vector3 position;
		position.x = Math::Random(-desc.Extent.x, desc.Extent.x);
		position.y = Math::Random(-desc.Extent.y, desc.Extent.y);
		position.z = Math::Random(-desc.Extent.z, desc.Extent.z);

		vertices[i + 0].Position = position;
		vertices[i + 1].Position = position;
		vertices[i + 2].Position = position;
		vertices[i + 3].Position = position;

		vertices[i + 0].UV = Vector2(0, 1);
		vertices[i + 1].UV = Vector2(0, 0);
		vertices[i + 2].UV = Vector2(1, 1);
		vertices[i + 3].UV = Vector2(1, 0);

		vertices[i + 0].Scale = scale;
		vertices[i + 1].Scale = scale;
		vertices[i + 2].Scale = scale;
		vertices[i + 3].Scale = scale;
	}

	indices = Debug_new UINT[drawCount * 6];
	for (UINT i = 0; i < drawCount; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	vertexBuffer = Debug_new VertexBuffer(vertices, drawCount * 4, sizeof(VertexRain));
	indexBuffer = Debug_new IndexBuffer(indices, drawCount * 6);

	//D3D11_BLEND_DESC

}

Rain::Rain(Actor * _actor, Transform * _transform, Shader * _shader)
	:Renderer(_actor, _transform, _shader)
{
}

Rain::~Rain()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(texture);
}

void Rain::SetRainShader(std::wstring _file, Vector3 & _extent, UINT _count)
{
	SetShader(L"40_Rain.fxo");
	drawCount = _count;

	desc.Extent = _extent;
	desc.DrawDistance = desc.DrawDistance;

	texture = Debug_new Texture(_file, false);
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Rain");

	vertices = Debug_new VertexRain[drawCount * 4];
	for (UINT i = 0; i < drawCount * 4; i += 4)
	{
		Vector2 scale;
		scale.x = Math::Random(0.1f, 0.4f);
		scale.y = Math::Random(2.0f, 6.0f);

		Vector3 position;
		position.x = Math::Random(-desc.Extent.x, desc.Extent.x);
		position.y = Math::Random(-desc.Extent.y, desc.Extent.y);
		position.z = Math::Random(-desc.Extent.z, desc.Extent.z);

		vertices[i + 0].Position = position;
		vertices[i + 1].Position = position;
		vertices[i + 2].Position = position;
		vertices[i + 3].Position = position;

		vertices[i + 0].UV = Vector2(0, 1);
		vertices[i + 1].UV = Vector2(0, 0);
		vertices[i + 2].UV = Vector2(1, 1);
		vertices[i + 3].UV = Vector2(1, 0);

		vertices[i + 0].Scale = scale;
		vertices[i + 1].Scale = scale;
		vertices[i + 2].Scale = scale;
		vertices[i + 3].Scale = scale;
	}

	indices = Debug_new UINT[drawCount * 6];
	for (UINT i = 0; i < drawCount; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	vertexBuffer = Debug_new VertexBuffer(vertices, drawCount * 4, sizeof(VertexRain));
	indexBuffer = Debug_new IndexBuffer(indices, drawCount * 6);
}

void Rain::Update()
{
	//ImGui::Separator();
	//ImGui::SliderFloat3("Origin", desc.Origin, 0, 200);
	//
	//ImGui::SliderFloat3("Velocity", desc.Velocitty, -200, 200);
	//ImGui::ColorEdit3("Color", desc.Color);
	//ImGui::SliderFloat("Distance", &desc.DrawDistance, 0, desc.Extent.z * 2.0f);

	perFrame->Update();
	transform->Update();
}

void Rain::Render()
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


	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	shader->DrawIndexed(0, GetPass(), drawCount * 6);
}
