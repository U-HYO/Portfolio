#include "Framework.h"
#include "Collider.h"

ColliderObject::ColliderObject()
{
	Init = Debug_new Transform();
	World = Debug_new Transform();
	Collision = Debug_new Collider(World, Init);
}

ColliderObject::~ColliderObject()
{
	SAFE_DELETE(Init);
	SAFE_DELETE(World);
	SAFE_DELETE(Collision);
}



Collider::Collider(Transform * _transform, Transform * _init)
	: transform(_transform), init(_init)
	, IComponent(nullptr, nullptr)
{
	lines[0] = Vector3(-0.5f, -0.5f, -0.5f);
	lines[1] = Vector3(-0.5f, +0.5f, -0.5f);
	lines[2] = Vector3(+0.5f, -0.5f, -0.5f);
	lines[3] = Vector3(+0.5f, +0.5f, -0.5f);
	lines[4] = Vector3(-0.5f, -0.5f, +0.5f);
	lines[5] = Vector3(-0.5f, +0.5f, +0.5f);
	lines[6] = Vector3(+0.5f, -0.5f, +0.5f);
	lines[7] = Vector3(+0.5f, +0.5f, +0.5f);
}

Collider::Collider(Actor * _actor, Transform * _transform)
	:IComponent(_actor, _transform)
{
	lines[0] = Vector3(-0.5f, -0.5f, -0.5f);
	lines[1] = Vector3(-0.5f, +0.5f, -0.5f);
	lines[2] = Vector3(+0.5f, -0.5f, -0.5f);
	lines[3] = Vector3(+0.5f, +0.5f, -0.5f);
	lines[4] = Vector3(-0.5f, -0.5f, +0.5f);
	lines[5] = Vector3(-0.5f, +0.5f, +0.5f);
	lines[6] = Vector3(+0.5f, -0.5f, +0.5f);
	lines[7] = Vector3(+0.5f, +0.5f, +0.5f);
}

Collider::~Collider()
{

}

void Collider::Update()
{
	Transform temp;
	temp.SetWorld(transform->GetWorld());

	if (init != NULL)
		temp.SetWorld(init->GetWorld() * transform->GetWorld());

	temp.GetPosition(&bounding.Position);

	D3DXVec3Normalize(&bounding.AxisX, &temp.GetRight());
	D3DXVec3Normalize(&bounding.AxisY, &temp.GetUp());
	D3DXVec3Normalize(&bounding.AxisZ, &temp.GetForward());

	Vector3 scale;
	temp.GetScale(&scale);
	bounding.HalfSize = scale * 0.5f;
}

void Collider::Render(Color _color)
{
	Vector3 dest[8];

	Transform temp;
	temp.SetWorld(transform->GetWorld());

	if (init != NULL)
		temp.SetWorld(init->GetWorld() * transform->GetWorld());

	Matrix world = temp.GetWorld();


	for (UINT i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&dest[i], &lines[i], &world);


	//Front
	DebugLine::Get()->RenderLine(dest[0], dest[1], _color);
	DebugLine::Get()->RenderLine(dest[1], dest[3], _color);
	DebugLine::Get()->RenderLine(dest[3], dest[2], _color);
	DebugLine::Get()->RenderLine(dest[2], dest[0], _color);

	//Backward
	DebugLine::Get()->RenderLine(dest[4], dest[5], _color);
	DebugLine::Get()->RenderLine(dest[5], dest[7], _color);
	DebugLine::Get()->RenderLine(dest[7], dest[6], _color);
	DebugLine::Get()->RenderLine(dest[6], dest[4], _color);

	//Side
	DebugLine::Get()->RenderLine(dest[0], dest[4], _color);
	DebugLine::Get()->RenderLine(dest[1], dest[5], _color);
	DebugLine::Get()->RenderLine(dest[2], dest[6], _color);
	DebugLine::Get()->RenderLine(dest[3], dest[7], _color);
}

bool Collider::Intersection(Vector3 & _position, Vector3 & _direction, float * _outDistance)
{
	return Intersection(Ray(_position, _direction), _outDistance);
}

