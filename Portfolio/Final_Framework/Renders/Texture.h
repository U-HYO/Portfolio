#pragma once

class Texture
{
public:
	friend class Textures;

public:
	void SaveFile(std::wstring file);
	static void SaveFile(std::wstring file, ID3D11Texture2D* src);

	D3D11_TEXTURE2D_DESC ReadPixel(std::vector<Color>* pixels);
	static D3D11_TEXTURE2D_DESC ReadPixel(ID3D11Texture2D* src, std::vector<Color>* pixels);

public:
	Texture(std::wstring file, bool isInTextureFolder = true,D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);
	~Texture();

	operator ID3D11ShaderResourceView*() { return view; }


	std::wstring GetFile() { return file; }

	UINT GetWidth() { return metaData.width; }
	void SetWidth(UINT _width) { metaData.width = _width; }
	UINT GetHeight() { return metaData.height; }
	void SetHeight(UINT _height) { metaData.height= _height; }

	void GetImageInfo(DirectX::TexMetadata* data)
	{
		*data = metaData;
	}

	//ID3D11ShaderResourceView* SRV() { return view; }
	ID3D11ShaderResourceView* SRV() const { return view; }
	ID3D11Texture2D* GetTexture();

private:
	std::wstring file;

	DirectX::TexMetadata metaData;
	ID3D11ShaderResourceView* view;
};

struct TextureDesc
{
	std::wstring file;
	UINT width, height;
	DirectX::TexMetadata metaData;
	ID3D11ShaderResourceView* view;

	bool operator==(const TextureDesc& desc)
	{
		bool b = true;
		b &= file == desc.file;
		b &= width == desc.width;
		b &= height == desc.height;

		return b;
	}
};

class Textures
{
public:
	friend class Texture;

public:
	static void Create();
	static void Delete();

private:
	static void Load(Texture* texture, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);

private:
	static std::vector<TextureDesc> descs;
};

class TextureArray
{
public:
	TextureArray(std::vector<std::wstring>& names, UINT width = 256, UINT height = 256, UINT mipLevels = 1);
	~TextureArray();

	ID3D11ShaderResourceView* SRV() { return srv; }

private:
	std::vector<ID3D11Texture2D *> CreateTextures(std::vector<std::wstring>& names, UINT width, UINT height, UINT mipLevels);

private:
	ID3D11ShaderResourceView* srv;
};