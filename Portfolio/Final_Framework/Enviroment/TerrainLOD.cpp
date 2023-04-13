#include "Framework.h"
#include "TerrainLOD.h"

TerrainLOD::TerrainLOD()
	:Renderer(L"57_TerrainLOD.fxo"), imageFile(nullptr)
{
	
}

TerrainLOD::TerrainLOD(std::wstring imageFile)
	:Renderer(L"57_TerrainLOD.fxo"), imageFile(imageFile)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	ReadHeightData();

	patchWidth = ((width - 1) / cellsPerPatch) + 1;
	patchHeight = ((height - 1) / cellsPerPatch) + 1;

	vertexCount = patchWidth * patchHeight;
	indexCount = (patchWidth - 1) * (patchHeight - 1);

	CreatePatchBound();
	CreatePatchVertex();
	CreatePatchIndex();

	sHeightMapSRV = shader->AsSRV("HeightMap");
	sBaseMap = shader->AsSRV("BaseMap");

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLOD");

	desc.CellspacingU = 1.0f / width;
	desc.CellspacingV = 1.0f / height;

	//camera = Debug_new Fixity();
	//camera = Debug_new FreeCamera();
	perspective = Debug_new Perspective(D3D::Width(), D3D::Height(), 0.1f, 1000.0f, Math::PI * 0.25);
	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, 5000.0f, Math::PI * 0.6f);
	//frustum = Debug_new Frustum(camera, perspective);
	frustum = Debug_new Frustum(Context::Get()->GetCamera(0), perspective);
	
	BaseMap(L"Terrain/Dirt3.png");
}

TerrainLOD::TerrainLOD(Actor * _actor, Transform * _transform, Shader * _shader)
	:Renderer(_actor, _transform, _shader)
{
	heights = nullptr;

	heightMap = nullptr;
	heightMapSRV = nullptr;

	buffer = nullptr;

	baseMap = nullptr;
	sBaseMap = nullptr;

	perspective = nullptr;
	frustum = nullptr;
	bounds = nullptr;
}


TerrainLOD::~TerrainLOD()
{
	SAFE_DELETE_ARRAY(heights);

	SAFE_RELEASE(heightMap);
	SAFE_RELEASE(heightMapSRV);

	SAFE_DELETE(buffer);

	SAFE_DELETE(baseMap);

	//SAFE_DELETE(camera);
	SAFE_DELETE(perspective);
	SAFE_DELETE(frustum);
	SAFE_DELETE(bounds);
}

void TerrainLOD::SetTerrainMap(std::wstring _imageFile)
{
	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
	SAFE_DELETE(buffer);
	SAFE_DELETE(perspective);
	SAFE_DELETE(frustum);
	SAFE_DELETE(bounds);

	this->imageFile = _imageFile;
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	ReadHeightData();

	patchWidth = ((width - 1) / cellsPerPatch) + 1;
	patchHeight = ((height - 1) / cellsPerPatch) + 1;

	vertexCount = patchWidth * patchHeight;
	indexCount = (patchWidth - 1) * (patchHeight - 1);

	CreatePatchBound();
	CreatePatchVertex();
	CreatePatchIndex();

	sHeightMapSRV = shader->AsSRV("HeightMap");
	sBaseMap = shader->AsSRV("BaseMap");

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLOD");

	desc.CellspacingU = 1.0f / width;
	desc.CellspacingV = 1.0f / height;

	//camera = Debug_new Fixity();
	//camera = Debug_new FreeCamera();
	perspective = Debug_new Perspective(D3D::Width(), D3D::Height(), 0.1f, 1000.0f, Math::PI * 0.25);
	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, 5000.0f, Math::PI * 0.6f);
	//frustum = Debug_new Frustum(camera, perspective);
	frustum = Debug_new Frustum(Context::Get()->GetCamera(0), perspective);

	BaseMap(L"Terrain/Dirt3.png");
}

void TerrainLOD::Update()
{
	perFrame->Update();
	transform->Update();


	static float distance = 5000.0f;
	static float angle = 0.5f;
	ImGui::SliderFloat("Distance", &distance, 1000.0f , 10000.0f);
	ImGui::SliderFloat("Angle", &angle, 0.1f, 1.0f);// , "%.3f", 0.01f);
	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, distance, Math::PI * angle);

	ImGui::Separator();
	ImGui::SliderFloat("Height Scale", &desc.HeightScale, 0.1f, 1.0f);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass = Math::Clamp<UINT>(pass, 0, 1);
	
	Pass(pass);

	frustum->Update();
	frustum->GetPlanes(desc.Culling, 6);
	//
	//static Vector3 test = Vector3(0.1f, 2000.0f, 0.6f);
	//ImGui::SliderFloat("zn", &test.x, 0.1f, 10.0f);
	//ImGui::SliderFloat("zf", &test.y, 1000.0f, 10000.0f);
	//ImGui::SliderFloat("fov", &test.z, 0.1f, 1.0f);
	//perspective->Set(D3D::Width(), D3D::Height(), test.x, test.y, Math::PI * test.z);
	//frustum->SetPerspective(perspective);

}

void TerrainLOD::Render()
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

	sHeightMapSRV->SetResource(heightMapSRV);
	sBaseMap->SetResource(baseMap->SRV());

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	shader->DrawIndexed(0, GetPass(), indexCount * 4);
}

void TerrainLOD::BaseMap(std::wstring file)
{
	SAFE_DELETE(baseMap);

	baseMap = Debug_new Texture(file);
}

