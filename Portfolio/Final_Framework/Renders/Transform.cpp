#include "Framework.h"
#include "Transform.h"

Transform::Transform()
	: shader(NULL), buffer(NULL)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, parent(NULL), parentS(NULL), bRender(true)
	, IComponent(nullptr, nullptr)
{
	D3DXMatrixIdentity(&bufferDesc.World);
}

Transform::Transform(Actor * _actor, Transform * _transform)
	:shader(NULL), buffer(NULL)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, parent(NULL), parentS(NULL), bRender(true)
	, IComponent(_actor, _transform)
{
	D3DXMatrixIdentity(&bufferDesc.World);
}

Transform::Transform(Shader * shader)
	: position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, parent(NULL), parentS(NULL), bRender(true)
	, IComponent(nullptr, nullptr)
{
	SetShader(shader);

	D3DXMatrixIdentity(&bufferDesc.World);
}

Transform::~Transform()
{
	SAFE_DELETE(buffer);
}

void Transform::Set(Transform * transform)
{
	position = transform->position;
	scale = transform->scale;
	rotation = transform->rotation;

	UpdateWorld();
}

void Transform::SetShader(Shader* shader)
{
	this->shader = shader;

	buffer = Debug_new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = this->shader->AsConstantBuffer("CB_World");
}

void Transform::SetPosition(float _x, float _y, float _z)
{
	SetPosition(Vector3(_x, _y, _z));
}

void Transform::SetPosition(Vector3 & _vec)
{
	position = _vec;
	UpdateWorld();
}

void Transform::GetPosition(Vector3 * _vec)
{
	*_vec = position;
}

void Transform::SetRotation(float _x, float _y, float _z)
{
	SetRotation(Vector3(_x, _y, _z));
}

void Transform::SetRotation(Vector3 & _vec)
{
	rotation = _vec;
	UpdateWorld();
}

void Transform::GetRotation(Vector3 * _vec)
{
	*_vec = rotation;
}

void Transform::SetRotationDegree(float _x, float _y, float _z)
{
	SetRotationDegree(Vector3(_x, _y, _z));
}

void Transform::SetRotationDegree(Vector3 & _vec)
{
	Vector3 temp;

	temp.x = Math::ToRadian(_vec.x);
	temp.y = Math::ToRadian(_vec.y);
	temp.z = Math::ToRadian(_vec.z);

	SetRotation(temp);
}

void Transform::GetRotationDegree(Vector3 * _vec)
{
	Vector3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	*_vec = temp;
}

void Transform::SetRotationFromDirection(float _x, float _y, float _z)
{
	SetRotationFromDirection(Vector3(_x, _y, _z));
}

void Transform::SetRotationFromDirection(Vector3 & _vec)
{
	Matrix R;
	Vector3 direction = _vec;
	D3DXVec3Normalize(&direction, &direction);
	D3DXMatrixLookAtLH(&R, &Vector3(0, 0, 0), &direction, &Vector3(0, 1, 0));


	float tempY = sqrtf(R._11 * R._11 + R._21 * R._21);

	float x, y, z;
	if (tempY < 1e-6f)
	{
		x = atan2(-R._23, R._22);
		y = atan2(-R._31, tempY);
		z = 0;
	}
	else
	{
		x = atan2(R._32, R._33);
		y = atan2(-R._31, tempY);
		z = atan2(R._21, R._11);
	}

	SetRotation(x, y, z);

}

void Transform::SetScale(float _x, float _y, float _z)
{
	SetScale(Vector3(_x, _y, _z));
}

void Transform::SetScale(Vector3 & _vec)
{
	scale = _vec;

	UpdateWorld();
}

void Transform::SetScale(float _x)
{
	SetScale(_x, _x, _x);
}

void Transform::GetScale(Vector3 * _vec)
{
	*_vec = scale;
}


Vector3 Transform::GetForward()
{
	return Vector3(bufferDesc.World._31, bufferDesc.World._32, bufferDesc.World._33);
}

Vector3 Transform::GetUp()
{
	return Vector3(bufferDesc.World._21, bufferDesc.World._22, bufferDesc.World._23);
}

Vector3 Transform::GetRight()
{
	return Vector3(bufferDesc.World._11, bufferDesc.World._12, bufferDesc.World._13);
}

void Transform::SetWorld(Matrix & matrix)
{
	Math::MatrixDecompose(matrix, scale, rotation, position);

	bufferDesc.World = matrix;
}

void Transform::UpdateWorld()
{
	Vector3 tempScale = bRender ? scale * 1.0f : scale * 0.0f;

	D3DXMatrixScaling(&S, tempScale.x, tempScale.y, tempScale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	D3DXMATRIX world;
	
	if (parent != NULL)
	{
		//D3DXMATRIX invParentS;
		//D3DXMatrixInverse(&invParentS, NULL, parentS);
		//world *= invParentS * *parent;

		world = S * R * T * *parentR * *parentT;
	}
	else
	{
		world = S * R * T;
	}

	bufferDesc.World = world;
}

void Transform::Update()
{
	UpdateWorld();
}

void Transform::Render()
{
	if (shader == NULL)
		return;

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());
}

void Transform::OnInit()
{
}

void Transform::OnStart()
{
}

void Transform::OnUpdate()
{
	Update();
}

void Transform::OnStop()
{
}

void Transform::OnDestroy()
{
}

void Transform::OnRender()
{
}

void Transform::SetParent(Transform * _transform)
{
	parent = _transform->GetParent();
	parentS = _transform->GetScaleMat();
	parentR = _transform->GetRotationMat();
	parentT = _transform->GetTranslationMat();
}

void Transform::SetRender(bool render)
{
	bRender = render;

	

}
