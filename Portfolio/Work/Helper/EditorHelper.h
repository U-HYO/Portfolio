#pragma once

enum class PostEffectType : UINT
{
	Unknown = 0,
	Bloom,
	Blur,
	ColorTone,
};

class EditorHelper final
{
public:
	static EditorHelper* Get();
	
	static void Create();
	static void Update();
	static void Delete();
	static void PreRender();
	static void Render();
	static void PostRender();

	void Init();

	void LoadModel(const std::wstring& path);
	void LoadSkycube(const std::wstring& path);
	void LoadSky();
	void LoadMesh(Mesh* mesh);
	//void LoadLight()
	void LoadMeshModel(const std::wstring& path);
	void LoadMaterial(const std::wstring& path);
	void LoadTerrain(const std::wstring& path);
	void LoadTerrainLOD(const std::wstring& path);
	void LoadParticle(const std::wstring& path);
	void LoadRain(const std::wstring& path);
	void LoadSnow(const std::wstring& path);
	void LoadSpotLight();
	void LoadPointLight();
	void LoadWater(const std::wstring& path);

	void PostEffectInit();
	//std::map<std::string, class ModelAnimator*> GetModels() { return models; }
	std::vector<Actor*> GetActors() { return actors; }


	RenderTarget* GetRenderTarget() { return renderTarget; }
	RenderTarget** GetBloomRenderTargets() { return bloomRenderTarget; }
private:
	EditorHelper();
	~EditorHelper();
	static EditorHelper* instance;

private:
	Shader* modelShader;
	Shader* terrainLODShader;
	
	Shadow* shadow;


	void Pass(UINT mesh, UINT model, UINT anim);

	std::vector<Actor*> actors;
	
	
private:
	//PostEffect
	RenderTarget* renderTarget;
	RenderTarget* bloomRenderTarget[5];

	DepthStencil* depthStencil;
	Viewport* viewport;

	//PostEffect* bloomEffect;
	//PostEffect* blurEffect;
	//PostEffect* colorToneEffect;
	//PostEffect* MRTEffect;
	//PostEffect* RTVEffect;

	PostEffectType postEffectType;


private:
	void GetBlurParams(std::vector<float>& _weights, std::vector<Vector2>& _offsets, float _x, float _y, float _sum, UINT _blurCount);


public:
	float threshold = 0.05f;
	UINT blurCount = 15;

	std::vector<float> weightX;
	std::vector<float> weightY;

	std::vector<Vector2> offsetX;
	std::vector<Vector2> offsetY;
	float sum = 0.0f;

	float blurRatio = 0.5f;

	float grading = 0.5f;
	float correlation = 0.5f;
	float concentration = 2.0f;

	UINT srvIndex = 0;
};

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}
