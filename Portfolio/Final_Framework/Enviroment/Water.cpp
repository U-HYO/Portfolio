#include "Framework.h"
#include "Water.h"

Water::Water(InitDesc & initDesc)
	:Renderer(initDesc.Shader), initDesc(initDesc)
{
	vertexCount = 4;
	indexCount = 6;

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[1].Position = Vector3(-initDesc.Radius, 0.0f, +initDesc.Radius);
	vertices[2].Position = Vector3(+initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[3].Position = Vector3(+initDesc.Radius, 0.0f, +initDesc.Radius);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };
	//바다는 정점으로 파도를 표현

	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);


	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Water");


	this->initDesc.Width = initDesc.Width > 0 ? initDesc.Width : D3D::Width();
	this->initDesc.Height = initDesc.Height > 0 ? initDesc.Height : D3D::Height();

	reflection = Debug_new Reflection(shader, transform, this->initDesc.Width, this->initDesc.Height);
	refraction = Debug_new Refraction(shader, transform, L"Environment/Wave.dds", this->initDesc.Width, this->initDesc.Height);

	heightMap = Debug_new Texture(initDesc.HeightFile);
	sHeightMap = shader->AsSRV("HeightMap");

	waterMap = Debug_new Texture(L"Environment/Water.png");
	shader->AsSRV("WaterMap")->SetResource(waterMap->SRV());

}

Water::Water(Actor * _actor, Transform * _transform, Shader * _shader)
	: Renderer(_actor, _transform, _shader)
{
	buffer = nullptr;
	sBuffer = nullptr;
	reflection = nullptr;
	refraction = nullptr;
	heightMap = nullptr;
	sHeightMap = nullptr;
	waterMap = nullptr;
}

Water::~Water()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE(reflection);
	SAFE_DELETE(refraction);

	SAFE_DELETE(heightMap);

	SAFE_DELETE(waterMap)
}

void Water::Update()
{
	perFrame->Update();
	transform->Update();
	desc.WaveTranslation += desc.WaveSpeed * Time::Delta();
	if (desc.WaveTranslation > 1.0f)
		desc.WaveTranslation -= 1.0f;

	ImGui::DragFloat("WaveScale", &desc.WaveScale, 0.01f, 0.0f, 10.0f);

	reflection->Update();
	refraction->Update();
}

void Water::SetWater(std::wstring path)
{
	initDesc.Radius = 125;
	initDesc.HeightFile = L"Terrain/Gray256.png";
	initDesc.Width = 0;
	initDesc.Height = 0;

	vertexCount = 4;
	indexCount = 6;

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[1].Position = Vector3(-initDesc.Radius, 0.0f, +initDesc.Radius);
	vertices[2].Position = Vector3(+initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[3].Position = Vector3(+initDesc.Radius, 0.0f, +initDesc.Radius);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };
	//바다는 정점으로 파도를 표현

	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);


	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Water");


	this->initDesc.Width = initDesc.Width > 0 ? initDesc.Width : D3D::Width();
	this->initDesc.Height = initDesc.Height > 0 ? initDesc.Height : D3D::Height();

	reflection = Debug_new Reflection(shader, transform, this->initDesc.Width, this->initDesc.Height);
	refraction = Debug_new Refraction(shader, transform, L"Environment/Wave.dds", this->initDesc.Width, this->initDesc.Height);
	
	heightMap = Debug_new Texture(initDesc.HeightFile);
	sHeightMap = shader->AsSRV("HeightMap");

	waterMap = Debug_new Texture(path, false);
	shader->AsSRV("WaterMap")->SetResource(waterMap->SRV());

}

void Water::PreRender_Reflection()
{
	reflection->PreRender();
}

void Water::PreRender_Refraction()
{
	refraction->PreRender();
}

void Water::Render()
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

	reflection->Render();
	refraction->Render();


	desc.TerrainWidth = (float)heightMap->GetWidth();
	desc.TerrainHeight = (float)heightMap->GetHeight();


	Vector3 scale;
	GetTransform()->GetScale(&scale);

	desc.WaterWidth = scale.x * initDesc.Radius * 2;
	desc.WaterHeight = scale.y * initDesc.Radius * 2;


	Vector3 position;
	GetTransform()->GetPosition(&position);

	desc.WaterPositionY = position.y;


	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	sHeightMap->SetResource(heightMap->SRV());

	shader->DrawIndexed(0, GetPass(), indexCount);
}
