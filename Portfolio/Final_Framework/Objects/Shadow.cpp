#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(Shader * shader, Vector3 & position, float radius, float width, float height)
	:shader(shader), position(position), radius(radius), width(width), height(height)
{
	renderTarget = Debug_new RenderTarget(width, height);
	depthStencil = Debug_new DepthStencil(width, height);
	viewport = Debug_new Viewport(width, height);

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Shadow");

	sShadowMap = shader->AsSRV("ShadowMap");

	//Create PCF Sampler
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //깊이기준을 어떻게 할 것인가
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MaxAnisotropy = 1;
		desc.MaxLOD = FLT_MAX;

		Check(D3D::GetDevice()->CreateSamplerState(&desc, &pcfSampler));
		sPcfSampler = shader->AsSampler("ShadowPcfSampler");
	}

	desc.MapSize = Vector2(width, height);
}

Shadow::~Shadow()
{
	SAFE_DELETE(renderTarget);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(viewport);

	SAFE_DELETE(buffer);

	SAFE_RELEASE(pcfSampler);
}

void Shadow::PreRender()
{
	//ImGui::InputInt("Quality", (int*)&desc.Quality);
	//desc.Quality %= 3;
	//
	//ImGui::SliderFloat("Bias", &desc.Bias, -0.001f, +0.001f, "%.4f");
	//
	//ImGui::SliderFloat3("Light Direction", Lighting::Get()->GetDirection(), -1, +1);
	//
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Vector3 up = Vector3(0, 1, 0);
	Vector3 direction = Lighting::Get()->GetDirection();
	Vector3 position = direction * radius * -2.0f; // 위치이자 방향
	position += this->position;

	D3DXMatrixLookAtLH(&desc.View, &position, &this->position, &up);


	Vector3 origin;
	D3DXVec3TransformCoord(&origin, &this->position, &desc.View);

	float l = origin.x - radius;
	float b = origin.y - radius;
	float n = origin.z - radius;

	float r = origin.x + radius;
	float t = origin.y + radius;
	float f = origin.z + radius;

	D3DXMatrixOrthoLH(&desc.Projection, r - l, t - b, n, f);
	//D3DXMatrixPerspectiveLH(&desc.Projection, )
	

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	sShadowMap->SetResource(depthStencil->SRV());
	sPcfSampler->SetSampler(0, pcfSampler);
}