float TerrainLOD::GetWidth()
{
	return width * desc.Cellspacing;
}

float TerrainLOD::GetHeight()
{
	return height * desc.Cellspacing;
}

void TerrainLOD::ReadHeightData()
{

	std::wstring ext = Path::GetExtension(imageFile);
	std::transform(ext.begin(), ext.end(), ext.begin(), toupper);

	assert(ext == L"DDS");


	//Texture* texture = Debug_new Texture(imageFile);
	Texture* texture = Debug_new Texture(imageFile, false);
	ID3D11Texture2D* srcTexture = texture->GetTexture();

	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);


	ID3D11Texture2D* readTexture;
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
	readDesc.Width = srcDesc.Width;
	readDesc.Height = srcDesc.Height;
	readDesc.ArraySize = 1;
	readDesc.Format = srcDesc.Format;
	readDesc.MipLevels = 1;
	readDesc.SampleDesc = srcDesc.SampleDesc;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, NULL, &readTexture));
	D3D::GetDC()->CopyResource(readTexture, srcTexture);


	UINT* pixels = Debug_new UINT[readDesc.Width * readDesc.Height];

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(pixels, subResource.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
	}
	D3D::GetDC()->Unmap(readTexture, 0);


	width = texture->GetWidth();
	height = texture->GetHeight();

	heights = Debug_new float[width * height];
	for (UINT i = 0; i < width * height; i++)
	{
		UINT temp = (pixels[i] & 0xFF000000) >> 24;
		heights[i] = (float)temp / 255.0f;
	}

	SAFE_DELETE(texture);
	SAFE_RELEASE(readTexture);
	SAFE_DELETE(pixels);
	
	D3D11_TEXTURE2D_DESC heightDesc;
	ZeroMemory(&heightDesc, sizeof(D3D11_TEXTURE2D_DESC));
	heightDesc.Width = srcDesc.Width;
	heightDesc.Height = srcDesc.Height;
	heightDesc.ArraySize = 1;
	heightDesc.Format = DXGI_FORMAT_R32_FLOAT;
	heightDesc.MipLevels = 1;
	heightDesc.SampleDesc = srcDesc.SampleDesc;
	heightDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = heights;
	data.SysMemPitch = width * 4;
	data.SysMemSlicePitch = width * height * 4;

	Check(D3D::GetDevice()->CreateTexture2D(&heightDesc, &data, &heightMap));


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; //읽어들인 데이터를 shader로 넘겨주기위함
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	Check(D3D::GetDevice()->CreateShaderResourceView(heightMap, &srvDesc, &heightMapSRV));
}

void TerrainLOD::CreatePatchVertex()
{
	std::vector<TerrainVertex> vertices(vertexCount);

	float halfWidth = GetWidth() * 0.5f;
	float halfHeight = GetHeight() * 0.5f;

	float tempWidth = GetWidth() / patchWidth;
	float tempHeight = GetWidth() / patchHeight;

	float du = 1.0f / patchWidth;
	float dv = 1.0f / patchHeight;

	for (UINT z = 0; z < patchHeight; z++)
	{
		float tempZ = halfHeight - z * tempHeight; 

		for (UINT x = 0; x < patchWidth; x++)
		{
			float tempX = -halfWidth + x * tempWidth;
			
			vertices[patchWidth * z + x].Position = Vector3(tempX, 0.0f, tempZ);
			vertices[patchWidth * z + x].Uv.y = z * dv;
			vertices[patchWidth * z + x].Uv.x = x * du;

			UINT patchID = z * patchWidth + x;
			vertices[z * patchWidth + x].Bound = bounds[patchID];
		}
	}

	vertexBuffer = Debug_new VertexBuffer(&vertices[0], vertices.size(), sizeof(TerrainVertex));

}

void TerrainLOD::CreatePatchBound()
{
	bounds = Debug_new Vector2[vertexCount];

	for (UINT z = 0; z < patchHeight; z++)
	{
		for (UINT x = 0; x < patchWidth; x++)
		{
			UINT x0 = x * cellsPerPatch;
			UINT x1 = (x + 1) * cellsPerPatch;

			UINT z0 = z * cellsPerPatch;
			UINT z1 = (z + 1) * cellsPerPatch;


			float minY = +FLT_MAX;
			float maxY = -FLT_MAX;

			for (UINT tempZ = z0; tempZ <= z1; tempZ++)
			{
				for (UINT tempX = x0; tempX <= x1; tempX++)
				{
					UINT k = z * width + x;

					minY = std::min(minY, heights[k]);
					maxY = std::max(maxY, heights[k]);
				}
			}

			UINT patchID = z * patchWidth + x;
			bounds[patchID] = Vector2(minY, maxY);
		}//for(x)
	}//for(z)
}

void TerrainLOD::CreatePatchIndex()
{
	std::vector<UINT> indices(indexCount * 4);

	UINT index = 0;
	for (UINT z = 0; z < patchHeight - 1; z++)
	{
		for (UINT x = 0; x < patchWidth - 1; x++)
		{
			indices[index + 0] = patchWidth * z + x;
			indices[index + 1] = patchWidth * z + x + 1;
			indices[index + 2] = patchWidth * (z + 1) + x;
			indices[index + 3] = patchWidth * (z + 1) + x + 1;

			index += 4;
		}
	}

	indexBuffer = Debug_new IndexBuffer(&indices[0], indices.size());
}
