#include "Framework.h"
#include "Terrain.h"

Terrain::Terrain(Shader * shader, std::wstring imageFile)
	: Renderer(shader)
{
	brush = nullptr;
	vertices = nullptr;
	indices = nullptr;
	heights = nullptr;

	material = nullptr;
	baseMap = nullptr;
	layer1.Map = nullptr;
	layer1.Data = nullptr;
	heightMap = nullptr;

	sBaseMap = nullptr;
	currentMapTexture = nullptr;

	pixels.clear();


	//this->imageFile = L"Terrain/";
	this->imageFile = imageFile;
	ReadHeightData();
	CreateVertexData();
	CreateIndexData();
	CreateNormalData();


	//CreatePicking();



	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain), 0, true);
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);

	material = Debug_new Material(shader);
	material->Diffuse(1, 1, 1, 1);
	material->Specular(1, 1, 1, 200);

	sBaseMap = shader->AsSRV("BaseMap");

	layer1.sSRV = shader->AsSRV("Layer1AlphaMap");
	layer1.sMap = shader->AsSRV("Layer1ColorMap");

	currentMapTexture = NULL;

	//brushShader = Debug_new Shader(L"55_Brush.fxo");
	//brush = Debug_new Brush(brushShader, this);
}

Terrain::Terrain(Shader * shader, std::wstring imageFile, bool isInTerrain)
	: Renderer(shader)
{
	brush = nullptr;
	vertices = nullptr;
	indices = nullptr;
	heights = nullptr;

	material = nullptr;
	baseMap = nullptr;
	layer1.Map = nullptr;
	layer1.Data = nullptr;
	heightMap = nullptr;

	sBaseMap = nullptr;
	currentMapTexture = nullptr;

	pixels.clear();

	this->imageFile = imageFile;

	ReadHeightData();
	CreateVertexData();
	CreateIndexData();
	CreateNormalData();
	
	
	//CreatePicking();



	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain), 0, true);
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);

	material = Debug_new Material(shader);
	material->Diffuse(1, 1, 1, 1);
	material->Specular(1, 1, 1, 200);

	sBaseMap = shader->AsSRV("BaseMap");

	layer1.sSRV = shader->AsSRV("Layer1AlphaMap");
	layer1.sMap = shader->AsSRV("Layer1ColorMap");

	//brushShader = Debug_new Shader(L"55_Brush.fxo");
	//brush = Debug_new Brush(brushShader, this);
}


Terrain::Terrain(Actor * _actor, Transform * _transform, Shader * _shader)
	:Renderer(_actor, _transform, _shader)
{
	vertices = nullptr;
	indices = nullptr;
	heights = nullptr;

	material = nullptr;
	baseMap = nullptr;
	layer1.Map = nullptr;
	layer1.Data = nullptr;
	heightMap = nullptr;

	sBaseMap = nullptr;
	currentMapTexture = nullptr;

	pixels.clear();

	//brushShader = Debug_new Shader(L"55_Brush.fxo");
	//brush = Debug_new Brush(brushShader, this);
	brush = nullptr;
}

Terrain::~Terrain()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(heights);

	SAFE_DELETE(material);
	SAFE_DELETE(baseMap);
	SAFE_DELETE(layer1.Map);
	SAFE_DELETE(heightMap);

	//SAFE_DELETE(brushShader);
	SAFE_DELETE(brush);
}

void Terrain::Update()
{
	//UpdatePicking();
	
	perFrame->Update();
	transform->Update();

	if (brush != nullptr)
		brush->Update();
}

void Terrain::Render()
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


	if (baseMap != NULL)
		shader->AsSRV("BaseMap")->SetResource(baseMap->SRV());

	if (layer1.Data != NULL)
	{
		layer1.sSRV->SetResource(layer1.SRV);
		layer1.sMap->SetResource(layer1.Map->SRV());
	}

	material->Render();
	shader->DrawIndexed(0, GetPass(), indexCount);

	if (brush)
		brush->Render();
}

void Terrain::BaseMap(std::wstring file)
{
	SAFE_DELETE(baseMap);

	baseMap = Debug_new Texture(file);
}

void Terrain::Layer1(std::wstring file)
{
	SAFE_DELETE(layer1.Map);

	layer1.Map = Debug_new Texture(file);
}
void Terrain::UpdateVertexData()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(Terrain::VertexTerrain) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 v[4];
	for (UINT i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;

	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;


	Vector3 result;

	if (ddx + ddz <= 1.0f)
	{
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1.0f - ddx;
		ddz = 1.0f - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}


	return result.y;
}

