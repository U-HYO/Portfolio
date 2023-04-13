#pragma once

class Particle : public Renderer
{
public:
	Particle(std::wstring file);
	Particle(class Actor* _actor, class Transform* _transform, Shader* _shader);
	~Particle();

	void Reset();
	void Add(Vector3& position); //���� �÷��� �� ��ġ
	void Add();
	void Update();

private:
	void MapVertices();
	void Activation();
	void Deactivation();


public:
	void GetParticleFile(std::wstring _path);
	void Render();

public:
	ParticleData& GetData() { return data; }
	void SetTexture(std::wstring file, bool bInTextureFolder = true);
	Texture* GetTexture() { return map; }
private:
	void ReadFile(std::wstring file);

private:
	struct VertexParticle
	{
		Vector3 Position;
		Vector2 Corner; //(-1 ~ +1)
		Vector3 Velocity;
		Vector4 Random; //x:�ֱ�, y:ũ��, z:ȸ��, w:����
		float Time;
	};

private:
	struct Desc
	{
		Color MinColor;
		Color MaxColor;

		Vector3 Gravity;
		float EndVelocity;

		Vector2 StartSize;
		Vector2 EndSize;

		Vector2 RotateSpeed;
		float ReadyTime;
		float ReadyRandomTime;

		float CurrentTime;
		float Padding[3];
	} desc;

private:
	ParticleData data;

	Texture* map;
	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;


	VertexParticle* vertices = NULL;
	UINT* indices = NULL;


	float currentTime = 0.0f; // ����ð�
	float lastAddTime = 0.0f; // ������ �߰��� �ð�

	UINT leadCount = 0; // �߰��� ��ü ���� ����
	UINT gpuCount = 0; // GPU�� ���� ���� ����
	UINT activeCount = 0; // �׷��� ����
	UINT deactiveCount = 0;
};