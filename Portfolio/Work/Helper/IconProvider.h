#pragma once

enum class IconType : UINT
{
	Custom,
	Thumbnail_Folder,
	Thumbnail_File,
	Button_Play_Big,
	Button_Play,
	Button_Pause,
	Button_Stop,
	Button_Option,
	Log_Info,
	Log_Warning,
	Log_Error,
};

struct Thumbnail
{
	Thumbnail()
		: type(IconType::Custom)
		, texture(nullptr)
		, path(L"")
	{}

	Thumbnail
	(
		const IconType& type,
		Texture* texture,
		const std::wstring& path
	)
		: type(type)
		, texture(texture)
		, path(path)
	{}

	IconType type;
	Texture* texture;
	std::wstring path;
};

class IconProvider final
{
public:
	static IconProvider& Get()
	{
		static IconProvider instance;
		return instance;
	}

	void Init(Context* context);

	auto GetShaderResourceFromType(const IconType& type)->ID3D11ShaderResourceView*;
	auto GetShaderResourceFromPath(const std::wstring& path)->ID3D11ShaderResourceView*;

	auto Load(const std::wstring& path, const IconType& type = IconType::Custom)->Thumbnail*;

	auto ImageButton(const Thumbnail& thumbnail, const ImVec2& size)->const bool;
	auto ImageButton(const IconType& type, const float& size)->const bool;
	auto ImageButton(const std::wstring& path, const float& size)->const bool;

private:
	IconProvider() = default;
	~IconProvider();

	IconProvider(const IconProvider&) = delete;
	IconProvider& operator=(const IconProvider&) = delete;

	auto GetThumbnailFromType(const IconType& type)->Thumbnail*;

private:
	Context* context;
	Texture* texture;
	std::vector<Thumbnail> thumbnails;
};