#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "ExportFile.h"
#include "./Demo/ModelDemo.h"
#include "./Demo/AnimationDemo.h"
#include "./Demo/MeshDemo.h"

void Main::Init()
{
	//Push(Debug_new ExportFile());
	//Push(Debug_new ModelDemo());
	Push(Debug_new AnimationDemo);
	//Push(Debug_new MeshDemo());

}

void Main::Release()
{
	for (IExecute* exe : executes)
	{
		exe->Release();
		SAFE_DELETE(exe);
	}

}

void Main::Update()
{
	for (IExecute* exe : executes)
	{
		exe->Update();
	}
}

void Main::PreRender()
{
}

void Main::Render()
{

	for (IExecute* exe : executes)
	{
		exe->Render();
	}
}

void Main::PostRender()
{
}

void Main::ResizeScreen()
{
}

void Main::Push(IExecute * _exe)
{
	executes.emplace_back(_exe);

	_exe->Init();
}

int WINAPI WinMain(HINSTANCE _instance, HINSTANCE _prevInstance, LPSTR _param, int _command)
{
	D3DDesc desc;
	desc.AppName = L"Model_Editor";
	desc.Instance = _instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1600;
	desc.Height = 900;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	D3D::SetDesc(desc);

	Main* main = Debug_new Main();

	WPARAM wParam = Window::Run(main);
	SAFE_DELETE(main);

	return wParam;
}