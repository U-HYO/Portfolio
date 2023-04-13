#include "Framework.h"
#include "Refraction.h"

Refraction::Refraction(Shader * shader, Transform* transform, std::wstring normalFile, float width, float height)
	: shader(shader), transform(transform)
{
	this->width = width > 0.0f ? width : (UINT)D3D::Width();
	this->height = height > 0.0f ? height : (UINT)D3D::Height();

	renderTarget = Debug_new RenderTarget(this->width, this->height);
	depthStencil = Debug_new DepthStencil(this->width, this->height);
	viewport = Debug_new Viewport(this->width, this->height);

	sRefractionSRV = shader->AsSRV("RefractionMap");

	normalMap = Debug_new Texture(normalFile);
	sNormalMap = shader->AsSRV("NormalMap");
}

Refraction::~Refraction()
{
	SAFE_DELETE(renderTarget);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(viewport);

	SAFE_DELETE(normalMap);
}

void Refraction::Update()
{

}

void Refraction::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Vector3 position;
	transform->GetPosition(&position);

	Plane plane = Plane(0, -1, 0, position.y + 0.5f); //윗면은 그리지않는다
	Context::Get()->GetClipping() = plane;
}

void Refraction::Render()
{
	sRefractionSRV->SetResource(renderTarget->GetSRV());
	sNormalMap->SetResource(normalMap->SRV());
}
