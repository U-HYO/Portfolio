#include "stdafx.h"
#include "FileDialog.h"

namespace _FileDialog
{
	static float item_size_min = 50.0f;
	static float item_size_max = 200.0f;
	static bool is_hovered_item;
	static bool is_hovered_window;
	static std::string hovered_item_path;
	//static DragDropPayload payload;
	static UINT menu_id;
}

#define OPERATION_NAME  (operation == FileDialogOperation::Open) ? "Open" : (operation == FileDialogOperation::Load) ? "Load" : (operation == FileDialogOperation::Save) ? "Save" : "View"
#define FILTER_NAME     (filter == FileDialogFilter::All) ? "All (*.*)" : (filter == FileDialogFilter::Model) ? "Model (*.*)" : "Scene (*.scene)"

FileDialog::FileDialog(Context * context, const FileDialogType & type, const FileDialogOperation & operation, const FileDialogFilter & filter, const bool & is_window)
	: //context(context),
	 type(type)
	, operation(operation)
	, filter(filter)
	, is_window(is_window)
	, item_size(100.0f, 100.0f)
	, is_update(true)
	, is_selection(false)
	, callback_on_item_clicked(nullptr)
	, callback_on_item_double_clicked(nullptr)
{
	title = OPERATION_NAME;
	current_directory = "../../_Assets";//FileSystem::GetWorkingDirectory();
	selectedItem = "Selected Item : none";

	filterName = "";
	extraButton = false;
	extraButtonLabel = "LoadExtra";
}

FileDialog::~FileDialog()
{
	items.clear();
}

void FileDialog::SetOperation(const FileDialogOperation & operation)
{
	if (this->operation == operation)
		return;

	this->operation = operation;
	this->title = OPERATION_NAME;
}

auto FileDialog::Show(bool * is_visible, std::string * directory, std::string * path) -> const bool
{
	if (!(*is_visible))
	{
		is_update = true;

		if (is_window)
		{
			current_directory = *directory;
		}
		else
			current_directory = "../../_Assets";

		if (directory->size() > 0)
		{
			current_directory = *directory;
		}

		filterName = "";

		return false;
	}

	//current_directory = directory->size() == 0 ? "../../_Assets" : *directory;
	

	is_selection = false;
	_FileDialog::is_hovered_item = false;
	_FileDialog::is_hovered_window = false;

	ShowTop(is_visible);
	ShowMiddle();
	ShowBottom(is_visible);


	if (is_window)
	{
		ImGui::End();
	}


	if (is_update)
	{
		UpdateDirectory(current_directory);
		is_update = false;
	}

	if (is_selection)
	{
		if (directory)
			(*directory) = current_directory;

		if (path)
			(*path) = current_directory + "/" + std::string(input_box);

		selectedItem = "Selected Item : " + current_directory + "/" + std::string(input_box);

	}

	EmptyAreaMenu();

	return is_selection;
}

void FileDialog::SetCallbackOnItemClicked(const std::function<void(const std::string&)>& callback)
{
	callback_on_item_clicked = callback;
}

void FileDialog::SetCallbackOnItemDoubleClicked(const std::function<void(const std::string&)>& callback)
{
	callback_on_item_double_clicked = callback;
}

void FileDialog::ExtraButton()
{
	//if (ImGui::Button(""))
	//{
	//	extraButton != extraButton;
	//};

	extraButton = ImGui::Button(extraButtonLabel.c_str());
}

bool FileDialog::GetExtraButton()
{
	return extraButton;
}

void FileDialog::ShowTop(bool * is_visible)
{
	if (is_window)
	{
		ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints
		(
			ImVec2(350.0f, 250.0f),
			ImVec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
		);
		ImGui::Begin(title.c_str(), is_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing); //| ImGuiWindowFlags_NoDocking);
		ImGui::SetWindowFocus();
	}

	if (ImGui::ArrowButton("##Left_button", ImGuiDir_Left))
	{
		SetCurrentPath(FileSystem::GetParentDirectory(current_directory));
		is_update = true;
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(current_directory.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.8f);
	ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.207f);
	{
		float previous_width = item_size.x;
		ImGui::SliderFloat("##FileDialogSlider", &item_size.x, _FileDialog::item_size_min, _FileDialog::item_size_max);
		item_size.y += item_size.x - previous_width;
	}
	ImGui::PopItemWidth();
	ImGui::TextUnformatted(selectedItem.c_str());
	ImGui::Separator();
}

