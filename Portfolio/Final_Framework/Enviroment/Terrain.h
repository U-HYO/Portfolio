#pragma once
#include "Brush.h"
class Terrain : public Renderer
{
private:
	struct VertexTerrain;
public:
	Terrain(Shader* shader, std::wstring imageFile);
	Terrain(Shader* shader, std::wstring imageFile, bool isInTerrain);
	Terrain(class Actor* _actor, class Transform* _transform, Shader* _shader); // 데이터파일
	~Terrain();

	void Update();
	void Render();

	void BaseMap(std::wstring file);
	void Layer1(std::wstring file);

	void UpdateVertexData();

public:
	UINT GetWidth() { return width; }
	UINT GetHeight() { return height; }
	VertexTerrain* GetVertices() { return vertices; }

	float GetHeight(Vector3& position);
	float GetVerticalRaycast(Vector3& position);
	Vector3 GetRaycastPosition();
	Vector3 GetRaycastPosition2();

	Texture* GetBaseMap() { return baseMap; }

	Texture* GetHeightMap() { return heightMap; }
	auto GetImageFile() { return imageFile; }

private:
	void ReadHeightData();
	void CreateVertexData();
	void CreateIndexData();

public:
	void CreateNormalData();
	void SaveHeightMap();

	void SetTerrainMap(std::wstring imageFile);
private:
	struct VertexTerrain
	{
		Vector3 Position = Vector3(0, 0, 0);
		Vector2 Uv = Vector2(0, 0);
		Vector3 Normal = Vector3(0, 0, 0);
		Color color = Color(0, 0, 0, 0);
	};

	struct Layer
	{
		float* Data = NULL;
		ID3D11Texture2D* Texture2D = NULL;
		ID3D11ShaderResourceView* SRV = NULL;
		ID3DX11EffectShaderResourceVariable* sSRV;

		Texture* Map = NULL;
		ID3DX11EffectShaderResourceVariable* sMap;

		~Layer()
		{
			SAFE_DELETE_ARRAY(Data);
			SAFE_RELEASE(Texture2D);
			SAFE_RELEASE(SRV);
			SAFE_DELETE(Map);
		}
	};
	Layer layer1;

private:
	Material* material;

	std::wstring imageFile;
	UINT width, height;

	VertexTerrain* vertices;
	UINT* indices;

	float* heights;

	Texture* heightMap;
	
	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	ID3D11Texture2D* currentMapTexture;
	std::vector<Color> pixels;

	//TerrainEditor
public:
	Billboard* billboard;

private:
	enum class Channel
	{
		Red = 0, Green, Blue, Alpha, Max,
	};
	int selectedChannel = 0;

	Shader* brushShader;
public:
	class Brush* brush;


///////////////////Picking//////////////////////////////////
public:
	//Vector3 GetIntersectPos() { return result[0]; }

private:

	void PickingMap();

	ID3D11Texture2D* worldPosTex;
	ID3D11ShaderResourceView* worldPosTexSRV;

	ID3DX11EffectShaderResourceVariable* sPickingMap;

	void CreatePicking();
	void UpdatePicking();

private:
	//struct PickingDesc
	//{
	//	Matrix world;
	//	Matrix view;
	//	Matrix projection;
	//	Vector2 screenPos;
	//	float Padding[2];
	//}pickingDesc;

	//Shader* pickingShader;

	//ConstantBuffer* pickingBuffer;
	//ID3DX11EffectConstantBuffer* sPickingBuffer;

	//PerFrame* pickingPerFrame;
	//


	Vector3 pickingPoint;


};