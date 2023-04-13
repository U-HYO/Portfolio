#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "./Demo/MapEditor.h"


void Main::Init()
{
	Push(Debug_new MapEditor());

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
	for (IExecute* exe : executes)
	{
		exe->PreRender();
	}
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
#ifdef _DEBUG
	//	������ �� �����޸𸮸� üũ�ϴ� ��ó����
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//	MainClass �� �׽�Ʈ������ int* pInt �� �����Ҵ��ϰ� ������ �ʾҴ�
	//	�� ���¿��� F5 (���� �����) ���� �����ϰԵǸ� '���' �ǿ�
	/*
		Detected memory leaks!
		Dumping objects ->
		(.. ���� ������Ʈ ����) : {232} normal block at 0x007D3C58, 4byte long.
		�̶�� ���ڿ��� ���´�.

		�� ���ڿ��� ����Ŭ���ϸ� �޸𸮸��� �߻��� �ڵ带 ����Ų��.
		�㳪 ��ġ�� ��Ÿ���� �ʴ� ��쵵 �ִµ� �߰�ȣ {232} ������ ��ȣ�� �̿��� ã�� �� �ִ�

		���� _CrtSetBreakAlloc() �ȿ� 232 ��� ��ȣ�� �ְ� ���� ����׸� �����ϸ�
		�ش� �޸𸮸��� �߻��� �ڵ忡�� ������� �����ȴ�
	*/
	//_CrtSetBreakAlloc();

#endif
	D3DDesc desc;
	desc.AppName = L"Terrain_Editor";
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