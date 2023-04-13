#pragma once

enum class RenderType : UINT
{
	Unknown,
	Cloud,
	Dome,
	Moon,
	Rain,
	Snow,
	Terrain,
	TerrainLOD,
	Water,
	Billboard,
	PostEffect,
	Render2D,
	Particle
};

struct RenderAttribute
{
	std::function<std::any()> getter;
	std::function<void(std::any)> setter;
};

class Renderer
{
public:
	template <typename T>
	static constexpr RenderType DeduceRenderType();

public:
	Renderer(Shader* _shader);
	Renderer(std::wstring _shaderFile);
	Renderer(class Actor* actor, class Transform* transform, Shader* _shader);
	//Renderer(class Actor* _actor, class Transform* _transform);
	virtual ~Renderer();

	Shader* GetShader() { return shader; }

	UINT& GetPass() { return pass; }
	void Pass(UINT _val) { pass = _val; }

	virtual void Update() = 0;
	virtual void Render() = 0;

	Transform* GetTransform() { return transform; }
	void SetTrasnform(Transform* _transform) { transform = _transform; }

	void SetShader(Shader* _shader);
	void SetShader(std::wstring _shaderFile);



private:
	void Init();

protected:
	void Topology(D3D11_PRIMITIVE_TOPOLOGY _val) { topology = _val; }

protected:
	Shader* shader;

	
	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	UINT vertexCount = 0;
	UINT indexCount = 0;

protected:
	bool bCreateShader = false;

	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT pass = 0;

	PerFrame* perFrame;

protected:
#define REGISTER_RENDERER_ATTRIBUTE_GET_SET(getter, setter, type)        RegisterRendererAttribute([this]() { return getter(); },    [this](const std::any& in) { setter(std::any_cast<type>(in)); });
#define REGISTER_RENDERER_ATTRIBUTE_VALUE_SET(value, setter, type)       RegisterRendererAttribute([this]() { return value; },       [this](const std::any& in) { setter(std::any_cast<type>(in)); });
#define REGISTER_RENDERER_ATTRIBUTE_VALUE_VALUE(value, type)             RegisterRendererAttribute([this]() { return value; },       [this](const std::any& in) { value = std::any_cast<type>(in); });

	void RegisterRendererAttribute(std::function<std::any(void)>&& getter, std::function<void(std::any)>&& setter);
	RenderType renderType;
	class Transform* transform;
	class Actor* actor;

private:
	UINT id;
	std::vector<RenderAttribute> attributes;

public:
	const UINT& GetID() const { return id; }
	class Actor* GetActor() const { return actor; }
	class Transform* GetTransform() const { return transform; }
	const RenderType& GetRenderType() const { return renderType; }

	void SetRemderType(const RenderType& type) { this->renderType = type; }

	auto GetRenderAttributes() const -> const std::vector<RenderAttribute>& { return attributes; }
	void SetRenderAttributes(const std::vector<RenderAttribute>& attributes);


};