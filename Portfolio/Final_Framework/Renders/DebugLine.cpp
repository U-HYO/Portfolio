#include "Framework.h"
#include "DebugLine.h"

DebugLine* DebugLine::instance = NULL;

void DebugLine::Create()
{
	assert(instance == NULL);

	instance = Debug_new DebugLine();
}

void DebugLine::Delete()
{
	SAFE_DELETE(instance);
}

DebugLine * DebugLine::Get()
{
	assert(instance != NULL);

	return instance;
}

void DebugLine::RenderLine(Vector3 & _start, Vector3 & _end)
{
	RenderLine(_start, _end, Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(Vector3 & _start, Vector3 & _end, float _r, float _g, float _b, float _a)
{
	RenderLine(_start, _end, Color(_r, _g, _b, _a));
}

void DebugLine::RenderLine(Vector3 & _start, Vector3 & _end, Color & _color)
{
	vertices[drawCount].Color = _color;
	vertices[drawCount++].Position = _start;

	vertices[drawCount].Color = _color;
	vertices[drawCount++].Position = _end;
}

void DebugLine::RenderLine(float _x, float _y, float _z, float _x2, float _y2, float _z2)
{
	RenderLine(Vector3(_x, _y, _z), Vector3(_x2, _y2, _z2), Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(float _x, float _y, float _z, float _x2, float _y2, float _z2, float _r, float _g, float _b, float _a)
{
	RenderLine(Vector3(_x, _y, _z), Vector3(_x2, _y2, _z2), Color(_r, _g, _b, _a));
}

void DebugLine::RenderLine(float _x, float _y, float _z, float _x2, float _y2, float _z2, Color & _color)
{
	RenderLine(Vector3(_x, _y, _z), Vector3(_x2, _y2, _z2), _color);
}

void DebugLine::Update()
{
	perFrame->Update();
	transform->Update();
}

void DebugLine::Render()
{
	if (drawCount < 1) return;

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(VertexLine) * drawCount, 0);

	UINT stride = sizeof(VertexLine);
	UINT offset = 0;
	
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	perFrame->Render();
	transform->Render();

	shader->Draw(0, 0, drawCount);
	drawCount = 0;
	ZeroMemory(vertices, sizeof(VertexLine) * MAX_DEBUG_LINE);
}

DebugLine::DebugLine()
{
	shader = Debug_new Shader(L"13_DebugLine.fxo");

	vertices = Debug_new VertexLine[MAX_DEBUG_LINE];
	ZeroMemory(vertices, sizeof(VertexLine) * MAX_DEBUG_LINE);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexLine) * MAX_DEBUG_LINE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices; // V-RAM으로 복사될 데이터

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
	
	transform = Debug_new Transform(shader);
	perFrame = Debug_new PerFrame(shader);
}

DebugLine::~DebugLine()
{
	SAFE_DELETE(shader);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(transform);
	SAFE_DELETE(perFrame);
}

