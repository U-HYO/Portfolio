#pragma once

class Cloud : public Renderer
{
public:
	Cloud(Shader* shader);
	~Cloud();

	void Update();
	void Render();
	void PostRender();

private:
	void CreateTexture();

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};



private:
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSRV;

	Render2D* render2D;


	//
	int skyPlaneResolution = 10;
	float skyPlaneWidth = 10.0f;
	float skyPlaneTop = 0.5f;
	float skyPlaneBottom = 0.0f;
	int textureRepeat = 1;



};