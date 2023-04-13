#include "stdafx.h"
#include "FrusutmDemo.h"

void FrusutmDemo::Init()
{
	gridShader = Debug_new Shader(L"36_Lighting.fxo");

	floor = Debug_new Material(gridShader);
	floor->DiffuseMap(L"White.png");

	plane = Debug_new MeshRender(gridShader, Debug_new MeshPlane());
	plane->AddTransform()->SetScale(10, 10, 10);
	plane->UpdateTransforms();
	plane->Pass(0);
	//¹Ù´Ú


	camera = Debug_new Fixity();
	camera->GetTransform()->SetPosition(0, 0, -50);
	perspective = Debug_new Perspective(1024, 768, 1, zFar, Math::PI * fov);
	//frustum = Debug_new Frustum(camera, perspective);
	//frustum = Debug_new Frustum();
	frustum = Debug_new Frustum(NULL, perspective);

	shader = Debug_new Shader(L"16_Mesh.fx");
	perFrame = Debug_new PerFrame(shader);

	red = Debug_new Material(shader);
	red->DiffuseMap("Red.png");

	for (float z = -50.0f; z <= 50.0f; z += 10)
	{
		for (float y = -50.0f; y <= 50.0f; y += 10)
		{
			for (float x = -50.0f; x <= 50.0f; x += 10)
			{
				Transform* transform = Debug_new Transform(shader);
				transform->SetPosition(x, y, z);
				transforms.push_back(transform);
			}
		}
	}

	CreateMeshData();

	modelShader = Debug_new Shader(L"36_Lighting.fxo");
	model = Debug_new ModelRender(modelShader);
	model->ReadMesh(L"Airplane/Airplane");
	model->ReadMaterial(L"Airplane/Airplane");

	for (float z = -100.0; z <= 100.0f; z += 30)
	{
		for (float y = -100.0; y <= 100.0f; y += 30)
		{
			for (float x = -100.0; x <= 100.0f; x += 30)
			{
				Transform* transform = model->AddTransform();
				transform->SetPosition(x, y, z);
				transform->SetScale(0.004f, 0.004f, 0.004f);
				transform->SetRotation(0, Math::PI * 0.25f, 0);
			}
		}
	}
	model->UpdateTransforms();

	paladin = Debug_new ModelAnimator(modelShader);
	paladin->ReadMesh(L"Paladin/Paladin");
	paladin->ReadMaterial(L"Paladin/Paladin");

	paladin->ReadClip(L"Paladin/BlockIdle");
	paladin->ReadClip(L"Paladin/Idle");
	paladin->ReadClip(L"Paladin/Rolling");
	paladin->ReadClip(L"Paladin/Run");
	paladin->ReadClip(L"Paladin/Walk");

	for (float z = -50.0; z <= 50.0f; z += 30)
	{
		for (float y = -50.0; y <= 50.0f; y += 30)
		{
			for (float x = -50.0; x <= 50.0f; x += 30)
			{
				Transform* transform = paladin->AddTransform();
				transform->SetPosition(x, y, z);
				transform->SetScale(0.04f, 0.04f, 0.04f);
				paladin->PlayTweenMode(0, 0, 1.0f);
			}
		}
	}
	paladin->UpdateTransforms();
}

void FrusutmDemo::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(gridShader);
	SAFE_DELETE(floor);
	SAFE_DELETE(plane);
	SAFE_DELETE(camera);
	SAFE_DELETE(perspective);
	SAFE_DELETE(frustum);
	SAFE_DELETE(perFrame);
	SAFE_DELETE(red);
	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
	
	for (auto a : transforms)
		SAFE_DELETE(a);

	SAFE_DELETE(paladin);
	SAFE_DELETE(modelShader);
}

void FrusutmDemo::Update()
{
	ImGui::InputFloat("Far", &zFar, 1.0f);
	ImGui::InputFloat("Fov", &fov, 1e-3f);
	perspective->Set(1024, 768, 1, zFar, Math::PI * fov);

	frustum->Update();

	perFrame->Update();
	plane->Update();

	shader->AsVector("LightDirection")->SetFloatVector(Vector3(-1, -1, +1));
	
	model->Update();

	paladin->Update();
}


void FrusutmDemo::Render()
{
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	floor->Render();
	plane->Render();

	perFrame->Render();
	vertexBuffer->Render();
	indexBuffer->Render();

	red->Render();

	UINT drawCount = 0;
	Vector3 position;
	for (Transform* transform : transforms)
	{
		transform->GetPosition(&position);

		if (frustum->CheckPoint(position))
		{
			transform->Update();
			transform->Render();

			shader->DrawIndexed(0, 0, 36);

			drawCount++;
		}

	}
	std::string str = std::to_string(drawCount) + " / " + std::to_string(transforms.size());
	Gui::Get()->RenderText(10, 60, 1, 0, 0, str);


	static float distance = 0.0f;
	ImGui::SliderFloat("Distance", &distance, 0.0f, 20.0f, "%.3f", 0.1f);
	//Context::Get()->GetClipping() = Plane(1, 0, 0, distance);

	//Context::Get()->GetCulling()[0] = Plane(1, 0, 0, distance);

	Plane planes[4];
	frustum->GetPlanes(Context::Get()->GetCulling(), 4);
	


	model->Pass(1);
	model->Render();

	paladin->Pass(2);
	paladin->Render();

}

void FrusutmDemo::CreateMeshData()
{
	std::vector<Mesh::VertexMesh> v;


	float w = 0.5f;
	float h = 0.5f;
	float d = 0.5f;

	//Front
	v.push_back(Mesh::VertexMesh(-w, -h, -d, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, -d, 1, 1, 0, 0, -1, 1, 0, 0));

	//Back
	v.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, +d, 0, 0, 0, 0, 1, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, +h, +d, 1, 0, 0, 0, 1, -1, 0, 0));

	//Top
	v.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 1, 0, 1, 0, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 1, 0, 1, 0, 1, 0, 0));

	//Bottom
	v.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, 0, -1, 0, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 0, -1, 0, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 0, 0, -1, 0, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 0, 0, -1, 0, -1, 0, 0));

	//Left
	v.push_back(Mesh::VertexMesh(-w, -h, +d, 0, 1, -1, 0, 0, 0, 0, -1));
	v.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(Mesh::VertexMesh(-w, +h, -d, 1, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, -1, 0, 0, 0, 0, -1));

	//Right
	v.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 1, 0, 0, 0, 0, 1));
	v.push_back(Mesh::VertexMesh(+w, +h, -d, 0, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(Mesh::VertexMesh(+w, -h, +d, 1, 1, 1, 0, 0, 0, 0, 1));


	Mesh::VertexMesh* vertices = Debug_new Mesh::VertexMesh[v.size()];
	UINT vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<Mesh::VertexMesh *>(vertices, vertexCount)
	);

	UINT indexCount = 36;
	UINT* indices = Debug_new UINT[indexCount]
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};


	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(Mesh::VertexMesh));
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);


}
