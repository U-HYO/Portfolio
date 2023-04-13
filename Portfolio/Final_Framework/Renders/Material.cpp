#include "Framework.h"
#include "Material.h"

Material::Material()
{
	Init();
}

Material::Material(Shader * _shader)
{
	Init();

	SetShader(_shader);
}

void Material::Init()
{
	name = L"";

	diffuseMap = NULL;
	specularMap = NULL;
	normalMap = NULL;

	buffer = Debug_new ConstantBuffer(&colorDesc, sizeof(ColorDesc));
}

Material::~Material()
{
	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
	SAFE_DELETE(normalMap);

	SAFE_DELETE(buffer);
}

void Material::SetShader(Shader * _shader)
{
	shader = _shader;

	sBuffer = _shader->AsConstantBuffer("CB_Material");

	sDiffuseMap = _shader->AsSRV("DiffuseMap");
	sSpecularMap = _shader->AsSRV("SpecularMap");
	sNormalMap = _shader->AsSRV("NormalMap");
}

void Material::Ambient(Color & _color)
{
	colorDesc.Ambient = _color;
}

void Material::Ambient(float _r, float _g, float _b, float _a)
{
	Ambient(Color(_r, _g, _b, _a));
}

void Material::Diffuse(Color & _color)
{
	colorDesc.Diffuse = _color;
}

void Material::Diffuse(float _r, float _g, float _b, float _a)
{
	Diffuse(Color(_r, _g, _b, _a));
}

void Material::Specular(Color & _color)
{
	colorDesc.Specular = _color;
}

void Material::Specular(float _r, float _g, float _b, float _a)
{
	Specular(Color(_r, _g, _b, _a));
}

void Material::Emissive(Color & _color)
{
	colorDesc.Emissive = _color;
}

void Material::Emissive(float _r, float _g, float _b, float _a)
{
	Emissive(Color(_r, _g, _b, _a));
}

void Material::DiffuseMap(std::string _file)
{
	DiffuseMap(String::ToWString(_file));
}

void Material::DiffuseMap(std::wstring _file)
{
	SAFE_DELETE(diffuseMap);

	diffuseMap = Debug_new Texture(_file);
}

void Material::SpecularMap(std::string _file)
{
	SpecularMap(String::ToWString(_file));
}

void Material::SpecularMap(std::wstring _file)
{
	SAFE_DELETE(specularMap);

	specularMap = Debug_new Texture(_file);
}

void Material::NormalMap(std::string _file)
{
	NormalMap(String::ToWString(_file));
}

void Material::NormalMap(std::wstring _file)
{
	SAFE_DELETE(normalMap);

	normalMap = Debug_new Texture(_file);
}

void Material::Render()
{
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());


	if (diffuseMap != NULL)
		sDiffuseMap->SetResource(diffuseMap->SRV());
	else
		sDiffuseMap->SetResource(NULL);

	if (specularMap != NULL)
		sSpecularMap->SetResource(specularMap->SRV());
	else
		sSpecularMap->SetResource(NULL);

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());
	else
		sNormalMap->SetResource(NULL);
}