#pragma once

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	void Update();
	void Render(UINT _instanceCount);

	void SetShader(Shader* _shader);
	void Pass(UINT _val) { pass = _val; }

protected:
	virtual void Create() = 0;

public:
	struct VertexMesh
	{
		Vector3 Position;
		Vector2 Uv;
		Vector3 Normal;
		Vector3 Tangent;

		VertexMesh()
		{
			Position = Vector3(0, 0, 0);
			Uv = Vector2(0, 0);
			Normal = Vector3(0, 0, 0);
			Tangent = Vector3(0, 0, 0);
		}

		VertexMesh(float x, float y, float z, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz)
		{
			Position = Vector3(x, y, z);
			Uv = Vector2(u, v);
			Normal = Vector3(nx, ny, nz);
			Tangent = Vector3(tx, ty, tz);
		}
	};

protected:
	Shader* shader;
	UINT pass = 0;

	PerFrame* perFrame = NULL;

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	VertexMesh* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;
};