float Terrain::GetVerticalRaycast(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 p[4];
	for (UINT i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;


	Vector3 start(position.x, 100.0f, position.z);
	Vector3 direction(0, -1, 0);

	float u, v, distance;
	Vector3 result(-1, FLT_MIN, -1);

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;
}

Vector3 Terrain::GetRaycastPosition()
{
	Matrix W = GetTransform()->GetWorld();
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();


	mouse.z = 0.0f;
	Vector3 n = Vp->UnProject(mouse, W, V, P);

	mouse.z = 0.999999f;
	Vector3 f = Vp->UnProject(mouse, W, V, P);

	
	Vector3 start = n;
	Vector3 direction = f - n;

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + x + 1;
			index[3] = width * (z + 1) + x + 1;

			Vector3 p[4];
			for (UINT i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;


			float u, v, distance;

			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		}
	}

	return Vector3(-1, -1, -1);
}

Vector3 Terrain::GetRaycastPosition2()
{
	Matrix W = GetTransform()->GetWorld();
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();


	mouse.z = 0.0f;
	Vector3 n = Vp->UnProject(mouse, W, V, P);

	mouse.z = 0.999999f;
	Vector3 f = Vp->UnProject(mouse, W, V, P);

	Vector3 start = n;
	Vector3 direction = f - n;

	float u, v, distance;

	for (int i = 0; i < indexCount; i += 3)
	{
		Vector3 p[3];
		p[0] = vertices[indices[i]].Position;
		p[1] = vertices[indices[i+1]].Position;
		p[2] = vertices[indices[i+2]].Position;

		if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		{
			auto df = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
			ImGui::Text("World MousePos D= (%.1f, %.1f, %.1f)", u, v, distance);
			return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
		}

		//if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		//	return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
	}


	return Vector3(-1, -1, -1);
}

//BOOL IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
//{
//	// Find vectors for two edges sharing vert0
//	D3DXVECTOR3 edge1 = v1 - v0;
//	D3DXVECTOR3 edge2 = v2 - v0;
//	// Begin calculating determinant - also used to calculate U parameter
//	D3DXVECTOR3 pvec;
//	D3DXVec3Cross(&pvec, &dir, &edge2);
//	// If determinant is near zero, ray lies in plane of triangle   
//	FLOAT det = D3DXVec3Dot(&edge1, &pvec);
//	D3DXVECTOR3 tvec;
//	if (det > 0)
//	{
//		tvec = orig - v0;
//	}
//	else
//	{
//		tvec = v0 - orig;       
//		det = -det;
//	}
//	if (det < 0.0001f)
//		return FALSE;    // Calculate U parameter and test bounds 
//	*u = D3DXVec3Dot(&tvec, &pvec);
//	if (*u < 0.0f || *u > det)
//		return FALSE;    // Prepare to test V parameter    
//	D3DXVECTOR3 qvec;
//	D3DXVec3Cross(&qvec, &tvec, &edge1);
//	// Calculate V parameter and test bounds  
//	*v = D3DXVec3Dot(&dir, &qvec);
//	if (*v < 0.0f || *u + *v > det)
//		return FALSE;
//	// Calculate t, scale parameters, ray intersects triangle 
//	*t = D3DXVec3Dot(&edge2, &qvec);
//	FLOAT fInvDet = 1.0f / det;
//	*t *= fInvDet;
//	*u *= fInvDet;
//	*v *= fInvDet;
//	return TRUE;
//}

void Terrain::ReadHeightData()
{
	std::wstring ext = Path::GetExtension(imageFile);
	std::transform(ext.begin(), ext.end(), ext.begin(), toupper);

	SAFE_DELETE(heightMap);
	SAFE_DELETE_ARRAY(heights);
	pixels.clear();

	if (ext == L"DDS")
	{
		heightMap = Debug_new Texture(imageFile, false);
		ID3D11Texture2D* srcTexture = heightMap->GetTexture();

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


		width = heightMap->GetWidth();
		height = heightMap->GetHeight();

		heights = Debug_new float[width * height];
		for (UINT i = 0; i < width * height; i++)
		{
			UINT temp = (pixels[i] & 0xFF000000) >> 24;
			heights[i] = (float)temp / 255.0f;

			//temp = (pixels[i] & 0x00FF0000) >> 16;
		}
		

		layer1.Data = Debug_new float[width * height];
		for (UINT i = 0; i < width * height; i++)
		{
			UINT temp = (pixels[i] & 0x000000FF);
			layer1.Data[i] = (float)temp / 255.0f;
		}

		SAFE_RELEASE(readTexture);
		SAFE_DELETE_ARRAY(pixels);

		D3D11_TEXTURE2D_DESC layerDesc;
		ZeroMemory(&layerDesc, sizeof(D3D11_TEXTURE2D_DESC));
		layerDesc.Width = srcDesc.Width;
		layerDesc.Height = srcDesc.Height;
		layerDesc.ArraySize = 1;
		layerDesc.Format = DXGI_FORMAT_R32_FLOAT;
		layerDesc.MipLevels = 1;
		layerDesc.SampleDesc = srcDesc.SampleDesc;
		layerDesc.Usage = D3D11_USAGE_DYNAMIC;
		layerDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		layerDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;


		D3D11_SUBRESOURCE_DATA layerSubResource = { 0 };
		layerSubResource.pSysMem = layer1.Data;
		layerSubResource.SysMemPitch = width * 4;
		layerSubResource.SysMemSlicePitch = width * 4 * height;

		Check(D3D::GetDevice()->CreateTexture2D(&layerDesc, &layerSubResource, &layer1.Texture2D));
		//D3DX11SaveTextureToFile(D3D::GetDC(), layer1, D3DX11_IFF_DDS, L"../Red.dds");


		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(layer1.Texture2D, &srvDesc, &layer1.SRV));



		return;
	}
	else
	{
		//heightMap = Debug_new Texture(imageFile);
		heightMap = Debug_new Texture(imageFile, false);

		heightMap->ReadPixel(&pixels);

		width = heightMap->GetWidth();
		height = heightMap->GetHeight();

		heights = Debug_new float[width * height];
		for (UINT i = 0; i < pixels.size(); i++)
			heights[i] = pixels[i].r;
	}
}