void FileDialog::ShowMiddle()
{
	const auto content_width = ImGui::GetContentRegionAvail().x;
	const auto content_height = ImGui::GetContentRegionAvail().y - (type == FileDialogType::Browser ? 50.0f : 30.0f);

	auto& g = *GImGui;
	auto& style = ImGui::GetStyle();
	const float font_scale = item_size.x / 100.0f;
	const float label_height = g.FontSize * font_scale;
	const float text_offset = 3.0f;
	auto check = false;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
	if (check = ImGui::BeginChild("##ContentRegion", ImVec2(content_width, content_height), true))
	{
		_FileDialog::is_hovered_window = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup);

		auto pen_x = 0.0f;
		auto new_line = true;

		for (int i = 0; i < static_cast<int>(items.size()); i++)
		{
			static int selected = -1;
			if (new_line)
			{
				ImGui::BeginGroup();
				new_line = false;
			}

			ImGui::BeginGroup();
			{
				auto& item = items[i];
				auto button_pos_min = ImGui::GetCursorScreenPos();
				auto button_pos_max = ImVec2(button_pos_min.x + item_size.x, button_pos_min.y + item_size.y);

				//Tumbnail
				{
					ImGui::PushID(i);
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
					{
						//if (ImGui::Button("##dummy", item_size))
						if (ImGui::Selectable("##dummy", selected == i, ImGuiSelectableFlags_SelectOnClick, item_size))
						{
							item.Clicked();
							const auto is_single_click = item.GetTimeSinceLastClickedMs() > 500;
							selected = i;
							if (is_single_click)
							{
								input_box = item.GetLabel();
								SetCurrentPath(item.GetPath());
								is_selection = !item.IsDirectory();

								if (callback_on_item_clicked)
									callback_on_item_clicked(item.GetPath());
							}
							else
							{
								is_update = SetCurrentPath(item.GetPath());
								is_selection = !item.IsDirectory();

								if (callback_on_item_double_clicked)
									callback_on_item_double_clicked(current_directory);
							}
						}

						if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
						{
							_FileDialog::is_hovered_item = true;
							_FileDialog::hovered_item_path = item.GetPath();
						}
						
						if (ImGui::IsMouseClicked(0) && _FileDialog::is_hovered_window && !_FileDialog::is_hovered_item)
						{//빈공간 클릭시
							selected = -1;
							selectedItem = "Selected Item : none";
						}

						ItemClick(&item);
						ItemMenu(&item);
						ItemDrag(&item);

						ImGui::SetCursorScreenPos(button_pos_min + style.FramePadding);
						ImGui::Image
						(
							item.GetTexture()->SRV(),
							ImVec2
							(
								button_pos_max.x - button_pos_min.x - style.FramePadding.x * 2.0f,
								button_pos_max.y - button_pos_min.y - style.FramePadding.y - label_height - 5.0f
							)
						);
					}
					ImGui::PopStyleColor(2);
					ImGui::PopID();
				}

				//Label
				{
					auto label_text = item.GetLabel().c_str();
					auto label_size = ImGui::CalcTextSize(label_text, nullptr, true);
					auto label_rect = ImRect
					(
						button_pos_min.x,
						button_pos_max.y - label_height - style.FramePadding.y,
						button_pos_max.x,
						button_pos_max.y
					);

					ImGui::GetWindowDrawList()->AddRectFilled
					(
						label_rect.Min,
						label_rect.Max,
						IM_COL32(255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.2f, 255.0f * 0.75f)
					);

					ImGui::SetWindowFontScale(font_scale);
					ImGui::SetCursorScreenPos(ImVec2(label_rect.Min.x, label_rect.Min.y + text_offset));
					if (label_size.x <= item_size.x)
						ImGui::TextUnformatted(label_text);
					else
						ImGui::RenderTextClipped
						(
							label_rect.Min,
							label_rect.Max,
							label_text,
							nullptr,
							&label_size,
							ImVec2(0, 0),
							&label_rect
						);
				}
			}
			ImGui::EndGroup();

			pen_x += item_size.x + style.ItemSpacing.x;
			if (pen_x >= content_width - item_size.x)
			{
				ImGui::EndGroup();
				pen_x = 0;
				new_line = true;
			}
			else
				ImGui::SameLine();
		}

		if (!new_line)
			ImGui::EndGroup();

		ImGui::EndChild();
	}

	if (!check)
		ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::Separator();
}

void FileDialog::ShowBottom(bool * is_visible)
{
	if (type == FileDialogType::Browser)
		return;

	//ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 35.0f);
	//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);

	//ImGui::PushItemWidth(ImGui::GetWindowSize().x - 235.0f);
	//{
	//	//ImGui::InputText("##InputBox", &input_box);
	//}
	//ImGui::PopItemWidth();

	//ImGui::SameLine();
	//ImGui::TextUnformatted(FILTER_NAME);

	ExtraButton();


	ImGui::SameLine();
	static char str0[128] = "Temp";
	strcpy_s(str0, filterName.c_str());
	ImGui::SetNextItemWidth(100.0f);
	ImGui::InputText("##Filter", str0, IM_ARRAYSIZE(str0));
	filterName = str0;
	//UpdateDirectory(current_directory);


	//ImGui::SameLine();
	//if (ImGui::Button(OPERATION_NAME))
	//	is_selection = true;
	//
	//ImGui::SameLine();
	//if (ImGui::Button("Cancel"))
	//{
	//	is_selection = false;
	//	(*is_visible) = false;
	//}


}

