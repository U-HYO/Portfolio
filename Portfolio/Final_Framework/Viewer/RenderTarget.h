#pragma once
class RenderTarget
{
public:
	RenderTarget(float width = 0, float height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	ID3D11RenderTargetView* GetRTV() { return rtv; }
	ID3D11ShaderResourceView* GetSRV() { return srv; }

	void SaveTexture(std::wstring file);

	void PreRender(class DepthStencil* depthStencil);
	static void PreRender(RenderTarget** targets, UINT count, DepthStencil* depthStencil);


private:
	float width, height;
	DXGI_FORMAT format;


	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;


};

