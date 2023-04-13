#include "Framework.h"
#include "ModelRender.h"

ModelRender::ModelRender(Shader * _shader)
	:IComponent(nullptr, nullptr)
	, shader(_shader)
{
	model = Debug_new Model();

	sTransformsSRV = shader->AsSRV("TransformsMap");

	instanceBuffer = Debug_new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = Debug_new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);
}

ModelRender::ModelRender(Actor * _actor, Transform * _transform)
	:IComponent(_actor, _transform)
{
	model = Debug_new Model();
}

ModelRender::~ModelRender()
{
	SAFE_DELETE(model);

	for (Transform* transform : transforms)
		SAFE_DELETE(transform);

	SAFE_DELETE(instanceBuffer);
	SAFE_DELETE(instanceColorBuffer);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(transformsSRV);
}

void ModelRender::Update()
{
	if (texture == NULL)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();

	UpdateTransforms();
}

void ModelRender::Render()
{
	instanceBuffer->Render();
	instanceColorBuffer->Render();

	sTransformsSRV->SetResource(transformsSRV);

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render(transforms.size());
}

void ModelRender::ReadMesh(std::wstring _file)
{
	//_file = L"../../Objects/" + _file + L".mesh";
	model->ReadMesh(_file);
}

void ModelRender::ReadMaterial(std::wstring _file)
{
	//_file = L"../../_Textures/" + _file + L".material";
	model->ReadMaterial(_file);
}

void ModelRender::Pass(UINT _pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(_pass);
}

Transform * ModelRender::AddTransform()
{
	Transform* transform = Debug_new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	

	return transform;
}

void ModelRender::CreateTexture()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = MAX_MODEL_TRANSFORMS * 4;
	desc.Height = MAX_MODEL_INSTANCE;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;

	Matrix bones[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix parent;
			int parentIndex = bone->GetParentIndex();

			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];

			Matrix matrix = bone->GetTransform();
			bones[b] = parent;
			boneTransforms[i][b] = matrix * bones[b];
		}//for(b)
	}//for(i)

	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = boneTransforms;
	subResource.SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
	subResource.SysMemSlicePitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix) * MAX_MODEL_INSTANCE;

	Check(D3D::GetDevice()->CreateTexture2D(&desc, &subResource, &texture));


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Format = desc.Format;

	Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &transformsSRV));
}

void ModelRender::OnInit()
{
}

void ModelRender::OnStart()
{
}

void ModelRender::OnUpdate()
{
	Update();
}

void ModelRender::OnRender()
{
	Render();
}

void ModelRender::OnStop()
{
}

void ModelRender::OnDestroy()
{
}

void ModelRender::SetShader(Shader * _shader)
{
	shader = _shader;
	sTransformsSRV = shader->AsSRV("TransformsMap");

	instanceBuffer = Debug_new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = Debug_new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

}

void ModelRender::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
	{
		transforms[i]->Update();
		memcpy(worlds[i], transforms[i]->GetWorld(), sizeof(Matrix));
	}


	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);


	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);
}

void ModelRender::SetColor(UINT _instance, Color & _color)
{
	colors[_instance] = _color;
}
