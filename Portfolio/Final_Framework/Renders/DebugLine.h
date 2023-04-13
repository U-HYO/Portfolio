#pragma once
#define MAX_DEBUG_LINE 150'000

class DebugLine
{
public:
	friend class Window;
private:
	static void Create();
	static void Delete();

public:
	static DebugLine* Get();

public:
	void RenderLine(Vector3& _start, Vector3& _end);
	void RenderLine(Vector3 & start, Vector3 & end, float r, float g, float b);
	void RenderLine(Vector3& _start, Vector3& _end, float _r, float _g, float _b, float _a);
	void RenderLine(Vector3& _start, Vector3& _end, Color& _color);

	void RenderLine(float _x, float _y, float _z, float _x2, float _y2, float _z2);
	void RenderLine(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b);
	void RenderLine(float _x, float _y, float _z, float _x2, float _y2, float _z2, float _r, float _g, float _b, float _a);
	void RenderLine(float _x, float _y, float _z, float _x2, float _y2, float _z2, Color& _color);


private:
	void Update();
	void Render();

private:
	DebugLine();
	~DebugLine();

private:
	static DebugLine* instance;

	struct VertexLine
	{
		Vector3 Position;
		Color Color;
	};

private:
	Shader* shader;

	class PerFrame* perFrame;
	class Transform* transform;
	
	ID3D11Buffer* vertexBuffer;
	VertexLine* vertices;

	UINT drawCount = 0;

};
