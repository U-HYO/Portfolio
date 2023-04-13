#pragma once

class TerrainLOD : public Renderer
{
public:
	TerrainLOD();
	TerrainLOD(std::wstring imageFile); // 데이터파일
	TerrainLOD(class Actor* _actor, class Transform* _transform, Shader* _shader); // 데이터파일
	~TerrainLOD();

	virtual void Update();
	virtual void Render();

	void BaseMap(std::wstring file);

public:
	float GetWidth();
	float GetHeight();

	void SetTerrainMap(std::wstring _imageFile);
	auto GetImageFile() { return imageFile; }
	ID3D11ShaderResourceView* GetHeightMap() { return heightMapSRV; }

private:
	void ReadHeightData();
	void CreatePatchVertex();
	void CreatePatchBound(); // 해당 구역을 돌며 최대, 최소높이를 구함
	void CreatePatchIndex();

private:
	struct TerrainVertex
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Bound; //x가 최소 y가 최대
	};

private:
	struct Desc
	{
		Vector2 Distance = Vector2(1, 1000);	// 테셀레이션 할 최소거리~최대거리
		Vector2 TessFactor = Vector2(1, 64);	// 테셀레이션 최소갯수 ~ 최대갯수

		float Cellspacing = 2.0f;
		float CellspacingU;			// 한 픽셀당 크기
		float CellspacingV;			// 한 픽셀당 크기
		float HeightScale = 0.5f;	// heightMap scale

		Plane Culling[6];
	} desc;

private:
	UINT cellsPerPatch = 32;	//한 칸을 나눌 갯수

	std::wstring imageFile;

	UINT width, height;
	UINT patchWidth, patchHeight;

	float* heights;	//높이맵 배열정보
	Vector2* bounds;

	ID3D11Texture2D* heightMap = NULL;
	ID3D11ShaderResourceView* heightMapSRV = NULL;
	ID3DX11EffectShaderResourceVariable* sHeightMapSRV;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Camera* camera;
	Perspective* perspective;
	Frustum* frustum;

};

