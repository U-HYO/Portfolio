#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "PlayerDemo/PlayerDemo.h"
//#include "Scene/Terrain/"
#include "UI/UI.h"

void Main::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(50, 2);

	EditorHelper::Create();
	//Push(Debug_new PlayerDemo);
	Push(Debug_new UI);
}

void Main::Release()
{
	for (IExecute* exe : executes)
	{
		exe->Release();
		SAFE_DELETE(exe);
	}

	EditorHelper::Delete();
}

void Main::Update()
{
	EditorHelper::Update();
	for (IExecute* exe : executes)
	{
		exe->Update();
	}
}

void Main::PreRender()
{
	EditorHelper::PreRender();
	for (IExecute* exe : executes)
	{
		exe->PreRender();
	}
}

void Main::Render()
{
	EditorHelper::Render();
	for (IExecute* exe : executes)
	{
		exe->Render();
	}
}

void Main::PostRender()
{
	EditorHelper::PostRender();
	for (IExecute* exe : executes)
	{
		exe->PostRender();
	}
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
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc();
#endif
	D3DDesc desc;
	desc.AppName = L"Work";
	desc.Instance = _instance;
	desc.bFullScreen = false;
	desc.bVsync = true;
	desc.Handle = NULL;
	desc.Width = 1600;
	desc.Height = 900;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	D3D::SetDesc(desc);

	Main* main = Debug_new Main();
	
	
	WPARAM wParam = Window::Run(main);
	



	SAFE_DELETE(main);
	_CrtDumpMemoryLeaks();

	return wParam;
}