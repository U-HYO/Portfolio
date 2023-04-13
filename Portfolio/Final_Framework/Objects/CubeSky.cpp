#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(std::wstring _file, Shader* shader)
	: shader(shader)
	, IComponent(nullptr, nullptr)
{
	if(this->shader == NULL)
	{
		this->shader = Debug_new Shader(L"17_CubeSky.fxo");
		bCreateShader = true;
	}

	sphere = Debug_new MeshRender(this->shader, Debug_new MeshSphere(0.5f));
	sphere->AddTransform();
	sphere->GetTransform(0)->SetScale(50, 50, 50);
	sphere->Pass(0);

	_file = L"../../_Textures/" + _file;

	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), _file.c_str(), NULL, NULL, &srv, NULL
	));

	sSrv = this->shader->AsSRV("SkyCubeMap");
	
}

CubeSky::CubeSky(Actor * _actor, Transform * _transform)
	: IComponent(_actor, _transform)
{
}

CubeSky::~CubeSky()
{
	if (bCreateShader == true)
		SAFE_DELETE(shader);

	SAFE_DELETE(sphere);
	SAFE_RELEASE(srv);
}

void CubeSky::Update()
{
	UINT camindex = Context::Get()->GetCameraIndex();
	Vector3 pos = Context::Get()->GetCamera(camindex)->GetTransform()->GetWorldPosition();
	sphere->GetTransform(0)->SetPosition(pos);

	
	Gizmo::Get()->SetTransform(sphere->GetTransform(0));

	sphere->UpdateTransforms();
	sphere->Update();

	transform = sphere->GetTransform(0);

}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	sphere->Pass(pass);
	sphere->Render();
}

void CubeSky::OnInit()
{
}

void CubeSky::OnStart()
{
}

void CubeSky::OnUpdate()
{
	Update();
}

void CubeSky::OnRender()
{
	Render();
}

void CubeSky::OnStop()
{
}

void CubeSky::OnDestroy()
{
}

void CubeSky::SetShader(Shader * shader)
{
	this->shader = shader;
	if (this->shader == NULL)
	{
		this->shader = Debug_new Shader(L"17_CubeSky.fxo");
		bCreateShader = true;
	}

	sphere = Debug_new MeshRender(this->shader, Debug_new MeshSphere(0.5f));
	sphere->AddTransform();
	sphere->GetTransform(0)->SetScale(500, 500, 500);
	sphere->Pass(0);
}

void CubeSky::SetCubeSky(std::wstring _file)
{
	//_file = L"../../_Textures/" + _file;
	//_file = L"../../_Assets/CubeMaps/" + _file + L".dds";
	_file = L"../../_Assets/CubeMaps/" + _file;

	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), _file.c_str(), NULL, NULL, &srv, NULL
	));

	sSrv = shader->AsSRV("SkyCubeMap");
}
