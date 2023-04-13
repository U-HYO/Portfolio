#pragma once

class PostEffect : public Renderer
{
public:
	PostEffect(std::wstring shaderFile);
	PostEffect(Actor * _actor, Transform * _transform, Shader * _shader);
	~PostEffect();

	void Update();
	void Render();

	void SRV(ID3D11ShaderResourceView* srv);

	void Temp();

private:
	struct Vertex
	{
		Vector3 Position;
	};

private:
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};