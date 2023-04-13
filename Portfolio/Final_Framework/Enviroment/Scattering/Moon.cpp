#include "Framework.h"
#include "Moon.h"

Moon::Moon(Shader * shader)
	: Renderer(shader)
	, distance(95), glowDistance(90)
{
	moon = Debug_new Texture(L"Environment/Moon.png");
	moonGlow = Debug_new Texture(L"Environment/MoonGlow.png");

	sMoon = shader->AsSRV("MoonMap");
	sAlpha = shader->AsScalar("MoonAlpha");


	VertexTexture vertices[6];

	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = Debug_new VertexBuffer(vertices, 6, sizeof(VertexTexture));
}

Moon::~Moon()
{
	SAFE_DELETE(moon);
	SAFE_DELETE(moonGlow);
	
}

void Moon::Update()
{
	perFrame->Update();
	transform->Update();
}

void Moon::Render(float theta)
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sAlpha->SetFloat(GetAlpha(theta));

	//Moon
	{
		Matrix W = GetTransform(theta);
		transform->SetWorld(W);

		if (vertexBuffer != NULL)
		{
			vertexBuffer->Render();

			if (indexBuffer != NULL)
				indexBuffer->Render();
		}

		D3D::GetDC()->IASetPrimitiveTopology(topology);
		perFrame->Render();
		transform->Render();


		sMoon->SetResource(moon->SRV());
		shader->Draw(0, GetPass(), 6);
	}

	//Glow
	{
		Matrix W = GetGlowTransform(theta);
		transform->SetWorld(W);

		if (vertexBuffer != NULL)
		{
			vertexBuffer->Render();

			if (indexBuffer != NULL)
				indexBuffer->Render();
		}

		D3D::GetDC()->IASetPrimitiveTopology(topology);
		perFrame->Render();
		transform->Render();

		sMoon->SetResource(moonGlow->SRV());
		shader->Draw(0, GetPass(), 6);
	}
}

float Moon::GetAlpha(float theta)
{
	if (theta < Math::PI * 0.5f || theta > Math::PI * 1.5f)
		return fabsf(sinf(theta + Math::PI / 2.0f));

	return 0.0f;
}

D3DXMATRIX Moon::GetTransform(float theta)
{
	Vector3 position;
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->GetPosition(&position);


	Matrix S, R, T, D;
	D3DXMatrixScaling(&S, 4, 4, 1);
	D3DXMatrixRotationYawPitchRoll(&R, Math::PI * 0.5f, theta - (Math::PI * 0.5f), 0);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	Vector3 direction = Lighting::Get()->GetDirection();
	D3DXMatrixTranslation
	(
		&D
		, direction.x * distance
		, direction.y * distance
		, direction.z * distance
	);

	return S * R * T * D;
}

D3DXMATRIX Moon::GetGlowTransform(float theta)
{
	D3DXVECTOR3 position;
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->GetPosition(&position);

	D3DXMATRIX S, R, T, D;
	D3DXMatrixScaling(&S, 12, 12, 1);
	D3DXMatrixRotationYawPitchRoll(&R, Math::PI * 0.5f, theta - (Math::PI * 0.5f), 0);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	D3DXVECTOR3 direction = Lighting::Get()->GetDirection();
	D3DXMatrixTranslation
	(
		&D
		, direction.x * glowDistance
		, direction.y * glowDistance
		, direction.z * glowDistance
	);

	return S * R * T * D;
}