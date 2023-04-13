#include "Framework.h"
#include "Reflection.h"

Reflection::Reflection(Shader * shader, Transform* transform, float width, float height)
	: shader(shader), transform(transform)
{
	this->width = width > 0.0f ? width : (UINT)D3D::Width();
	this->height = height > 0.0f ? height : (UINT)D3D::Height();


	camera = Debug_new Fixity();
	renderTarget = Debug_new RenderTarget(this->width, this->height);
	depthStencil = Debug_new DepthStencil(this->width, this->height);
	viewport = Debug_new Viewport(this->width, this->height);

	sReflectionSRV = shader->AsSRV("ReflectionMap");
	sReflectionView = shader->AsMatrix("ReflectionView");
}

Reflection::~Reflection()
{
	SAFE_DELETE(camera);
	SAFE_DELETE(renderTarget);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(viewport);
}

void Reflection::Update()
{
	Vector3 R, T;
	//Context::Get()->GetCamera(0)->Rotation(&R);
	//Context::Get()->GetCamera(0)->Position(&T);

	Context::Get()->GetCamera(0)->GetTransform()->GetRotation(&R);
	Context::Get()->GetCamera(0)->GetTransform()->GetPosition(&T);

	R.x *= -1.0f;
	camera->GetTransform()->SetRotation(R);

	Vector3 position;
	transform->GetPosition(&position);

	T.y = (position.y * 2.0f) - T.y;
	camera->GetTransform()->SetPosition(T);
	camera->UpdateMove();
}

void Reflection::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();


	Matrix view;
	camera->GetMatrix(&view);
	sReflectionView->SetMatrix(view);


	Vector3 position;
	transform->GetPosition(&position);
	Plane plane = Plane(0, 1, 0, -position.y);

	Context::Get()->GetClipping() = plane;
}

void Reflection::Render()
{
	sReflectionSRV->SetResource(renderTarget->GetSRV());
}
