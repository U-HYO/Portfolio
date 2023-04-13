#include "Framework.h"
#include "MeshRender.h"

MeshRender::MeshRender(Shader * _shader, Mesh * _mesh)
	: mesh(_mesh)
	, IComponent(nullptr, nullptr)
	, material(nullptr)
{
	Pass(0);
	_mesh->SetShader(_shader);

	instanceBuffer = Debug_new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = Debug_new VertexBuffer(colors, MAX_MESH_INSTANCE, sizeof(Color), 2, true);
}

MeshRender::MeshRender(Actor * _actor, Transform * _transform)
	: IComponent(_actor, _transform)
	, material(nullptr)
{
}

MeshRender::~MeshRender()
{
	for (Transform* transform : transforms)
		SAFE_DELETE(transform);

	SAFE_DELETE(instanceBuffer);
	SAFE_DELETE(instanceColorBuffer);
	
	SAFE_DELETE(mesh);

	SAFE_DELETE(material);
}

void MeshRender::Update()
{
	UpdateTransforms();
	mesh->Update();
}

void MeshRender::Render()
{
	instanceColorBuffer->Render();
	instanceBuffer->Render();

	mesh->Render(transforms.size());
}

Transform * MeshRender::AddTransform()
{
	Transform* transform = Debug_new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	return transform;
}

void MeshRender::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
	{
		transforms[i]->Update();
		memcpy(worlds[i], transforms[i]->GetWorld(), sizeof(Matrix));
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);

	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);


}

void MeshRender::SetColor(UINT _instance, Color & _color)
{
	colors[_instance] = _color;
}

void MeshRender::OnInit()
{
}

void MeshRender::OnStart()
{
}

void MeshRender::OnUpdate()
{
	Update();
}

void MeshRender::OnRender()
{
	if (material)
		material->Render();
	Render();
}

void MeshRender::OnStop()
{
}

void MeshRender::OnDestroy()
{
}

void MeshRender::SetMesh(Mesh * _mesh, Shader * _shader)
{
	mesh = _mesh;
	Pass(0);
	mesh->SetShader(_shader);

	instanceBuffer = Debug_new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = Debug_new VertexBuffer(colors, MAX_MESH_INSTANCE, sizeof(Color), 2, true);

	material = Debug_new Material(_shader);
	material->DiffuseMap("Stones.png");
	material->NormalMap("Stones_Normal.png");
	material->Specular(1, 1, 1, 20);
	material->SpecularMap("Stones_Specular.png");
	material->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

	materialName = "Mesh";

}

void MeshRender::SetMaterial(std::string _fileName)
{
	auto name = "Material/" + FileSystem::GetIntactFileNameFromPath(_fileName);
	material->DiffuseMap(name + ".png");
	material->NormalMap(name + "_Normal.png");
	material->Specular(1, 1, 1, 20);
	material->SpecularMap(name + "_Specular.png");
	material->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

	materialName = FileSystem::GetIntactFileNameFromPath(_fileName);
}