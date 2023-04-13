#pragma once
#include "Camera.h"

class FallowCamera : public Camera
{
public:
	FallowCamera();
	FallowCamera(class Actor* _actor, class Transform* _transform);
	~FallowCamera();

	// Camera을(를) 통해 상속됨
	void Update() override;

	void Speed(float _move, float _rotation = 2.0f);


	void SetTarget(Vector3 _target, Vector3 _forward);
	
private:
	float moveSpeed = 20.0f;
	float rotation = 20.0f;

	Vector3 target = Vector3(0,0,0);
	Vector3 target2 = Vector3(0, 0, 0);

};