void Terrain::CreateVertexData()
{
	SAFE_DELETE_ARRAY(vertices);

	vertexCount = width * height;
	vertices = Debug_new VertexTerrain[vertexCount];

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * y + x;
			UINT pixel = width * (height - y - 1) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = heights[pixel] * 255.0f / 10.0f;
			vertices[index].Position.z = (float)y;

			vertices[index].Uv.x = x / (float)(width - 1);
			vertices[index].Uv.y = 1.0f - (y / (float)(height - 1));
		}//for(x)
	}//for(y)
}

void Terrain::CreateIndexData()
{
	SAFE_DELETE_ARRAY(indices);

	indexCount = (width - 1) * (height - 1) * 6;
	indices = Debug_new UINT[indexCount];

	UINT index = 0;
	for (UINT y = 0; y < height - 1; y++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * y + x;
			indices[index + 1] = width * (y + 1) + x;
			indices[index + 2] = width * y + x + 1;
			indices[index + 3] = width * y + x + 1;
			indices[index + 4] = width * (y + 1) + x;
			indices[index + 5] = width * (y + 1) + x + 1;

			index += 6;
		}//for(x)
	}//for(y)
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexTerrain v0 = vertices[index0];
		VertexTerrain v1 = vertices[index1];
		VertexTerrain v2 = vertices[index2];


		Vector3 e1 = v1.Position - v0.Position;
		Vector3 e2 = v2.Position - v0.Position;

		Vector3 normal;
		D3DXVec3Cross(&normal, &e1, &e2);


		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}

void Terrain::SaveHeightMap()
{
	UpdateVertexData();
	ID3D11Texture2D* srcTexture = heightMap->GetTexture();
	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	ID3D11Texture2D* dstTexture = nullptr;
	D3D11_TEXTURE2D_DESC dstDesc;
	ZeroMemory(&dstDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dstDesc.Width = width;
	dstDesc.Height = height;
	dstDesc.MipLevels = 1;
	dstDesc.ArraySize = 1;
	dstDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dstDesc.SampleDesc = srcDesc.SampleDesc;
	dstDesc.Usage = D3D11_USAGE_STAGING;
	dstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	Check(D3D::GetDevice()->CreateTexture2D(&dstDesc, NULL, &dstTexture));


	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(dstTexture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		UINT* pixels = static_cast<UINT*>(subResource.pData);

		for (UINT y = 0; y < height; y++)
		{
			UINT yStart = y * subResource.RowPitch;

			for (UINT x = 0; x < width; x++)
			{
				UINT xStart = x * 4;

				UINT index = width * y + x;
				UINT pixel = width * (height - y - 1) + x;

				pixels[pixel] = (UINT)((vertices[index].Position.y) * 10) << 24;
				

			}//for(x)
		}//for(y)
	}
	D3D::GetDC()->Unmap(dstTexture, 0);

	//auto hr = D3DX11SaveTextureToFile(
	//	D3D::GetDC(),
	//	dstTexture,
	//	D3DX11_IFF_DDS,
	//	L"../../_Textures/Terrain/aad.dds"
	//);
	//assert(SUCCEEDED(hr));
	FileSystem::SaveFileDialog([=](std::string path)
	{
		std::wstring out;
		out += FileSystem::ToWString(path).c_str();
		out += L".dds";
		auto hr = D3DX11SaveTextureToFile(
			D3D::GetDC(),
			dstTexture,
			D3DX11_IFF_DDS,
			out.c_str()
		);
		assert(SUCCEEDED(hr));
	});

	SAFE_RELEASE(srcTexture);
	SAFE_RELEASE(dstTexture);
}

void Terrain::SetTerrainMap(std::wstring imageFile)
{
	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
	SAFE_DELETE(material);
	//SAFE_DELETE(brush);

	this->imageFile = imageFile;
	ReadHeightData();
	CreateVertexData();
	CreateIndexData();
	CreateNormalData();


	CreatePicking();



	vertexBuffer = Debug_new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain), 0, true);
	indexBuffer = Debug_new IndexBuffer(indices, indexCount);

	material = Debug_new Material(shader);
	material->Diffuse(1, 1, 1, 1);
	material->Specular(1, 1, 1, 200);

	sBaseMap = shader->AsSRV("BaseMap");

	layer1.sSRV = shader->AsSRV("Layer1AlphaMap");
	layer1.sMap = shader->AsSRV("Layer1ColorMap");

	currentMapTexture = NULL;

	BaseMap(L"Terrain/Dirt3.png");
	Layer1(L"Terrain/Dirt.png");

	//brush = Debug_new Brush(brushShader, this);
}

