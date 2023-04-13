#pragma once

enum class ComponentType : UINT
{
	Unknown,
	Camera,
	FreeCamera,
	FallowCamera,
	Skybox,
	Sky,
	Transform,
	Renderable,
	Animator,
	Light,
	Collider,
	Particle,
	ObjectModel,
	ObjectMesh,
	Weather,
	Water,

};

struct Attribute
{
	std::function<std::any()> getter;
	std::function<void(std::any)> setter;
};

class IComponent
{
public:
	template <typename T>
	static constexpr ComponentType DeduceComponentType();

public:
	IComponent
	(
		class Actor* actor,
		class Transform* transform
	);
	virtual ~IComponent() = default;

	
	virtual void OnInit() = 0;
	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnStop() = 0;
	virtual void OnDestroy() = 0;

	const UINT& GetID() const { return id; }
	class Actor* GetActor() const { return actor; }
	class Transform* GetTransform() const { return transform; }
	const ComponentType& GetComponentType() const { return componentType; }
	const bool& IsEnabled() const { return bEnabled; }

	void SetComponentType(const ComponentType& type) { this->componentType = type; }
	void SetIsEnabled(const bool& bEnabled) { this->bEnabled = bEnabled; }

	auto GetAttributes() const -> const std::vector<Attribute>& { return attributes; }
	void SetAttributes(const std::vector<Attribute>& attributes);

protected:
#define REGISTER_ATTRIBUTE_GET_SET(getter, setter, type)        RegisterAttribute([this]() { return getter(); },    [this](const std::any& in) { setter(std::any_cast<type>(in)); });
#define REGISTER_ATTRIBUTE_VALUE_SET(value, setter, type)       RegisterAttribute([this]() { return value; },       [this](const std::any& in) { setter(std::any_cast<type>(in)); });
#define REGISTER_ATTRIBUTE_VALUE_VALUE(value, type)             RegisterAttribute([this]() { return value; },       [this](const std::any& in) { value = std::any_cast<type>(in); });

	void RegisterAttribute(std::function<std::any(void)>&& getter, std::function<void(std::any)>&& setter);

protected:
	class Actor* actor;
	class Transform* transform;
	ComponentType componentType;
	bool bEnabled;

private:
	UINT id;
	std::vector<Attribute> attributes;
};