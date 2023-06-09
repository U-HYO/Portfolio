#include "Framework.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(float width, float height, DXGI_FORMAT format)
	: format(format)
{
	this->width = (width < 1) ? D3D::Width() : width;
	this->height = (height < 1) ? D3D::Height() : height;


	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)this->width;
	textureDesc.Height = (UINT)this->height;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	Check(D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc; 
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Check(D3D::GetDevice()->CreateRenderTargetView(texture, &rtvDesc, &rtv)); // 결과물 출력

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv));
}

RenderTarget::~RenderTarget()
{
	SAFE_RELEASE(texture);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(srv);
}

void RenderTarget::SaveTexture(std::wstring file)
{
	Check(D3DX11SaveTextureToFile(D3D::GetDC(), texture, D3DX11_IFF_PNG, file.c_str()));
}

void RenderTarget::PreRender(DepthStencil * depthStencil)
{
	D3D::GetDC()->OMSetRenderTargets(1, &rtv, depthStencil->DSV());
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, depthStencil->DSV());
}

void RenderTarget::PreRender(RenderTarget ** targets, UINT count, DepthStencil * depthStencil)
{
	std::vector<ID3D11RenderTargetView*> rtvs;
	for (UINT i = 0; i < count; i++)
	{
		ID3D11RenderTargetView* rtv = targets[i]->GetRTV();
		rtvs.push_back(rtv);

		D3D::GetDC()->ClearRenderTargetView(rtv, Color(0, 0, 0, 1));
	}
	D3D::GetDC()->ClearDepthStencilView(depthStencil->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	D3D::GetDC()->OMSetRenderTargets(count, &rtvs[0], depthStencil->DSV());

}