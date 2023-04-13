#include "stdafx.h"
#include "TessellationRectDistanceDemo.h"

void TessellationRectDistanceDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(00, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(0, 0, -5);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(10, 0);


	shader = Debug_new Shader(L"56_RectDistance.fxo");

	transform = Debug_new Transform(shader);
	perFrame = Debug_new PerFrame(shader);

	Vertex vertices[4];
	vertices[0].Position = Vector3(-0.9f, -0.9f, 0.0f);
	vertices[1].Position = Vector3(-0.9f, +0.9f, 0.0f);
	vertices[2].Position = Vector3(+0.9f, -0.9f, 0.0f);
	vertices[3].Position = Vector3(+0.9f, +0.9f, 0.0f);

	vertexBuffer = Debug_new VertexBuffer(vertices, 4, sizeof(Vertex));

}

void TessellationRectDistanceDemo::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(vertexBuffer);
}

void TessellationRectDistanceDemo::Update()
{
	static Vector2 distance = Vector2(1, 1000);
	ImGui::SliderFloat2("Distance", distance, 0, 1000);

	shader->AsVector("Distance")->SetFloatVector(distance);

	transform->Update();
	perFrame->Update();

}


void TessellationRectDistanceDemo::Render()
{
	transform->Render();
	perFrame->Render();

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);	// 제어점 4개

	shader->Draw(0, 0, 4);
}