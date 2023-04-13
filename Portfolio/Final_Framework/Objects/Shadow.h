#pragma once
class Shadow
{

public:
	Shadow(Shader* shader, Vector3& position, float radius, float width = 1024, float height = 1024);
	~Shadow();

	void PreRender();

	ID3D11ShaderResourceView* GetSRV() { return renderTarget->GetSRV(); }
	//그림자 계산할땐 DSV 이건 결과 확인용


private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;

		Vector2 MapSize;
		float Bias = 0.0f;

		UINT Quality = 2;
	}desc;

private:
	Shader* shader;

	float width, height;	//RTV, DSV 해상도(그림자의 해상도)

	float radius;			//조명 반경
	Vector3 position;		//조명 위치

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ID3DX11EffectShaderResourceVariable* sShadowMap;

	//pcf (percentage closer filtering) 
	ID3D11SamplerState* pcfSampler;
	ID3DX11EffectSamplerVariable* sPcfSampler;


};

