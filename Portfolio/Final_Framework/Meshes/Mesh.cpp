#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	SAFE_DELETE(perFrame);

	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
	
}

void Mesh::Update()
{
	perFrame->Update();
}

void Mesh::Render(UINT _instanceCount)
{
	if (vertexBuffer == NULL || indexBuffer == NULL)
	{
		Create();
		vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexMesh));
		indexBuffer = Debug_new IndexBuffer(indices, indexCount);
	}

	perFrame->Render();
	vertexBuffer->Render();
	indexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawIndexedInstanced(0, pass, indexCount, _instanceCount);
}

void Mesh::SetShader(Shader * _shader)
{
	shader = _shader;

	SAFE_DELETE(perFrame);
	perFrame = Debug_new PerFrame(_shader);

}
