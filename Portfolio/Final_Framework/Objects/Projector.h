#pragma once
class Projector
{
public:
	Projector(Shader* shader, std::wstring mapFile, float width, float height);
	~Projector();

	void Update();
	void Render();
private:
	struct Desc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);

		Matrix View;
		Matrix Projection;
	}desc;


private:
	Shader* shader;

	float width, height;

	Fixity* camera;
	Projection* projection;

	Texture* map;
	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Collider* collider;
};

