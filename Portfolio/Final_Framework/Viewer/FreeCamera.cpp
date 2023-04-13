#include "Framework.h"
#include "FreeCamera.h"

FreeCamera::FreeCamera()
	:Camera()
{
}

FreeCamera::FreeCamera(Actor * _actor, Transform * _transform)
	:Camera(_actor, _transform)
{
}

FreeCamera::~FreeCamera()
{
}

void FreeCamera::Update()
{
	if (Context::Get()->GetCameraIndex() != (UINT)CameraMode::FREE_CAMERA) return;

	//if (Mouse::Get()->Press(1) == false) return; // 오른쪽마우스

	//Vector3 f = Vector3(0, 0, 1);
	//Vector3 u = Vector3(0, 1, 0);
	//Vector3 r = Vector3(1, 0, 0);

	//Vector3 f = GetForward();
	//Vector3 u = GetUp();
	//Vector3 r = GetRight();

	Vector3 f = GetTransform()->GetForward();
	Vector3 u = GetTransform()->GetUp();
	Vector3 r = GetTransform()->GetRight();

	//Move
	{
		Vector3 pos;
		GetTransform()->GetPosition(&pos);

		if (_KEYCODE->Press('W'))
			pos += f * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('S'))
			pos -= f * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('D'))
			pos += r * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('A'))
			pos -= r * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('Q'))
			pos -= u * moveSpeed * Time::Delta();

		if (_KEYCODE->Press('E'))
			pos += u * moveSpeed * Time::Delta();


		Vector3 rot;
		GetTransform()->GetRotation(&rot);

		if (Mouse::Get()->Press(1) == false)
		{
			GetTransform()->SetPosition(pos);
			GetTransform()->SetRotation(rot);
			View();
			return;
		}
		Vector3 val = Mouse::Get()->GetMoveValue();
		rot.x = rot.x + val.y * rotation * (Time::Delta()/4);
		rot.y = rot.y + val.x * rotation * (Time::Delta()/4);

		rot.z = 0.0f;

		pos += f * val.z * Time::Delta() * 10.0f;

		GetTransform()->SetPosition(pos);
		GetTransform()->SetRotation(rot);

		View();
	}

}

void FreeCamera::Speed(float _move, float _rotation)
{
	moveSpeed = _move;
	rotation = _rotation;
}
