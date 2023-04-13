#include "Framework.h"
#include "Snow.h"

Snow::Snow(Vector3 & extent, UINT count, std::wstring file)
	: Renderer(L"40_Snow.fxo"), drawCount(count)
{
	desc.Extent = extent;
	desc.DrawDistance = desc.Extent.z * 2.0f;

	texture = Debug_new Texture(file);
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Snow");


	vertices = Debug_new VertexSnow[drawCount * 4];
	for (UINT i = 0; i < drawCount * 4; i += 4)
	{
		float scale;
		//scale = Math::Random(5.1f, 10.4f);
		scale = Math::Random(0.51f, 1.04f);

		Vector3 position;
		position.x = Math::Random(-desc.Extent.x, desc.Extent.x);
		position.y = Math::Random(-desc.Extent.y, desc.Extent.y);
		position.z = Math::Random(-desc.Extent.z, desc.Extent.z);

		Vector2 random = Math::RandomVec2(0.0f, 1.0f);


		vertices[i + 0].Position = position;
		vertices[i + 1].Position = position;
		vertices[i + 2].Position = position;
		vertices[i + 3].Position = position;

		vertices[i + 0].Uv = Vector2(0, 1);
		vertices[i + 1].Uv = Vector2(0, 0);
		vertices[i + 2].Uv = Vector2(1, 1);
		vertices[i + 3].Uv = Vector2(1, 0);

		vertices[i + 0].Scale = scale;
		vertices[i + 1].Scale = scale;
		vertices[i + 2].Scale = scale;
		vertices[i + 3].Scale = scale;

		vertices[i + 0].Random = random;
		vertices[i + 1].Random = random;
		vertices[i + 2].Random = random;
		vertices[i + 3].Random = random;
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

	vertexBuffer = Debug_new VertexBuffer(vertices, drawCount * 4, sizeof(VertexSnow));
	indexBuffer = Debug_new IndexBuffer(indices, drawCount * 6);

	//ID3D11BlendState
	//D3D11_BLEND_DESC;
}
Snow::Snow(Actor * _actor, Transform * _transform, Shader * _shader)
	:Renderer(_actor, _transform, _shader)
{
}

Snow::~Snow()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(texture);
}

void Snow::SetSnowShader(std::wstring _file, Vector3 & _extent, UINT _count)
{
	SetShader(L"40_Snow.fxo");
	drawCount = _count;

	desc.Extent = _extent;
	desc.DrawDistance = desc.Extent.z * 2.0f;

	texture = Debug_new Texture(_file, false);
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Snow");


	vertices = Debug_new VertexSnow[drawCount * 4];
	for (UINT i = 0; i < drawCount * 4; i += 4)
	{
		float scale;
		//scale = Math::Random(5.1f, 10.4f);
		scale = Math::Random(0.51f, 1.04f);

		Vector3 position;
		position.x = Math::Random(-desc.Extent.x, desc.Extent.x);
		position.y = Math::Random(-desc.Extent.y, desc.Extent.y);
		position.z = Math::Random(-desc.Extent.z, desc.Extent.z);

		Vector2 random = Math::RandomVec2(0.0f, 1.0f);


		vertices[i + 0].Position = position;
		vertices[i + 1].Position = position;
		vertices[i + 2].Position = position;
		vertices[i + 3].Position = position;

		vertices[i + 0].Uv = Vector2(0, 1);
		vertices[i + 1].Uv = Vector2(0, 0);
		vertices[i + 2].Uv = Vector2(1, 1);
		vertices[i + 3].Uv = Vector2(1, 0);

		vertices[i + 0].Scale = scale;
		vertices[i + 1].Scale = scale;
		vertices[i + 2].Scale = scale;
		vertices[i + 3].Scale = scale;

		vertices[i + 0].Random = random;
		vertices[i + 1].Random = random;
		vertices[i + 2].Random = random;
		vertices[i + 3].Random = random;
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

	vertexBuffer = Debug_new VertexBuffer(vertices, drawCount * 4, sizeof(VertexSnow));
	indexBuffer = Debug_new IndexBuffer(indices, drawCount * 6);
}

void Snow::Update()
{
	//ImGui::InputInt("Snow Pass", (int *)&GetPass());
	//GetPass() %= 4;
	//
	//ImGui::SliderFloat3("Velocity", desc.Velocity, -200, 200);
	//ImGui::ColorEdit3("Color", desc.Color);
	//ImGui::SliderFloat("Distance", &desc.DrawDistance, 0, desc.Extent.z * 2.0f);
	//ImGui::InputFloat("Turbulence", &desc.Turbulence, 0.1f);
	//
	//ImGui::Text(std::to_string(Time::Get()->Running()).c_str());

	perFrame->Update();
	transform->Update();
}

void Snow::Render()
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
