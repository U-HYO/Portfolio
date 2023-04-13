#include "Framework.h"
#include "Camera.h"

Camera::Camera()
	:IComponent(nullptr, nullptr)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	transform = Debug_new Transform();
	Rotation();
	Move();
}

Camera::Camera(Actor * _actor, Transform * _transform)
	:IComponent(_actor, _transform)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	//transform = Debug_new Transform();
	Rotation();
	Move();
}

Camera::~Camera()
{
	SAFE_DELETE(transform);
}

void Camera::GetMatrix(Matrix * _mat)
{
	//*_mat = matView;
	memcpy(_mat, &matView, sizeof(Matrix));
}

void Camera::SetRotMatrix(Matrix & _mat)
{
	matRotation = _mat;
}

void Camera::UpdateMove()
{
	Move();
}

Vector3 Camera::ScreenToWorldPoint(Vector2 mousePos, Matrix viewPorjectionMat, int screenWidth, int screenHeight)
{
	// Convert screen position to normalized device coordinates
	float ndcX = (2.0f * mousePos.x) / screenWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y) / screenHeight;
	D3DXVECTOR3 ndcPos(ndcX, ndcY, 0.0f);

	// Convert to view space
	D3DXVECTOR4 clipPos;
	D3DXVec3Transform(&clipPos, &ndcPos, &viewPorjectionMat);
	D3DXVECTOR3 viewPos(clipPos.x / clipPos.w, clipPos.y / clipPos.w, clipPos.z / clipPos.w);

	// Convert to world space
	D3DXMATRIX inverseViewMatrix;
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewPorjectionMat);
	D3DXVECTOR3 worldPos;
	D3DXVec3TransformCoord(&worldPos, &viewPos, &inverseViewMatrix);

	return worldPos;
}

void Camera::Rotation()
{
	Matrix x, y, z;
	D3DXMatrixRotationX(&x, transform->GetRotation().x);
	D3DXMatrixRotationY(&y, transform->GetRotation().y);
	D3DXMatrixRotationZ(&z, transform->GetRotation().z);

	matRotation = x * y * z;
}

void Camera::Move()
{
	View();
}

void Camera::View()
{
	D3DXMatrixLookAtLH(&matView, &transform->GetPosition(), &(transform->GetPosition() + transform->GetForward()), &transform->GetUp());
}

void Camera::SetView(Vector3 _position, Vector3 _target)
{
	D3DXMatrixLookAtLH(&matView, &_position, &_target, &transform->GetUp());
}

void Camera::OnInit()
{
}

void Camera::OnStart()
{
}

void Camera::OnUpdate()
{
}

void Camera::OnStop()
{
}

void Camera::OnDestroy()
{
}

void Camera::OnRender()
{
}
