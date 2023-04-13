#include "Framework.h"
#include "Cloud.h"

Cloud::Cloud(Shader * shader)
	: Renderer(shader)
{
	sSRV = shader->AsSRV("CloudMap");

	render2D = Debug_new Render2D();
	render2D->GetTransform()->SetPosition(400 + 100, D3D::Height() - 100, 0);
	render2D->GetTransform()->SetScale(200, 200, 1);


	//VertexTexture vertices[6];

	//vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	//vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	//vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	//vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	//vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	//vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	//vertices[0].Uv = Vector2(0, 1);
	//vertices[1].Uv = Vector2(0, 0);
	//vertices[2].Uv = Vector2(1, 1);
	//vertices[3].Uv = Vector2(1, 1);
	//vertices[4].Uv = Vector2(0, 0);
	//vertices[5].Uv = Vector2(1, 0);

	//vertexBuffer = Debug_new VertexBuffer(vertices, 6, sizeof(VertexTexture));

	/////////////////////////////////////

	VertexTexture* vertices = Debug_new VertexTexture[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;
	VertexTexture* tempVertices = Debug_new VertexTexture[vertexCount];
	indexCount = vertexCount;

	float quadSize, radius, constant, textureDelta;
	int i, j, index;
	float positionX, positionY, positionZ, tu, tv;

	quadSize = skyPlaneWidth / (float)skyPlaneResolution;
	radius = skyPlaneWidth / 2.0f;
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);
	//textureDelta = (float)textureRepeat / (float)skyPlaneResolution;
	textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	for (j = 0; j <= skyPlaneResolution; j++)
	{
		for (i = 0; i <= skyPlaneResolution; i++)
		{
			// Calculate the vertex coordinates.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// Calculate the texture coordinates.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;
			//tu = (float)skyPlaneResolution + i / (float)skyPlaneResolution;
			//tv = (float)skyPlaneResolution + j / (float)skyPlaneResolution;
			
			// Calculate the index into the sky plane array to add this coordinate.
			index = j * (skyPlaneResolution + 1) + i;

			// Add the coordinates to the sky plane array.
			vertices[index].Position = Vector3(positionX, positionY, positionZ);
			vertices[index].Uv = Vector2(tu, tv);
		}
	}
	
	index = 0;
	UINT index1 = 0;
	UINT index2 = 0;
	UINT index3 = 0;
	UINT index4 = 0;

	UINT* indices = Debug_new UINT[indexCount];
	for (j = 0; j < skyPlaneResolution; j++)
	{
		for (i = 0; i < skyPlaneResolution; i++)
		{
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i + 1);
			index3 = (j + 1) * (skyPlaneResolution + 1) + i;
			index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

			// Triangle 1 - Upper Left
			tempVertices[index].Position = vertices[index1].Position;
			tempVertices[index].Uv = vertices[index1].Uv;
			indices[index] = index;
			index++;

			// Triangle 1 - Upper Right
			tempVertices[index].Position = vertices[index2].Position;
			tempVertices[index].Uv = vertices[index2].Uv;
			indices[index] = index;
			index++;

			// Triangle 1 - Bottom Left
			tempVertices[index].Position = vertices[index3].Position;
			tempVertices[index].Uv = vertices[index3].Uv;
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom Left
			tempVertices[index].Position = vertices[index3].Position;
			tempVertices[index].Uv = vertices[index3].Uv;
			indices[index] = index;
			index++;

			// Triangle 2 - Upper Right
			tempVertices[index].Position = vertices[index2].Position;
			tempVertices[index].Uv = vertices[index2].Uv;
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom Right
			tempVertices[index].Position = vertices[index4].Position;
			tempVertices[index].Uv = vertices[index4].Uv;
			indices[index] = index;
			index++;
		}
	}

	

	vertexBuffer = Debug_new VertexBuffer(tempVertices, vertexCount, sizeof(VertexTexture));
	//vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);


	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(tempVertices);
	SAFE_DELETE_ARRAY(indices);
	CreateTexture();

	Vector3 position(0, 0, 0);
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->GetPosition(&position);

	static Vector3 Scale(100, 100, 100);
	static Vector3 Rot(0, 0, 0);
	static Vector3 Pos(position.x + 95, position.y + 100.0f, position.z + 300.0f);

	transform->SetScale(Scale);
	transform->SetRotation(Rot);
	transform->SetPosition(Pos);
}

Cloud::~Cloud()
{
	//SAFE_RELEASE(texture);
	//SAFE_RELEASE(srv);
	SAFE_DELETE(render2D);
}

void Cloud::Update()
{
	perFrame->Update();
	transform->Update();

	Vector3 position(0, 0, 0);
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->GetPosition(&position);

	//static Vector3 Scale(100, 100, 100);
	//static Vector3 Rot(0, 0, 0);
	//static Vector3 Pos(position.x + 95, position.y + 100.0f, position.z + 300.0f);
	//
	////static Vector3 Scale(1000, 500, 1);
	////static Vector3 Rot(-Math::PI * 0.5f, 0, 0);
	////static Vector3 Pos(position.x + 95, position.y + 100.0f, position.z + 300.0f);

	////ImGui::DragFloat3("Scale", Scale, 1.0f, 0.0f, 1000.0f);
	////ImGui::DragFloat3("Rot", Rot, 0.1f, 0.0f, Math::PI);
	////ImGui::DragFloat3("Pos", Pos, 10.0f, 0.0f, 1000.0f);

	//transform->SetScale(Scale);
	//transform->SetRotation(Rot);
	//transform->SetPosition(Pos);


	render2D->Update();
}

void Cloud::Render()
{
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Render();

		if (indexBuffer != NULL)
			indexBuffer->Render();
	}

	D3D::GetDC()->IASetPrimitiveTopology(topology);
	perFrame->Render();
	transform->Render();




	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sSRV->SetResource(srv);
	shader->DrawIndexed(0, GetPass(), indexCount);
}

void Cloud::PostRender()
{
	render2D->SRV(srv);
	//render2D->SRV(cloudMap->SRV());
	render2D->Render();
}

void Cloud::CreateTexture()
{

	int perm[] =
	{
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};//펄린노이즈 상수

	int gradValues[] =
	{
		+1, +1, +0, -1, +1, +0, +1, -1, +0, -1, -1, +0,
		+1, +0, +1, -1, +0, +1, +1, +0, -1, -1, +0, -1,
		+0, +1, +1, +0, -1, +1, +0, +1, -1, +0, -1, -1,
		+1, +1, +0, +0, -1, +1, -1, +1, +0, +0, -1, -1
	};


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = 256;
	desc.Height = 256;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	Color* pixels = Debug_new Color[256 * 256];
	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			int value = perm[(x + perm[y]) & 0xFF];

			Color color;
			color.r = (float)(gradValues[value & 0x0F] * 64 + 64);
			color.g = (float)(gradValues[value & 0x0F + 1] * 64 + 64);
			color.b = (float)(gradValues[value & 0x0F + 2] * 64 + 64);
			color.a = (float)value;


			UINT index = desc.Width * y + x;
			pixels[index] = color;
		}
	}

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = pixels;
	subResource.SysMemPitch = 256 * 4;

	Check(D3D::GetDevice()->CreateTexture2D(&desc, &subResource, &texture));

	//Create SRV
	{
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = desc.Format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv));
	}

	SAFE_DELETE_ARRAY(pixels);
}
