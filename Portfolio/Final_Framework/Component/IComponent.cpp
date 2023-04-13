#include "Framework.h"
#include "IComponent.h"

IComponent::IComponent( Actor * actor, Transform * transform)
	: actor(actor)
	, transform(transform)
	, bEnabled(true)
{
	id = GUIDGenerator::Generate();
}

void IComponent::SetAttributes(const std::vector<Attribute>& attributes)
{
	for (UINT i = 0; i < static_cast<UINT>(this->attributes.size()); i++)
		this->attributes[i].setter(attributes[i].getter());
}

void IComponent::RegisterAttribute(std::function<std::any()>&& getter, std::function<void(std::any)>&& setter)
{
	Attribute attribute;
	attribute.getter = std::move(getter);
	attribute.setter = std::move(setter);
	attributes.emplace_back(attribute);
}

template <typename T>
constexpr ComponentType IComponent::DeduceComponentType()
{
	return ComponentType::Unknown;
}

#define REGISTER_COMPONENT(T, enumT) template<> ComponentType IComponent::DeduceComponentType<T>() { return enumT; }

REGISTER_COMPONENT(Camera, ComponentType::Camera)
REGISTER_COMPONENT(Transform, ComponentType::Transform)
REGISTER_COMPONENT(ModelAnimator, ComponentType::Animator)
REGISTER_COMPONENT(Lighting, ComponentType::Light)
REGISTER_COMPONENT(CubeSky, ComponentType::Skybox)
REGISTER_COMPONENT(Sky, ComponentType::Sky)
REGISTER_COMPONENT(Collider, ComponentType::Collider)
REGISTER_COMPONENT(ModelRender, ComponentType::ObjectModel)
REGISTER_COMPONENT(MeshRender, ComponentType::ObjectMesh)
REGISTER_COMPONENT(FreeCamera, ComponentType::FreeCamera)
REGISTER_COMPONENT(FallowCamera, ComponentType::FallowCamera)
