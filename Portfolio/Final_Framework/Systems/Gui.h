#pragma once

struct GuiText
{
	D3DXVECTOR2 Position;
	D3DXCOLOR Color;
	std::string Content;

	GuiText()
	{

	}
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

class Gui
{
public:
	static void Create();
	static void Delete();

	static Gui* Get();

	LRESULT MsgProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	void Resize();

	void Update();
	void Render();

	void RenderText(GuiText& text);
	void RenderText(float x, float y, std::string content);
	void RenderText(float x, float y, float r, float g, float b, std::string content);
	void RenderText(D3DXVECTOR2 position, D3DXCOLOR color, std::string content);

private:
	Gui();
	~Gui();

private:
	void ApplyStyle();

private:
	static Gui* instance;

	std::vector<GuiText> texts;

	bool bShowDemo;
};