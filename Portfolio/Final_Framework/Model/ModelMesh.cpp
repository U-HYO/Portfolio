#include "Framework.h"
#include "ModelMesh.h"

ModelBone::ModelBone()
{
}

ModelBone::~ModelBone()
{
}

///////////////////////////////////////////////////////////////////////////////

ModelMesh::ModelMesh()
{
	boneBuffer = Debug_new ConstantBuffer(&boneDesc, sizeof(BoneDesc));
}

ModelMesh::~ModelMesh()
{
	//SAFE_DELETE(transform);
	SAFE_DELETE(perFrame);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);

	SAFE_DELETE(boneBuffer);

	for (ModelMeshPart* part : meshParts)
		SAFE_DELETE(part);

}

void ModelMesh::Binding(Model * _model)
{//데이터를 읽고 그 데이터로 정점버퍼 인덱스버퍼 생성
	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(Model::VertexModel));
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);

	for (ModelMeshPart* part : meshParts)
		part->SetBinding(_model);

}

void ModelMesh::Pass(UINT _val)
{
	for (ModelMeshPart* part : meshParts)
		part->SetPass(_val);

}

void ModelMesh::SetShader(Shader * _shader)
{
	shader = _shader;

	SAFE_DELETE(perFrame);
	perFrame = Debug_new PerFrame(_shader);

	sBoneBuffer = _shader->AsConstantBuffer("CB_Bones");

	for (ModelMeshPart* part : meshParts)
		part->SetShader(_shader);

}

void ModelMesh::Update()
{
	boneDesc.BoneIndex = boneIndex;

	perFrame->Update();

	for (ModelMeshPart* part : meshParts)
		part->Update();

}

void ModelMesh::Render()
{
	boneBuffer->Render();
	sBoneBuffer->SetConstantBuffer(boneBuffer->GetBuffer());
	
	perFrame->Render();
	
	vertexBuffer->Render();
	indexBuffer->Render();
	
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (ModelMeshPart* part : meshParts)
		part->Render();
}

void ModelMesh::Render(UINT _drawCount)
{
	boneBuffer->Render();
	sBoneBuffer->SetConstantBuffer(boneBuffer->GetBuffer());

	perFrame->Render();
	//transform->Render();

	vertexBuffer->Render();
	indexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (ModelMeshPart* part : meshParts)
		part->Render(_drawCount);
		

}

void ModelMesh::Render(Transform _transform)
{
	//if (!_transform.GetRender())
	//	return;
	//
	//
	//boneBuffer->Render();
	//sBoneBuffer->SetConstantBuffer(boneBuffer->GetBuffer());
	//
	//perFrame->Render();
	////transform->Render();
	//
	//vertexBuffer->Render();
	//indexBuffer->Render();
	//
	//D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//for (ModelMeshPart* part : meshParts)
	//	part->Render(_drawCount);

}

void ModelMesh::Transforms(Matrix * _transforms)
{
	//memcpy(boneDesc.Transform, _transforms, sizeof(Matrix)* MAX_MODEL_TRANSFORMS);
}

void ModelMesh::SetTransform(Transform * _transform)
{
	//transform->Set(_transform);
}

///////////////////////////////////////////////////////////////////////////////

ModelMeshPart::ModelMeshPart()
{
}

ModelMeshPart::~ModelMeshPart()
{
	SAFE_DELETE(material);
}

void ModelMeshPart::Update()
{
}

void ModelMeshPart::Render()
{
	material->Render();

	shader->DrawIndexed(0, pass, indexCount, startIndex);
}

void ModelMeshPart::Render(UINT _drawCount)
{
	material->Render();

	shader->DrawIndexedInstanced(0, pass, indexCount, _drawCount, startIndex);
}

void ModelMeshPart::SetBinding(Model * model)
{
	Material* srcMaterial = model->MaterialByName(materialName);

	material = Debug_new Material();
	material->Ambient(srcMaterial->Ambient());
	material->Diffuse(srcMaterial->Diffuse());
	material->Specular(srcMaterial->Specular());
	material->Emissive(srcMaterial->Emissive());

	if (srcMaterial->DiffuseMap() != NULL)
		material->DiffuseMap(srcMaterial->DiffuseMap()->GetFile());

	if (srcMaterial->SpecularMap() != NULL)
		material->SpecularMap(srcMaterial->SpecularMap()->GetFile());

	if (srcMaterial->NormalMap() != NULL)
		material->NormalMap(srcMaterial->NormalMap()->GetFile());
}


void ModelMeshPart::SetShader(Shader * _shader)
{
	shader = _shader;

	material->SetShader(_shader);
}
