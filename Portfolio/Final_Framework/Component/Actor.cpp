#include "Framework.h"
#include "Actor.h"

Actor::Actor()
	: transform(nullptr)
	, name("")
	, bActive(true)
{
	id = GUIDGenerator::Generate();
}

Actor::~Actor()
{
	for (auto iter = components.begin(); iter != components.end(); iter++)
	{
		(*iter)->OnDestroy();
		SAFE_DELETE(*iter);
	}

	components.clear();

	for (auto iter = renders.begin(); iter != renders.end(); iter++)
	{
		SAFE_DELETE(*iter);
	}
	renders.clear();


}

void Actor::Init(Transform * transform)
{
	this->transform = transform;
}

void Actor::Clone()
{
	std::vector<Actor*> clones;

	auto clone_actor = [&](Actor* actor)
	{
		auto clone = CreateActor();
		clone->SetName(actor->GetName());
		clone->SetIsActive(actor->IsActive());

		for (const auto& component : actor->GetAllComponents())
		{
			const auto& original_component = component;
			auto clone_component = clone->AddComponent(component->GetComponentType());
			clone_component->SetAttributes(original_component->GetAttributes());
		}

		clones.emplace_back(clone);
		return clone;
	};

	//std::function<Actor*(Actor*)> clone_actor_descendants = [&clone_actor_descendants, &clone_actor](Actor* original) -> Actor*
	//{
	//	const auto clone = clone_actor(original);
	//
	//	for (const auto& child : original->GetTransform()->GetChilds())
	//	{
	//		const auto child_clone = clone_actor_descendants(child->GetActor());
	//		child_clone->GetTransform()->SetParent(clone->GetTransform());
	//	}
	//
	//	return clone;
	//};

	//clone_actor_descendants(this);

}

void Actor::Start()
{
	if (!bActive)
		return;

	for (auto component : components)
		component->OnStart();
}

void Actor::Update()
{
	if (!bActive)
		return;

	for (auto component : components)
		component->OnUpdate();

	for (auto render : renders)
		render->Update();
}
void Actor::Stop()
{
	if (!bActive)
		return;

	for (auto component : components)
		component->OnStop();
}

void Actor::Render()
{
	if (!bActive)
		return;

	for (auto component : components)
		component->OnRender();

	for (auto render : renders)
		render->Render();
}

Actor * Actor::CreateActor()
{
	//auto actor = Debug_new Actor();
	//actor->Init(actor->AddComponent<Transform>());
	//
	//return actor;
	
	Init(AddComponent<Transform>());

	return this;

}

auto Actor::AddComponent(const ComponentType & type) -> IComponent *
{
	IComponent* component = nullptr;

	switch (type)
	{
	case ComponentType::Unknown:														break;
	case ComponentType::Camera:				component = AddComponent<Camera>();				break;
	case ComponentType::FreeCamera:			component = AddComponent<FreeCamera>();			break;
	case ComponentType::FallowCamera:		component = AddComponent<FallowCamera>();		break;
	case ComponentType::Skybox:				component = AddComponent<CubeSky>();			break;
	case ComponentType::Transform:			component = AddComponent<Transform>();			break;
	case ComponentType::Animator:			component = AddComponent<ModelAnimator>();      break;
	case ComponentType::Light:				component = AddComponent<Lighting>();			break;
	case ComponentType::Collider:			component = AddComponent<Collider>();			break;
	case ComponentType::ObjectModel:		component = AddComponent<ModelRender>();		break;
	case ComponentType::ObjectMesh:			component = AddComponent<MeshRender>();			break;
	default:																			break;
	}

	return component;
}

const bool Actor::HasComponent(const ComponentType & type)
{
	for (auto component : components)
	{
		if (component->GetComponentType() == type)
			return true;
	}
	return false;
}

const bool Actor::HasRender(const RenderType & type)
{
	for (auto render : renders)
	{
		if (render->GetRenderType() == type)
			return true;
	}
	return false;
}

//auto Actor::AddRender(const RenderType & type) -> Renderer *
//{
//	Renderer* renderer = nullptr;
//
//	switch (type)
//	{
//	case RenderType::Cloud:					renderer = AddRender<Cloud>();					break;
//	case RenderType::Dome:					renderer = AddRender<Dome>();					break;
//	case RenderType::Moon:					renderer = AddRender<Moon>();					break;
//	case RenderType::Rain:					renderer = AddRender<Rain>();					break;
//	case RenderType::Snow:					renderer = AddRender<Snow>();					break;
//	case RenderType::Terrain:				renderer = AddRender<Terrain>();				break;
//	case RenderType::TerrainLOD:			renderer = AddRender<TerrainLOD>();				break;
//	case RenderType::Water:					renderer = AddRender<Water>();					break;
//	case RenderType::Billboard:				renderer = AddRender<Billboard>();				break;
//	case RenderType::PostEffect:			renderer = AddRender<PostEffect>();				break;
//	case RenderType::Render2D:				renderer = AddRender<Render2D>();				break;
//	case RenderType::Particle:				renderer = AddRender<Particle>();				break;
//	case RenderType::Unknown:																break;
//	default:																				break;
//	}
//
//	return renderer;
//}
