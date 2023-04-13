#pragma once

class CubeSky : public IComponent
{
public:
	CubeSky(std::wstring _file, Shader* shader = nullptr);
	CubeSky(class Actor* _actor, class Transform* _transform);
	~CubeSky();

	void Update();
	void Render();

	void Pass(UINT val) { pass = val; }

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnStop() override;
	void OnDestroy() override;

	void SetShader(Shader* shader);
	void SetCubeSky(std::wstring _file);

	ID3D11ShaderResourceView* GetSrv() { return srv; }
private:
	Shader* shader;

	UINT pass = 0;
	bool bCreateShader = false;
	MeshRender* sphere;


	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSrv;

public:
	Transform* GetTransform() { return sphere->GetTransform(0); }
	
};

