#include "Framework.h"
#include "FallowCamera.h"

FallowCamera::FallowCamera()
	:Camera()
{
	GetTransform()->SetPosition(Vector3(0, 20, 30));
	GetTransform()->SetRotation(Vector3(0.25f, 0.0f, 0.0f));
	GetTransform()->SetScale(0.075f);
	target2.y = 15.0f;
}

FallowCamera::FallowCamera(Actor * _actor, Transform * _transform)
	:Camera(_actor, _transform)
{
}

FallowCamera::~FallowCamera()
{
}

void FallowCamera::Update()
{
	GetTransform()->Update();
}

void FallowCamera::Speed(float _move, float _rotation)
{
	moveSpeed = _move;
	rotation = _rotation;
}

void FallowCamera::SetTarget(Vector3 _target, Vector3 _forward)
{
	target = _target;
	SetView(Vector3(GetTransform()->GetWorld()._41, GetTransform()->GetWorld()._42, GetTransform()->GetWorld()._43), target + target2);
}