#pragma once
class Shadow
{

public:
	Shadow(Shader* shader, Vector3& position, float radius, float width = 1024, float height = 1024);
	~Shadow();

	void PreRender();

	ID3D11ShaderResourceView* GetSRV() { return renderTarget->GetSRV(); }
	//�׸��� ����Ҷ� DSV �̰� ��� Ȯ�ο�


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

	float width, height;	//RTV, DSV �ػ�(�׸����� �ػ�)

	float radius;			//���� �ݰ�
	Vector3 position;		//���� ��ġ

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

