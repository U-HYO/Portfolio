#pragma once

class Rain : public Renderer
{
public:
	Rain(Vector3& _extent, UINT _count, std::wstring _file);
	Rain(class Actor* _actor, class Transform* _transform, Shader* _shader);
	~Rain();

	void SetRainShader(std::wstring _file, Vector3 & _extent = Vector3(300, 100, 500), UINT _count = (UINT)1e+4f);

	void Update();
	void Render();

private:
	struct Desc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		Vector3 Velocitty = Vector3(-10, -100, 0);
		float DrawDistance = 0;

		Vector3 Origin = Vector3(0, 0, 0);
		float Padding;

		Vector3 Extent = Vector3(0, 0, 0);
		float Padding2;
	}desc;


private:
	struct VertexRain
	{
		Vector3 Position;
		Vector2 UV;
		Vector2 Scale;
	};

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexRain* vertices;
	UINT* indices;

	Texture* texture;
	UINT drawCount = 0;

public:
	Desc GetDesc() { return desc; }
	void SetDesc(Desc _desc) { desc = _desc; }

};

