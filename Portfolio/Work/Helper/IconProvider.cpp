#include "stdafx.h"
#include "IconProvider.h"

void IconProvider::Init(Context * context)
{
	this->context = context;

	Load(L"../../_Textures/Icon/folder.png", IconType::Thumbnail_Folder);
	Load(L"../../_Textures/Icon/file.png", IconType::Thumbnail_File);
	Load(L"../../_Textures/Icon/big_play_button.png", IconType::Button_Play_Big);
	Load(L"../../_Textures/Icon/playButton.png", IconType::Button_Play);
	Load(L"../../_Textures/Icon/pauseButton.png", IconType::Button_Pause);
	Load(L"../../_Textures/Icon/stopButton.png", IconType::Button_Stop);
	Load(L"../../_Textures/Icon/option_button.png", IconType::Button_Option);
	Load(L"../../_Textures/Icon/log_info.png", IconType::Log_Info);
	Load(L"../../_Textures/Icon/log_warning.png", IconType::Log_Warning);
	Load(L"../../_Textures/Icon/log_error.png", IconType::Log_Error);
}

auto IconProvider::GetShaderResourceFromType(const IconType & type) -> ID3D11ShaderResourceView *
{
	return Load(L"", type)->texture->SRV();
}

auto IconProvider::GetShaderResourceFromPath(const std::wstring & path) -> ID3D11ShaderResourceView *
{
	return Load(path)->texture->SRV();
}

auto IconProvider::Load(const std::wstring & path, const IconType & type) -> Thumbnail *
{
	if (type != IconType::Custom)
	{
		for (auto& thumbnail : thumbnails)
		{
			if (thumbnail.type == type)
				return &thumbnail;
		}
	}
	else
	{
		for (auto& thumbnail : thumbnails)
		{
			if (thumbnail.path == path)
				return &thumbnail;
		}
	}

	if (FileSystem::IsDirectory(path))
		return GetThumbnailFromType(IconType::Thumbnail_Folder);
	if (FileSystem::IsSupportedImageFile(FileSystem::ToString(path)))
	{
		//Thread::Get()->AddTask([texture, path]()
		//{
			texture = Debug_new Texture(path, false);
			texture->SetWidth(100);
			texture->SetHeight(100);
		//});
		return &thumbnails.emplace_back(type, texture, path);
	}

	return GetThumbnailFromType(IconType::Thumbnail_File);
}

auto IconProvider::ImageButton(const Thumbnail & thumbnail, const ImVec2 & size) -> const bool
{
	bool bPressed = ImGui::ImageButton
	(
		thumbnail.texture->SRV(),
		size
	);
	return bPressed;
}

auto IconProvider::ImageButton(const IconType & type, const float & size) -> const bool
{
	bool bPressed = ImGui::ImageButton
	(
		GetShaderResourceFromType(type),
		ImVec2(size, size)
	);
	return bPressed;
}

auto IconProvider::ImageButton(const std::wstring & path, const float & size) -> const bool
{
	bool bPressed = ImGui::ImageButton
	(
		GetShaderResourceFromPath(path),
		ImVec2(size, size)
	);
	return bPressed;
}

IconProvider::~IconProvider()
{
	for (auto t : thumbnails)
	{
		SAFE_DELETE(t.texture);
	}
	thumbnails.clear();
}

auto IconProvider::GetThumbnailFromType(const IconType & type) -> Thumbnail *
{
	for (auto& thumbnail : thumbnails)
	{
		if (thumbnail.type == type)
			return &thumbnail;
	}
	return nullptr;
}
