#pragma once

enum class FileDialogType : UINT
{
	Browser,
	FileSelection,
};

enum class FileDialogOperation : UINT
{
	Open,
	Load,
	Save,
};

enum class FileDialogFilter : UINT
{
	All,
	Scene,
	Model,
};

class FileDialogItem final
{
public:
	FileDialogItem(const std::string& path, const Thumbnail& thumbnail)
		: thumbnail(thumbnail)
		, path(path)
	{
		id = GUIDGenerator::Generate();
		is_directory = FileSystem::IsDirectory(path);
		label = FileSystem::GetFileNameFromPath(path);
	}

	auto GetPath() const -> const std::string& { return path; }
	auto GetLabel() const -> const std::string& { return label; }
	auto GetID() const -> const UINT& { return id; }
	Texture* GetTexture() { return thumbnail.texture; }
	auto IsDirectory() const -> const bool& { return is_directory; }
	auto GetTimeSinceLastClickedMs() const -> const float { return static_cast<float>(time_since_last_click.count()); }

	void Clicked()
	{
		const auto now = std::chrono::high_resolution_clock::now();
		time_since_last_click = now - last_click_time;
		last_click_time = now;
	}

private:
	Thumbnail thumbnail;
	UINT id;
	std::string path;
	std::string label;
	bool is_directory;
	std::chrono::duration<double, std::milli> time_since_last_click;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_click_time;
};

class FileDialog final
{
public:
	FileDialog
	(
		class Context* context,
		const FileDialogType& type,
		const FileDialogOperation& operation,
		const FileDialogFilter& filter,
		const bool& is_window
	);
	~FileDialog();

	auto GetType() const -> const FileDialogType& { return type; }
	auto GetFilter() const -> const FileDialogFilter& { return filter; }

	auto GetOperation() const -> const FileDialogOperation& { return operation; }
	void SetOperation(const FileDialogOperation& operation);

	auto Show(bool* is_visible, std::string* directory = nullptr, std::string* path = nullptr) -> const bool;

	void SetCallbackOnItemClicked(const std::function<void(const std::string&)>& callback);
	void SetCallbackOnItemDoubleClicked(const std::function<void(const std::string&)>& callback);

	void SetFilterName(std::string _filter) { filterName = _filter; }
	
	void SetExtraButtonName(std::string str) { extraButtonLabel = str; }
	bool GetExtraButton();

private:
	void ShowTop(bool* is_visible);
	void ShowMiddle();
	void ShowBottom(bool* is_visible);

	void ItemDrag(FileDialogItem* item) const;
	void ItemClick(FileDialogItem* item) const;
	void ItemMenu(FileDialogItem* item);

	auto SetCurrentPath(const std::string& path) -> const bool;
	auto UpdateDirectory(const std::string& path) -> const bool;
	void EmptyAreaMenu();
	
	void ExtraButton();
private:
	//class Context* context;
	FileDialogType type;
	FileDialogOperation operation;
	FileDialogFilter filter;
	std::string title;
	std::string current_directory;
	std::string input_box;
	std::vector<FileDialogItem> items;
	std::function<void(const std::string&)> callback_on_item_clicked;
	std::function<void(const std::string&)> callback_on_item_double_clicked;
	ImVec2 item_size;
	bool is_window;
	bool is_selection;
	bool is_update;

	std::string selectedItem;


	std::string filterName;

	bool extraButton;
	std::string extraButtonLabel;
};

//static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
//{
//	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
//}
//
//static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
//{
//	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
//}