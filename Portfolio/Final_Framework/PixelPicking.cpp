#include "Framework.h"
#include "PixelPicking.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

PixelPicking::PixelPicking()
{
	//ID3D11Texture2D* renderTargetTexture;
	//D3D11_TEXTURE2D_DESC textureDesc;
	//ZeroMemory(&textureDesc, sizeof(textureDesc));
	//textureDesc.Width = screenWidth;
	//textureDesc.Height = screenHeight;
	//textureDesc.MipLevels = 1;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//textureDesc.CPUAccessFlags = 0;
	//textureDesc.MiscFlags = 0;
	//device->CreateTexture2D(&textureDesc, NULL, &renderTargetTexture);

	//ID3D11RenderTargetView* renderTargetView;
	//D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	//ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	//renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//renderTargetViewDesc.Texture2D.MipSlice = 0;
	//device->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);

	//deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

	//D3D11_TEXTURE2D_DESC textureDesc;
	//renderTargetTexture->GetDesc(&textureDesc);

	//D3D11_TEXTURE2D_DESC textureDescRead;
	//textureDescRead.Width = textureDesc.Width;
	//textureDescRead.Height = textureDesc.Height;
	//textureDescRead.MipLevels = 1;
	//textureDescRead.ArraySize = 1;
	//textureDescRead.Format = textureDesc.Format;
	//textureDescRead.SampleDesc.Count = 1;
	//textureDescRead.SampleDesc.Quality = 0;
	//textureDescRead.Usage = D3D11_USAGE_STAGING;
	//textureDescRead.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//textureDescRead.MiscFlags = 0;

	//ID3D11Texture2D* texture;

	//device->CreateTexture2D(&textureDescRead, NULL, &texture);

	//deviceContext->CopyResource(texture, renderTargetTexture);

	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//deviceContext->Map(texture, 0, D3D11_MAP_READ, 0, &mappedResource);

	//// 'mappedResource.pData' now contains the pixel data of the render target

	//deviceContext->Unmap(texture, 0);

	//texture->Release();

}
