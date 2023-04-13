#include "Framework.h"
#include "Renderer.h"

Renderer::Renderer(Shader * _shader)
	: shader(_shader), bCreateShader(false)
{
	Init();
}

Renderer::Renderer(std::wstring _shaderFile)
	: bCreateShader(true)
{
	shader = Debug_new Shader(_shaderFile);

	Init();
}

Renderer::Renderer(Actor * actor, Transform * transform, Shader* _shader)
	: actor(actor)
	, transform(transform)
	, shader(_shader)
{
	if (shader)
		Init();
}

Renderer::~Renderer()
{
	SAFE_DELETE(perFrame);
	SAFE_DELETE(transform);

	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);

	if (bCreateShader == true)
		SAFE_DELETE(shader);
}

//Renderer::Renderer(Actor * _actor, Transform * _transform)
//	:IComponent(_actor, _transform)
//{
//}

void Renderer::SetShader(Shader * _shader)
{
	shader = _shader;
	bCreateShader = false;
	Init();
}

void Renderer::SetShader(std::wstring _shaderFile)
{
	shader = Debug_new Shader(_shaderFile);
	bCreateShader = true;

	Init();
}

void Renderer::Init()
{
	id = GUIDGenerator::Generate();
	perFrame = Debug_new PerFrame(shader);
	transform = Debug_new Transform(shader);
}

void Renderer::SetRenderAttributes(const std::vector<RenderAttribute>& attributes)
{
	for (UINT i = 0; i < static_cast<UINT>(this->attributes.size()); i++)
		this->attributes[i].setter(attributes[i].getter());
}

//void Renderer::Update()
//{
//	perFrame->Update();
//	transform->Update();
//}

//void Renderer::Render()
//{
//	if (vertexBuffer != NULL)
//	{
//		vertexBuffer->Render();
//
//		if (indexBuffer != NULL)
//			indexBuffer->Render();
//	}
//
//	D3D::GetDC()->IASetPrimitiveTopology(topology);
//	perFrame->Render();
//	transform->Render();
//}

void Renderer::RegisterRendererAttribute(std::function<std::any()>&& getter, std::function<void(std::any)>&& setter)
{
	RenderAttribute attribute;
	attribute.getter = std::move(getter);
	attribute.setter = std::move(setter);
	attributes.emplace_back(attribute);
}

template<typename T>
constexpr RenderType Renderer::DeduceRenderType()
{
	return RenderType::Unknown;
}


#define REGISTER_RENDER(T, enumT) template<> RenderType Renderer::DeduceRenderType<T>() { return enumT; }

REGISTER_RENDER(Cloud, RenderType::Cloud)
REGISTER_RENDER(Dome, RenderType::Dome)
REGISTER_RENDER(Moon, RenderType::Moon)
REGISTER_RENDER(Rain, RenderType::Rain)
REGISTER_RENDER(Snow, RenderType::Snow)
REGISTER_RENDER(Terrain, RenderType::Terrain)
REGISTER_RENDER(TerrainLOD, RenderType::TerrainLOD)
REGISTER_RENDER(Water, RenderType::Water)
REGISTER_RENDER(Billboard, RenderType::Billboard)
REGISTER_RENDER(PostEffect, RenderType::PostEffect)
REGISTER_RENDER(Render2D, RenderType::Render2D)
REGISTER_RENDER(Particle, RenderType::Particle )