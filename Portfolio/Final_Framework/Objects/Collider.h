#pragma once
#include "Component/IComponent.h"

struct Ray
{
	Ray()
		: Position(0, 0, 0), Direction(0, 0, 0)
	{

	}

	Ray(Vector3& _position, Vector3& _dircetion)
		:Position(_position), Direction(_dircetion)
	{

	}

	Vector3 Position;
	Vector3 Direction;

};

struct ColliderObject
{
	class Transform* Init = NULL; // 기준
	class Transform* World = NULL; // 함수셋팅(메트릭스)
	class Collider* Collision = NULL;

public:
	ColliderObject();
	~ColliderObject();
};

class Collider : public IComponent
{
private:
	struct Bounding;

public:
	Collider(Transform* _transform, Transform* _init = NULL);
	Collider(class Actor* _actor, Transform* _transform);
	~Collider();

	void Update();
	void Render(Color _color = Color(0, 1, 0, 1));
	
	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnStop() override;
	void OnDestroy() override;

	bool Intersection(Vector3& _position, Vector3& _direction, float* _outDistance);
	bool Intersection(Ray& _ray, float* _outDistance);
	bool Intersection(Collider* _other);
private:
	bool SeperatingPlane(Vector3& _position, Vector3& _direction, Bounding & _box1, Bounding & _box2); // 분리축 평면생성
	bool Collision(Bounding& _box1, Bounding& _box2);
	Vector3 Cross(Vector3& _vec1, Vector3& _vec2);

private:
	struct Bounding
	{
		Vector3 Position;

		Vector3 AxisX;
		Vector3 AxisY;
		Vector3 AxisZ;

		Vector3 HalfSize;
	} bounding;

private:
	Transform* init = NULL;
	Transform* transform;

	Vector3 lines[8];

};