#pragma once
#include "Component/IComponent.h"

enum class CameraMode : UINT
{
	FREE_CAMERA = 0,
	FALLOW_CAMERA
};

class Camera : public IComponent
{
public:
	Camera();
	Camera(class Actor* _actor, class Transform* _transform);
	virtual ~Camera();

	virtual void Update() {};

public:
	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;
	void OnDestroy() override;
	void OnRender() override;

	void GetMatrix(Matrix* _mat);
	void SetRotMatrix(Matrix& _mat);

	class Transform* GetTransform() { return transform; }
	//void SetTransform(Transform* _transform) { transform = _transform; }
	void UpdateMove();

	Vector3 ScreenToWorldPoint(Vector2 mousePos, Matrix viewPorjectionMat, int screenWidth, int screenHeight);
protected:
	virtual void Rotation();
	virtual void Move();

protected:
	void View();
	void SetView(Vector3 _position, Vector3 _forward);

private:
	class Transform* transform;

	Matrix matRotation; // 회전행렬
	Matrix matView;

	// 뷰행렬
};

