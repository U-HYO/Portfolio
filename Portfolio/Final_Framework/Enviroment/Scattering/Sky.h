#pragma once

class Sky : public IComponent
{
private:
	struct CloudDesc;

public:
	Sky(Shader* shader);
	Sky(class Actor* _actor, class Transform* _transform);

	~Sky();

	void Pass(UINT scatteringPass, UINT domePass, UINT moonPass, UINT cloudPass);
	void Pass(UINT pass);

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnStop() override;
	void OnDestroy() override;

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	void RealTime(bool val, float theta, float timeFactor = 1.0f);

	void SetShader(Shader* shader);

	float GetTheta() { return theta; }
	void SetTheta(float _theta) { theta = _theta; }

	bool GetRealTime() { return bRealTime; }
	void SetRealTime(bool _bRealTime) { bRealTime = _bRealTime; }

	CloudDesc GetCloudDesc() { return cloudDesc; }
	void SetCloudDesc(CloudDesc cloud) { cloudDesc = cloud; }

	Transform* GetCloudTransform();
	void SetCloudTransform(Transform* _transform);

private:
	struct ScatterDesc
	{
		Vector3 WaveLength = Vector3(0.65f, 0.57f, 0.475f);
		float Padding;

		Vector3 InvWaveLength;
		int SampleCount = 8;

		Vector3 WaveLengthMie;
		float Padding2;
	} scatterDesc;

	struct CloudDesc
	{
		float Tiles = 1.5f;
		float Cover = 0.005f;
		float Sharpness = 0.405f;
		float Speed = 0.05f;
	} cloudDesc;


private:
	Shader* shader;

	bool bRealTime = false;

	float timeFactor = 1.0f;
	float theta = 0.0f;

	class Scattering* scattering;
	ConstantBuffer* scatterBuffer;
	ID3DX11EffectConstantBuffer* sScatterBuffer;

	ID3DX11EffectShaderResourceVariable* sRayleighMap;
	ID3DX11EffectShaderResourceVariable* sMieMap;


	class Dome* dome;
	class Moon* moon;

	class Cloud* cloud;
	ConstantBuffer* cloudBuffer;
	ID3DX11EffectConstantBuffer* sCloudBuffer;




};