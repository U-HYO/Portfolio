#pragma once
#define MAX_BILLBOARD_COUNT 10'000

class Billboard : public Renderer
{
public:
	Billboard(Shader* _shader);
	~Billboard();

	void Render();
	void Update();
	void Add(Vector3& _position, Vector2& _scale, UINT mapIndex);
	void AddTexture(std::wstring file);
private:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Scale;
		UINT MapIndex;
	};

private:
	std::vector<VertexBillboard> vertices;
	
	std::vector<std::wstring> textureFiles;
	TextureArray* textureArray = nullptr;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;

};

