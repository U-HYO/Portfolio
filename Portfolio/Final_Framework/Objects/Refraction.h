#pragma once

class Refraction
{
public:
	Refraction(Shader* shader, Transform* transform, std::wstring normalFile, float width = 0, float height = 0);
	~Refraction();

	void Update();

	void PreRender();
	void Render();

private:
	Shader* shader;
	Transform* transform;

	float width, height;
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sRefractionSRV;

	Texture* normalMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};