bool Collider::Intersection(Ray & _ray, float * _outDistance)
{
	Transform temp;
	temp.SetWorld(transform->GetWorld());

	if (init != NULL)
		temp.SetWorld(init->GetWorld() * transform->GetWorld());

	Matrix world = temp.GetWorld();

	Vector3 minPosition, maxPosition;
	D3DXVec3TransformCoord(&minPosition, &lines[0], &world);
	D3DXVec3TransformCoord(&maxPosition, &lines[7], &world);

	if (fabsf(_ray.Direction.x) == 0.0f) _ray.Direction.x = 1e-6f;
	if (fabsf(_ray.Direction.y) == 0.0f) _ray.Direction.y = 1e-6f;
	if (fabsf(_ray.Direction.z) == 0.0f) _ray.Direction.z = 1e-6f;

	float minValue = 0.0f, maxValue = FLT_MAX;

	//Check X
	if (fabsf(_ray.Direction.x) >= 1e-6f)
	{
		float value = 1.0f / _ray.Direction.x;
		float minX = (minPosition.x - _ray.Position.x) * value;
		float maxX = (maxPosition.x - _ray.Position.x) * value;

		if (minX > maxX)
		{
			float temp = minX;
			minX = maxX;
			maxX = temp;
		}

		minValue = Math::Max(minX, minValue);
		maxValue = Math::Min(maxX, maxValue);

		if (minValue > maxValue)
			return false;
	}
	else if (_ray.Position.x < minPosition.x || _ray.Position.x > maxPosition.x)
		return false;

	//Check Y
	if (fabsf(_ray.Direction.y) >= 1e-6f)
	{
		float value = 1.0f / _ray.Direction.y;
		float minY = (minPosition.y - _ray.Position.y) * value;
		float maxY = (maxPosition.y - _ray.Position.y) * value;

		if (minY > maxY)
		{
			float temp = minY;
			minY = maxY;
			maxY = temp;
		}

		minValue = Math::Max(minY, minValue);
		maxValue = Math::Min(maxY, maxValue);

		if (minValue > maxValue)
			return false;
	}
	else if (_ray.Position.y < minPosition.y || _ray.Position.y > maxPosition.y)
		return false;

	//Check z
	if (fabsf(_ray.Direction.z) >= 1e-6f)
	{
		float value = 1.0f / _ray.Direction.z;
		float minZ = (minPosition.z - _ray.Position.z) * value;
		float maxZ = (maxPosition.z - _ray.Position.z) * value;

		if (minZ > maxZ)
		{
			float temp = minZ;
			minZ = maxZ;
			maxZ = temp;
		}

		minValue = Math::Max(minZ, minValue);
		maxValue = Math::Min(maxZ, maxValue);

		if (minValue > maxValue)
			return false;
	}
	else if (_ray.Position.z < minPosition.z || _ray.Position.z > maxPosition.z)
		return false;

	if (_outDistance != NULL)
		*_outDistance = minValue;

	return true; 
}

bool Collider::Intersection(Collider * _other)
{
	return Collision(bounding, _other->bounding);
}

bool Collider::SeperatingPlane(Vector3 & _position, Vector3 & _direction, Bounding & _box1, Bounding & _box2)
{
	float val = fabsf(D3DXVec3Dot(&_position, &_direction));

	float val2 = 0.0f;
	val2 += fabsf(D3DXVec3Dot(&(_box1.AxisX * _box1.HalfSize.x), &_direction));
	val2 += fabsf(D3DXVec3Dot(&(_box1.AxisY * _box1.HalfSize.y), &_direction));
	val2 += fabsf(D3DXVec3Dot(&(_box1.AxisZ * _box1.HalfSize.z), &_direction));
	val2 += fabsf(D3DXVec3Dot(&(_box2.AxisX * _box2.HalfSize.x), &_direction));
	val2 += fabsf(D3DXVec3Dot(&(_box2.AxisY * _box2.HalfSize.y), &_direction));
	val2 += fabsf(D3DXVec3Dot(&(_box2.AxisZ * _box2.HalfSize.z), &_direction));

	return val > val2;
}

bool Collider::Collision(Bounding & _box1, Bounding & _box2)
{
	Vector3 position = _box2.Position - _box1.Position;

	if (SeperatingPlane(position, _box1.AxisX, _box1, _box2) == true) return false;
	if (SeperatingPlane(position, _box1.AxisY, _box1, _box2) == true) return false;
	if (SeperatingPlane(position, _box1.AxisZ, _box1, _box2) == true) return false;

	if (SeperatingPlane(position, _box2.AxisX, _box1, _box2) == true) return false;
	if (SeperatingPlane(position, _box2.AxisY, _box1, _box2) == true) return false;
	if (SeperatingPlane(position, _box2.AxisZ, _box1, _box2) == true) return false;

	if (SeperatingPlane(position, Cross(_box1.AxisX, _box2.AxisX), _box1, _box2) == true) return false;
	if (SeperatingPlane(position, Cross(_box1.AxisX, _box2.AxisY), _box1, _box2) == true) return false;
	if (SeperatingPlane(position, Cross(_box1.AxisX, _box2.AxisZ), _box1, _box2) == true) return false;

	if (SeperatingPlane(position, Cross(_box1.AxisY, _box2.AxisX), _box1, _box2) == true) return false;
	if (SeperatingPlane(position, Cross(_box1.AxisY, _box2.AxisY), _box1, _box2) == true) return false;
	if (SeperatingPlane(position, Cross(_box1.AxisY, _box2.AxisZ), _box1, _box2) == true) return false;

	if (SeperatingPlane(position, Cross(_box1.AxisZ, _box2.AxisX), _box1, _box2) == true) return false;
	if (SeperatingPlane(position, Cross(_box1.AxisZ, _box2.AxisY), _box1, _box2) == true) return false;
	if (SeperatingPlane(position, Cross(_box1.AxisZ, _box2.AxisZ), _box1, _box2) == true) return false;

	return true;
}

Vector3 Collider::Cross(Vector3 & _vec1, Vector3 & _vec2)
{
	float x = _vec1.y * _vec2.z - _vec1.z * _vec2.y;
	float y = _vec1.z * _vec2.x - _vec1.x * _vec2.z;
	float z = _vec1.x * _vec2.y - _vec1.y * _vec2.x;

	return Vector3(x, y, z);
}

void Collider::OnInit()
{
}

void Collider::OnStart()
{
}

void Collider::OnUpdate()
{
	Update();
}

void Collider::OnRender()
{
	Render();
}

void Collider::OnStop()
{
}

void Collider::OnDestroy()
{
}

