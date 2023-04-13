#pragma once
#include "Camera.h"

class FreeCamera : public Camera
{
public:
	FreeCamera();
	FreeCamera(class Actor* _actor, class Transform* _transform);
	~FreeCamera();

	// Camera을(를) 통해 상속됨
	void Update() override;

	void Speed(float _move, float _rotation = 2.0f);

private:
	float moveSpeed = 20.0f;
	float rotation = 2.0f;




};

