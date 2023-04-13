#pragma once

class Water : public Renderer
{
public:
	struct InitDesc;

public:
	Water(InitDesc& initDesc);
	Water(class Actor* _actor, class Transform* _transform, Shader* _shader); // ����������
	~Water();

	void Update();


	void SetWater(std::wstring path);

	void PreRender_Reflection();
	void PreRender_Refraction();
	void Render();

public:
	struct InitDesc
	{
		Shader* Shader;

		float Radius;				// ���� �� ������
		std::wstring HeightFile;	// �������� ������ó��
		float Width = 0;			// �ػ�
		float Height = 0;
	} initDesc;

private:
	struct Desc
	{
		Color RefractionColor = Color(0.2f, 0.3f, 1.0f, 1.0f);

		Vector2 NormalMapTile = Vector2(4, 8);
		float WaveTranslation = 0.0f;
		float WaveScale = 0.05f;

		float Shininess = 30.0f;
		float Alpha = 0.5f;
		float WaveSpeed = 0.06f;
		float WaterPositionY;

		float TerrainWidth;
		float TerrainHeight;
		float WaterWidth;
		float WaterHeight;
	} desc;

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	class Reflection* reflection;
	class Refraction* refraction;

	Texture* heightMap;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	Texture* waterMap;
};