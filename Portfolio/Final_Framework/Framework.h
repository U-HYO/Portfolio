#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <assert.h>

/*
	crtdbg.h ?
	����ũ�μ���Ʈ���� �����ϴ� CRT ���̺귯��
	������ �Ҵ��� �޸𸮸� ���� �����ϰ� �������� ���Ͽ��� ��� �߻��ϴ� ����
	(�޸� �������� ������ ��� �޸� ���� ���� ���� ���α׷� �浹 ���.. )
	�� �����ϰ��� '�޸� ���� Ž�� ���' �� ����Ѵ� (���� �޸�)

	����� �߰��ϰ� <crtdbg.h>
	���� ��ó����� ���� �ڵ�, ���� ��ġ, �ڵ� ���� �� ǥ���� �� �ֵ��� ����
*/
#include <crtdbg.h>
#ifdef _DEBUG
#define Debug_new new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif


//==========================================

//STL
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <map>
#include <unordered_map>
#include <functional> //�Լ�������
#include <iterator>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <fstream>
#include <any>

//Direct3D
#include <dxgi1_2.h> //�׷���ī�����
#include <d3dcommon.h> 
#include <d3dcompiler.h> //shader�����Ϸ�
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Effects11d.lib")

//ImGui
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/ImGuizmo.h>
#pragma comment(lib, "ImGui/imgui.lib")

//DirectXTex
#include <DirectXTex.h> // DirectX �̹��� Ȯ�������ϵ� �߰�
#pragma comment(lib, "directxtex.lib")

#define Check(hr) { assert(SUCCEEDED(hr)); }
#define Super __super //��ӹ޾��� ���� �θ�

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE(p){ if(p != nullptr){ delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = nullptr; } }

typedef D3DXVECTOR2 Vector2;
typedef D3DXVECTOR3 Vector3;
typedef D3DXVECTOR4 Vector4;
typedef D3DXCOLOR Color;
typedef D3DXMATRIX Matrix;
typedef D3DXQUATERNION Quaternion;
typedef D3DXPLANE Plane;

#include "Systems/D3D.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Systems/Time.h"
#include "Systems/Gui.h"
#include "Systems/Thread.h"

#include "Viewer/Viewport.h"
#include "Viewer/Projection.h"
#include "Viewer/Perspective.h"
#include "Viewer/Orthographic.h"
#include "Viewer/Camera.h"
#include "Viewer/FreeCamera.h"
#include "Viewer/FallowCamera.h"
#include "Viewer/RenderTarget.h"
#include "Viewer/DepthStencil.h"
#include "Viewer/Fixity.h"


#include "Renders/Shader.h"
#include "Renders/Texture.h"
#include "Renders/Context.h"
#include "Renders/Gizmo.h"
#include "Renders/DebugLine.h"
#include "Renders/Buffers.h"
#include "Renders/Lighting.h"
#include "Renders/PerFrame.h"
#include "Renders/Transform.h"
#include "Renders/Renderer.h"
#include "Renders/Material.h"

#include "Utilities/Math.h"
#include "Utilities/String.h"
#include "Utilities/Path.h"
#include "Utilities/FileSystem.h"
#include "Utilities/GUIDGenerator.h"

#include "Meshes/Mesh.h"
#include "Meshes/MeshQuad.h"
#include "Meshes/MeshPlane.h"
#include "Meshes/MeshCube.h"
#include "Meshes/MeshCylinder.h"
#include "Meshes/MeshSphere.h"
#include "Meshes/MeshRender.h"

#include "Model/Model.h"
#include "Model/ModelMesh.h"
#include "Model/ModelRender.h"
#include "Model/ModelClip.h"
#include "Model/ModelAnimator.h"

#include "Objects/Render2D.h"
#include "Objects/CubeSky.h"
#include "Objects/Collider.h"
#include "Objects/Billboard.h"
#include "Objects/PostEffect.h"
#include "Objects/EnvCube.h"
#include "Objects/Projector.h"
#include "Objects/Shadow.h"
#include "Objects/Frustum.h"
#include "Objects/Refraction.h"
#include "Objects/Reflection.h"

#include "Enviroment/Terrain.h"
#include "Enviroment/Rain.h"
#include "Enviroment/Snow.h"
#include "Enviroment/TerrainLOD.h"
#include "Enviroment/Scattering/Sky.h"
#include "Enviroment/Water.h"

#include "Enviroment/Scattering/Cloud.h"
#include "Enviroment/Scattering/Dome.h"
#include "Enviroment/Scattering/Moon.h"


#include "Particle/ParticleData.h"
#include "Particle/Particle.h"

#include "Component/Actor.h"

#define _KEYCODE Keyboard::Get()
#define _TIMER Time::Get()