#include "Framework.h"
#include "EnvCube.h"

EnvCube::EnvCube(Shader * shader, float width, float height)
	: shader(shader), width(width), height(height)
{
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Create Texture2D - RTV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.ArraySize = 6; //6면 , 텍스쳐는 하나지만 안의 면이 6개
		desc.Format = rtvFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &rtvTexture));
	}

	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateRenderTargetView(rtvTexture, &desc, &rtv));
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // shader용이라 CUBE
		desc.TextureCube.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(rtvTexture, &desc, &srv));
	}


	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//Create Texture - DSV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.ArraySize = 6;
		desc.Format = dsvFormat;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &dsvTexture));
	}

	//CreateDSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = dsvFormat;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv));
	}

	viewport = Debug_new Viewport((float)width, (float)height);

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_EnvCube");

	sSrv = shader->AsSRV("EnvCubeMap");

	projection = Debug_new Perspective(1, 1);
}

EnvCube::~EnvCube()
{
	SAFE_RELEASE(rtvTexture);
	SAFE_RELEASE(srv);
	SAFE_RELEASE(rtv);

	SAFE_RELEASE(dsvTexture);
	SAFE_RELEASE(dsv);

	SAFE_DELETE(viewport);
	SAFE_DELETE(buffer);
	
	SAFE_DELETE(projection);
}

void EnvCube::PreRender(Vector3 & position, Vector3 & scale, float zNear, float zFar, float fov)
{
	//Create ViewMatrix
	{
		float x = position.x;
		float y = position.y;
		float z = position.z;


		struct LookAt
		{
			Vector3 At;
			Vector3 Up;
		} lookAt[6];

		lookAt[0] = { Vector3(x + scale.x, y, z), Vector3(0, 1, 0) };
		lookAt[1] = { Vector3(x - scale.x, y, z), Vector3(0, 1, 0) };
		lookAt[2] = { Vector3(x, y + scale.y, z), Vector3(0, 0, -1) };
		lookAt[3] = { Vector3(x, y - scale.y, z), Vector3(0, 0, +1) };
		lookAt[4] = { Vector3(x, y, z + scale.z), Vector3(0, 1, 0) };
		lookAt[5] = { Vector3(x, y, z - scale.z), Vector3(0, 1, 0) };

		for (UINT i = 0; i < 6; i++)
			D3DXMatrixLookAtLH(&desc.Views[i], &position, &lookAt[i].At, &lookAt[i].Up);
	}

	((Perspective *)projection)->Set(1, 1, zNear, zFar, fov * Math::PI);
	projection->GetMatrix(&desc.Projection);


	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, dsv);

	viewport->RSSetViewport();
}

void EnvCube::Render()
{
	sSrv->SetResource(srv);
}
