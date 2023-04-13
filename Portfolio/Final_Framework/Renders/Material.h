#pragma once

class Material
{
public:
	Material();
	Material(Shader* _shader);
	~Material();

	Shader* GetShader() { return shader; }
	void SetShader(Shader* _shader);


	void Name(std::wstring _val) { name = _val; }
	std::wstring Name() { return name; }

	Color& Ambient() { return colorDesc.Ambient; }
	void Ambient(Color& _color);
	void Ambient(float _r, float _g, float _b, float _a = 1.0f);

	Color& Diffuse() { return colorDesc.Diffuse; }
	void Diffuse(Color& _color);
	void Diffuse(float _r, float _g, float _b, float _a = 1.0f);

	Color& Specular() { return colorDesc.Specular; }
	void Specular(Color& _color);
	void Specular(float _r, float _g, float _b, float _a = 1.0f);

	Color& Emissive() { return colorDesc.Emissive; }
	void Emissive(Color& _color);
	void Emissive(float _r, float _g, float _b, float _a = 1.0f);


	Texture* DiffuseMap() { return diffuseMap; }
	void DiffuseMap(std::string _file);
	void DiffuseMap(std::wstring _file);

	Texture* SpecularMap() { return specularMap; }
	void SpecularMap(std::string _file);
	void SpecularMap(std::wstring _file);

	Texture* NormalMap() { return normalMap; }
	void NormalMap(std::string _file);
	void NormalMap(std::wstring _file);

	void Render();

private:
	void Init();

private:
	struct ColorDesc
	{
		Color Ambient = Color(0, 0, 0, 1);
		Color Diffuse = Color(1, 1, 1, 1);
		Color Specular = Color(0, 0, 0, 1);
		Color Emissive = Color(0, 0, 0, 1);
	} colorDesc;

private:
	Shader* shader;

	std::wstring name;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
	ID3DX11EffectShaderResourceVariable* sSpecularMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;

	
	
	
	
	
	
	
};