void FileDialog::ItemDrag(FileDialogItem * item) const
{
	//if (!item || type != FileDialogType::Browser)
	//	return;
	//
	//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	//{
	//	const auto set_payload = [](const PayloadType& type, const std::string& path)
	//	{
	//		_FileDialog::payload.type = type;
	//		_FileDialog::payload.data = path.c_str();
	//		DragDrop::CreateDragDropPayload(_FileDialog::payload);
	//	};
	//
	//	if (FileSystem::IsSupportedImageFile(item->GetPath())) set_payload(PayloadType::Texture, item->GetPath());
	//	if (FileSystem::IsSupportedAudioFile(item->GetPath())) set_payload(PayloadType::Audio, item->GetPath());
	//	if (FileSystem::IsSupportedModelFile(item->GetPath())) set_payload(PayloadType::Model, item->GetPath());
	//
	//	ImGui::Image(item->GetTexture()->GetShaderResourceView(), ImVec2(50, 50));
	//
	//	ImGui::EndDragDropSource();
	//}
}

void FileDialog::ItemClick(FileDialogItem * item) const
{
	if (!item || !_FileDialog::is_hovered_window)
		return;

	if (ImGui::IsItemClicked(1))
	{
		_FileDialog::menu_id = item->GetID();
		ImGui::OpenPopup("##FileDialogPopup");
	}
}

void FileDialog::ItemMenu(FileDialogItem * item)
{
	if (_FileDialog::menu_id != item->GetID())
		return;

	if (!ImGui::BeginPopup("##FileDialogPopup"))
		return;

	if (ImGui::MenuItem("Delete"))
	{
		if (item->IsDirectory())
		{
			FileSystem::Delete_Directory(item->GetPath());
			is_update = true;
		}
		else
		{
			FileSystem::Delete_File(item->GetPath());
			is_update = true;
		}
	}

	ImGui::Separator();

	if (ImGui::MenuItem("Open in file explorer"))
		FileSystem::OpenDirectoryWindow(item->GetPath());

	ImGui::EndPopup();
}

auto FileDialog::SetCurrentPath(const std::string & path) -> const bool
{
	if (!FileSystem::IsDirectory(path))
		return false;

	current_directory = path;
	return true;
}

auto FileDialog::UpdateDirectory(const std::string & path) -> const bool
{
	if (!FileSystem::IsDirectory(path))
	{
		//LOG_ERROR("Invalid parameter");
		return false;
	}
	
	items.clear();
	items.shrink_to_fit();
	

	//Get directories
	auto directories = FileSystem::GetDirectoriesInDirectory(path);
	for (const auto& directory : directories)
		items.emplace_back(directory, *IconProvider::Get().Load(FileSystem::ToWString(directory), IconType::Thumbnail_Folder));

	//Get files
	std::vector<std::string> files;
	switch (filter)
	{
	case FileDialogFilter::All:
		files = FileSystem::GetFilesInDirectory(path);
		for (const auto& file : files)
			items.emplace_back(file, *IconProvider::Get().Load(FileSystem::ToWString(file), IconType::Custom));
		break;
	case FileDialogFilter::Scene:
		//TODO :
		break;
	case FileDialogFilter::Model:
		//TODO :
		break;
	}

	if (filterName != "")
	{
		items.clear();

		auto directories = FileSystem::GetDirectoriesInDirectory(path);
		for (const auto& directory : directories)
			items.emplace_back(directory, *IconProvider::Get().Load(FileSystem::ToWString(directory), IconType::Thumbnail_Folder));

		files = FileSystem::GetFilesInDirectory(path);
		for (const auto& file : files)
		{
			if (filterName.find("!") != std::string::npos)
			{
				filterName.erase(0,1);
				if (!FileSystem::IsSupportedFilter(std::string(filterName), file))
					items.emplace_back(file, *IconProvider::Get().Load(FileSystem::ToWString(file), IconType::Custom));
				filterName = "!" + filterName;
			}
			else
			{
				if (FileSystem::IsSupportedFilterFile(std::string(filterName), file))
					items.emplace_back(file, *IconProvider::Get().Load(FileSystem::ToWString(file), IconType::Custom));
			}
		}
	}

	return true;
}

void FileDialog::EmptyAreaMenu()
{
	if (ImGui::IsMouseClicked(1) && _FileDialog::is_hovered_window && !_FileDialog::is_hovered_item)
		ImGui::OpenPopup("##Content_Menu");

	if (!ImGui::BeginPopup("##Content_Menu"))
		return;

	if (ImGui::MenuItem("CreateShaderResourceView folder"))
	{
		FileSystem::Create_Directory(current_directory + "New_folder");
		is_update = true;
	}

	if (ImGui::MenuItem("Open directory in explorer"))
		FileSystem::OpenDirectoryWindow(current_directory);

	ImGui::EndPopup();
}
