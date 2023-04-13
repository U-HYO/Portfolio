#pragma once
#include "IComponent.h"

class Actor
{
public:
	Actor();
	~Actor();

	void Init(class Transform* transform);
	void Clone();

	void Start();
	void Update();
	void Stop();

	void Render();

	class Actor* CreateActor();
	class Actor* CreateRender();

	auto GetID() const -> const UINT& { return id; }

	const std::string& GetName() const { return name; }
	void SetName(const std::string& name) { this->name = name; }

	const bool& IsActive() const { return bActive; }
	void SetIsActive(const bool& bActive) { this->bActive = bActive; }

	auto GetTransform() const -> class Transform* { return transform; }

	//////////////////Component//////////////////
	auto AddComponent(const ComponentType& type)->IComponent*;
	template <typename T> T* AddComponent();
	template <typename T> T* GetComponent();
	template <typename T> std::vector<T*> GetComponents();

	auto GetAllComponents() const -> const std::vector<IComponent*>& { return components; }

	const bool HasComponent(const ComponentType& type);

	int GetSelectionMask() { return selection_mask; }
	void SetSelectionMask(int mask) { selection_mask = mask; }

	template <typename T>
	const bool HasComponent()
	{
		return HasComponent(IComponent::DeduceComponentType<T>());
	}

	template <typename T>
	void RemoveComponent();
	//////////////////////////////////////////

	//////////////////Render//////////////////
	//auto AddRender(const RenderType& type)->Renderer*;
	template <typename T> T* AddRender(Shader* shader);
	template <typename T> T* GetRender();
	template <typename T> std::vector<T*> GetRenders();

	auto GetAllRenders() const -> const std::vector<Renderer*>& { return renders; }

	const bool HasRender(const RenderType& type);

	template <typename T>
	const bool HasRender()
	{
		return HasRender(Renderer::DeduceRenderType<T>());
	}

	template <typename T>
	void RemoveRender();
	//////////////////////////////////////////


private:
	class Transform* transform;
	std::string name;
	UINT id;
	bool bActive;
	int selection_mask = -1;

	std::vector<IComponent*> components;
	std::vector<Renderer*> renders;
};

template<typename T>
inline T * Actor::AddComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	//if (HasComponent(type) && type != ComponentType::Script)
	//	return GetComponent<T>();

	components.emplace_back
	(
		Debug_new T
		(
			this,
			transform
		)
	);

	auto newComponent = static_cast<T*>(components.back());
	newComponent->SetComponentType(type);
	newComponent->OnInit();

	return newComponent;
}

template<typename T>
inline T * Actor::GetComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	for (auto component : components)
	{
		if (component->GetComponentType() == type)
			return static_cast<T*>(component);
	}
	return nullptr;
}

template<typename T>
inline std::vector<T*> Actor::GetComponents()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	std::vector<T*> tempComponents;
	for (auto component : components)
	{
		if (component->GetComponentType() != type)
			continue;

		tempComponents.emplace_back(static_cast<T*>(component));
	}

	return tempComponents;
}

template<typename T>
inline void Actor::RemoveComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	for (auto iter = components.begin(); iter != components.end();)
	{
		auto component = *iter;
		if (component->GetComponentType() == type)
		{
			component->OnDestroy();
			SAFE_DELETE(component);
			iter = components.erase(iter);
		}
		else
			iter++;
	}
}

template<typename T>
inline T * Actor::AddRender(Shader* shader)
{
	RenderType type = Renderer::DeduceRenderType<T>();

	//if (HasComponent(type) && type != ComponentType::Script)
	//	return GetComponent<T>();

	renders.emplace_back
	(
		Debug_new T
		(
			this,
			transform,
			shader
		)
	);

	auto newRender = static_cast<T*>(renders.back());
	newRender->SetRemderType(type);

	return newRender;
}

template<typename T>
inline T * Actor::GetRender()
{
	RenderType type = Renderer::DeduceRenderType<T>();

	std::vector<T*> tempComponents;
	for (auto render : renders)
	{
		if (render->GetRenderType() == type)
			return static_cast<T*>(render);
	}

	return nullptr;
}

template<typename T>
inline std::vector<T*> Actor::GetRenders()
{
	RenderType type = Renderer::DeduceRenderType<T>();

	std::vector<T*> tempRenders;
	for (auto render : renders)
	{
		if (render->GetRenderType() != type)
			continue;

		tempRenders.emplace_back(static_cast<T*>(component));
	}

	return tempRenders;
}

template<typename T>
inline void Actor::RemoveRender()
{
	RenderType type = Renderer::DeduceRenderType<T>();

	for (auto iter = renders.begin(); iter != renders.end();)
	{
		auto renderer = *iter;
		if (renderer->GetRenderType() == type)
		{
			//component->OnDestroy();
			SAFE_DELETE(component);
			iter = renders.erase(iter);
		}
		else
			iter++;
	}
}
