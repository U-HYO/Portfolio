#include "Framework.h"
#include "Scattering.h"

Scattering::Scattering(Shader * shader)
	: shader(shader), width(128), height(64)
{
	mieTarget = Debug_new RenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	rayleighTarget = Debug_new RenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	depthStencil = Debug_new DepthStencil(width, height);
	viewport = Debug_new Viewport((float)width, (float)height);

	render2D = Debug_new Render2D();
	render2D->GetTransform()->SetScale(200, 200, 1);
	render2D->GetTransform()->SetRotationDegree(0, 0, -90);

	CreateQuad();
}

Scattering::~Scattering()
{
	SAFE_DELETE(render2D);

	SAFE_DELETE(mieTarget);
	SAFE_DELETE(rayleighTarget);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(viewport);

	SAFE_DELETE(vertexBuffer);
}

void Scattering::Update()
{
	render2D->Update();
}

void Scattering::PreRender()
{
	RenderTarget* rtvs[2];
	rtvs[0] = rayleighTarget;
	rtvs[1] = mieTarget;
	RenderTarget::PreRender(rtvs, 2, depthStencil);

	viewport->RSSetViewport();


	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, pass, 6);
}

void Scattering::PostRender()
{
	render2D->SRV(rayleighTarget->GetSRV());
	render2D->GetTransform()->SetPosition(0 + 100, D3D::Height() - 100, 0);
	render2D->Update();
	render2D->Render();

	render2D->SRV(mieTarget->GetSRV());
	render2D->GetTransform()->SetPosition(200 + 100, D3D::Height() - 100, 0);
	render2D->Update();
	render2D->Render();
}

void Scattering::CreateQuad()
{
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