#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "FrameworkDemo/FrameworkDemo.h"
#include "FrameworkDemo/CollisionDemo.h"
#include "FrameworkDemo/BillboardDemo.h"
#include "FrameworkDemo/WeatherDemo.h"

#include "LightingDemo/LightingDemo.h"
#include "LightingDemo/NormalMapDemo.h"
#include "LightingDemo/AreaLightDemo.h"

#include "PostEffectDemo/RTVDemo.h"
#include "PostEffectDemo/ColorToneDemo.h"
#include "PostEffectDemo/MRTDemo.h"
#include "PostEffectDemo/BlurDemo.h"
#include "PostEffectDemo/BloomDemo.h"

#include "ObjectDemo/EnvCubeDemo.h"
#include "ObjectDemo/ProjectorDemo.h"
#include "ObjectDemo/DepthDemo.h"
#include "ObjectDemo/DepthBufferDemo.h"
#include "ObjectDemo/ShadowDemo.h"
#include "ObjectDemo/ShadowFrameworkDemo.h"
#include "ObjectDemo/FrusutmDemo.h"
#include "ObjectDemo/ScatteringDemo.h"
#include "ObjectDemo/RefractionDemo.h"
#include "ObjectDemo/ReflectionDemo.h"
#include "ObjectDemo/WaterDemo.h"

#include "Tessellation/TessellationTriangleDemo.h"
#include "Tessellation/TessellationRectDemo.h"
#include "Tessellation/TessellationRectDistanceDemo.h"
#include "Tessellation/TessellationLODDemo.h"

void Main::Init()
{
//	Push(Debug_new BillboardDemo);
	//Push(Debug_new AreaLightDemo);
	//Push(Debug_new FrusutmDemo);
	//Push(Debug_new TessellationLODDemo);
	//Push(Debug_new ScatteringDemo);
	//Push(Debug_new RefractionDemo);
	//Push(Debug_new ReflectionDemo);
	
	//Push(Debug_new EnvCubeDemo);
	Push(Debug_new WaterDemo);

	//Push(Debug_new CollisionDemo);
	//Push(Debug_new WeatherDemo);
	//Push(Debug_new BloomDemo);
	//Push(Debug_new BlurDemo);
	//Push(Debug_new ColorToneDemo);
	//Push(Debug_new MRTDemo);
	
}

void Main::Release()
{
	for (IExecute* exe : executes)
	{
		exe->Release();
		SAFE_DELETE(exe);
	}
	executes.clear();
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

	//_CrtSetBreakAlloc(507);
#endif
	D3DDesc desc;
	desc.AppName = L"Unit_Test";
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