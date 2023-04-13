#pragma once
#include "Component/IComponent.h"

class Transform : public IComponent
{
public:
	Transform();
	Transform(class Actor* _actor, class Transform* _transform);
	Transform(Shader* shader);
	~Transform();

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;
	void OnDestroy() override;
	void OnRender() override;

	void Set(Transform* transform);

	void SetShader(Shader* shader);

	void SetPosition(float _x, float _y, float _z);
	void SetPosition(Vector3& _vec);
	void GetPosition(Vector3* _vec);
	Vector3 GetPosition() { return position; }

	Vector3 GetWorldPosition() { return Vector3(bufferDesc.World._41, bufferDesc.World._42, bufferDesc.World._43);	}

	void SetRotation(float _x, float _y, float _z);
	void SetRotation(Vector3& _vec);
	void GetRotation(Vector3* _vec);
	Vector3 GetRotation() { return rotation; }

	void SetRotationDegree(float _x, float _y, float _z);
	void SetRotationDegree(Vector3& _vec);
	void GetRotationDegree(Vector3* _vec);

	Vector3 GetDirection();

	void SetRotationFromDirection(float _x, float _y, float _z); //방향벡터(Radian)로 Rotation설정
	void SetRotationFromDirection(Vector3& _vec);

	void SetScale(float _x, float _y, float _z);
	void SetScale(Vector3& _vec);
	void SetScale(float _x);
	void GetScale(Vector3* _vec);
	Vector3 GetScale() { return scale; }

	Vector3 GetForward();
	Vector3 GetUp();
	Vector3 GetRight();

	//void SetForward(Vector3 _forward)
	//{
	//	bufferDesc.World._31 = _forward.x;
	//	bufferDesc.World._32 = _forward.y;
	//	bufferDesc.World._33 = _forward.z;
	//}

	void SetWorld(Matrix& matrix);
	Matrix& GetWorld() { return bufferDesc.World; }

	Matrix* GetParent() { return &bufferDesc.World; }
	Matrix* GetScaleMat() { return &S; }
	Matrix* GetRotationMat() { return &R; }
	Matrix* GetTranslationMat() { return &T; }
	void SetParent(Transform* _transform);

	void SetRender(bool render);
	bool GetRender() { return bRender; }

private:
	void UpdateWorld();

public:
	void Update();
	void Render();

private:
	struct BufferDesc
	{
		Matrix World;
	} bufferDesc;

private:
	Shader* shader;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;


	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	Matrix S;
	Matrix R;
	Matrix T;
	D3DXMATRIX* parent; // parent World
	D3DXMATRIX* parentS; //parent Scale
	D3DXMATRIX* parentR; //parent Rotate
	D3DXMATRIX* parentT; //parent Translation


	bool bRender;


};