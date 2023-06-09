#include "Framework.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad()
{
}

MeshQuad::~MeshQuad()
{
}

void MeshQuad::Create()
{
	float w = 0.5f;
	float h = 0.5f;

	std::vector<VertexMesh> v;
	v.push_back(VertexMesh(-w, -h, 0, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(VertexMesh(-w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(VertexMesh(+w, -h, 0, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(VertexMesh(+w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

	vertices = Debug_new VertexMesh[v.size()];
	vertexCount = v.size();

	std::copy(v.begin(), v.end(), stdext::checked_array_iterator<VertexMesh*>(vertices, vertexCount));
	
	indexCount = 6;
	indices = Debug_new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };
}
