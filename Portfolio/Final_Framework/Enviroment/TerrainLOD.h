#pragma once

class TerrainLOD : public Renderer
{
public:
	TerrainLOD();
	TerrainLOD(std::wstring imageFile); // ����������
	TerrainLOD(class Actor* _actor, class Transform* _transform, Shader* _shader); // ����������
	~TerrainLOD();

	virtual void Update();
	virtual void Render();

	void BaseMap(std::wstring file);

public:
	float GetWidth();
	float GetHeight();

	void SetTerrainMap(std::wstring _imageFile);
	auto GetImageFile() { return imageFile; }
	ID3D11ShaderResourceView* GetHeightMap() { return heightMapSRV; }

private:
	void ReadHeightData();
	void CreatePatchVertex();
	void CreatePatchBound(); // �ش� ������ ���� �ִ�, �ּҳ��̸� ����
	void CreatePatchIndex();

private:
	struct TerrainVertex
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Bound; //x�� �ּ� y�� �ִ�
	};

private:
	struct Desc
	{
		Vector2 Distance = Vector2(1, 1000);	// �׼����̼� �� �ּҰŸ�~�ִ�Ÿ�
		Vector2 TessFactor = Vector2(1, 64);	// �׼����̼� �ּҰ��� ~ �ִ밹��

		float Cellspacing = 2.0f;
		float CellspacingU;			// �� �ȼ��� ũ��
		float CellspacingV;			// �� �ȼ��� ũ��
		float HeightScale = 0.5f;	// heightMap scale

		Plane Culling[6];
	} desc;

private:
	UINT cellsPerPatch = 32;	//�� ĭ�� ���� ����

	std::wstring imageFile;

	UINT width, height;
	UINT patchWidth, patchHeight;

	float* heights;	//���̸� �迭����
	Vector2* bounds;

	ID3D11Texture2D* heightMap = NULL;
	ID3D11ShaderResourceView* heightMapSRV = NULL;
	ID3DX11EffectShaderResourceVariable* sHeightMapSRV;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Camera* camera;
	Perspective* perspective;
	Frustum* frustum;

};

