#pragma once

class Snow : public Renderer
{
public:
	Snow(Vector3& extent, UINT count, std::wstring file);
	Snow(class Actor* _actor, class Transform* _transform, Shader* _shader);
	~Snow();

	
	void SetSnowShader(std::wstring _file, Vector3 & _extent = Vector3(300, 100, 500), UINT _count = (UINT)1e+4f);
	void Update();
	void Render();

private:
	struct Desc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		Vector3 Velocity = Vector3(0, -5, 0);
		float DrawDistance = 0;

		Vector3 Origin = Vector3(0, 0, 0);
		float Turbulence = 5;

		Vector3 Extent = Vector3(0, 0, 0);
		float Padding2;
	} desc;

private:
	struct VertexSnow
	{
		Vector3 Position;
		Vector2 Uv;
		float Scale;
		Vector2 Random;
	};

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexSnow* vertices;
	UINT* indices;

	Texture* texture;
	UINT drawCount = 100;


public:
	Desc GetDesc() { return desc; }
	void SetDesc(Desc _desc) { desc = _desc; }
};