void Terrain::PickingMap()
{
	D3D11_TEXTURE2D_DESC worldPosTexDesc;
	int textureWidth = 1024;
	int textureHeight = 1024;
	worldPosTexDesc.Width = textureWidth;
	worldPosTexDesc.Height = textureHeight;
	worldPosTexDesc.MipLevels = 1;
	worldPosTexDesc.ArraySize = 1;
	worldPosTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	worldPosTexDesc.SampleDesc.Count = 1;
	worldPosTexDesc.Usage = D3D11_USAGE_DEFAULT;
	worldPosTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	worldPosTexDesc.CPUAccessFlags = 0;
	worldPosTexDesc.MiscFlags = 0;
	D3D::GetDevice()->CreateTexture2D(&worldPosTexDesc, NULL, &worldPosTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = worldPosTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	D3D::GetDevice()->CreateShaderResourceView(worldPosTex, &srvDesc, &worldPosTexSRV);
	
	sPickingMap = shader->AsSRV("worldPosTex");

}

void Terrain::CreatePicking()
{
	PickingMap();
	//Matrix W = GetTransform()->GetWorld();
	//Matrix V = Context::Get()->View();
	//Matrix P = Context::Get()->Projection();
	//Viewport* Vp = Context::Get()->GetViewport();
	//
	//
	//pickingShader = Debug_new Shader(L"00_TerrainPicking3.fxo");
	//pickingPerFrame = Debug_new PerFrame(pickingShader);
	//
	//pickingDesc.projection = P;
	//pickingDesc.screenPos = Vector2(Mouse::Get()->GetPosition().x, Mouse::Get()->GetPosition().y);
	//pickingDesc.view = V;
	//pickingDesc.world = W;
	//
	//pickingBuffer = Debug_new ConstantBuffer(&pickingDesc, sizeof(PickingDesc));
	//sPickingBuffer = pickingShader->AsConstantBuffer("Constants");


}

void Terrain::UpdatePicking()
{
	//pickingPerFrame->Update();
	//
	//pickingBuffer->Render();
	//sPickingBuffer->SetConstantBuffer(pickingBuffer->GetBuffer());
	//
	//
	//pickingPerFrame->Render();

	shader->AsSRV("worldPosTex")->SetResource(worldPosTexSRV);

	//pickingShader->AsScalar("screenPosX")->SetFloat(Mouse::Get()->GetPosition().x);
	//pickingShader->AsScalar("screenPosY")->SetFloat(Mouse::Get()->GetPosition().y);
	shader->AsScalar("screenPosX")->SetFloat(Mouse::Get()->GetPosition().x);
	shader->AsScalar("screenPosY")->SetFloat(Mouse::Get()->GetPosition().y);

	float testa;
	float testb;
	float testc;
	float testd;
	float teste;
	//float testf;
	//pickingShader->AsScalar("pickingPointx")->GetFloat(&testa);
	//pickingShader->AsScalar("pickingPointx")->GetFloat(&testb);
	//pickingShader->AsScalar("pickingPointx")->GetFloat(&testc);
	shader->AsScalar("pickingPointx")->GetFloat(&testa);
	shader->AsScalar("pickingPointy")->GetFloat(&testb);
	shader->AsScalar("pickingPointz")->GetFloat(&testc);
	
	
	//shader->AsScalar("wawt")->GetFloat(&testf);


	shader->AsScalar("screenPosX")->GetFloat(&testd);
	shader->AsScalar("screenPosY")->GetFloat(&teste);

	pickingPoint = Vector3(testa, testb, testc);

	ImGui::Text("screenPos %.2f %.2f", testd, teste);
	ImGui::Text("pickingPoint %.2f %.2f %.2f", pickingPoint.x, pickingPoint.y, pickingPoint.z);
	//ImGui::Text("awaef %.2f", testf);

}