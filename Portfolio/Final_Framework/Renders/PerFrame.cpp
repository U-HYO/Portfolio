#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * _shader)
	: shader(_shader)
{
	cBuffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sCBuffer = shader->AsConstantBuffer("CB_PerFrame");

	lightBuffer = Debug_new ConstantBuffer(&lightDesc, sizeof(LightDesc));
	sLightBuffer = shader->AsConstantBuffer("CB_Light");

	pointLightBuffer = Debug_new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_PointLights");

	spotLightBuffer = Debug_new ConstantBuffer(&spotLightDesc, sizeof(PointLightDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_SpotLights");

	fogBuffer = Debug_new ConstantBuffer(&fogDesc, sizeof(FogDesc));
	sFogBuffer = shader->AsConstantBuffer("CB_Fog");
}

PerFrame::~PerFrame()
{
	SAFE_DELETE(cBuffer);
	SAFE_DELETE(lightBuffer);
	SAFE_DELETE(pointLightBuffer);
	SAFE_DELETE(spotLightBuffer);
	SAFE_DELETE(fogBuffer);
}

void PerFrame::Update()
{
	desc.Time = Time::Get()->Running();

	desc.View = Context::Get()->View();
	D3DXMatrixInverse(&desc.ViewInverse, NULL, &desc.View);

	desc.Projection = Context::Get()->Projection();
	desc.VP = desc.View * desc.Projection;
	D3DXMatrixInverse(&desc.ProjectionInverse, NULL, &desc.Projection);

	lightDesc.Ambient =  Lighting::Get()->GetAmbient();
	lightDesc.Specular = Lighting::Get()->GetSpecular();
	lightDesc.Direction =Lighting::Get()->GetDirection();
	lightDesc.Position = Lighting::Get()->GetPosition();

	pointLightDesc.Count = Lighting::Get()->PointLights(pointLightDesc.Lights);
	spotLightDesc.Count = Lighting::Get()->SpotLights(spotLightDesc.Lights);

	memcpy(desc.Culling, Context::Get()->GetCulling(), sizeof(Plane) * 4);
	desc.Clipping = Context::Get()->GetClipping();

	fogDesc.FogColor = Lighting::Get()->FogColor();
	fogDesc.FogDistance = Lighting::Get()->FogDistance();
	fogDesc.FogDensity = Lighting::Get()->FogDensity();
	fogDesc.FogType = Lighting::Get()->FogType();
}

void PerFrame::Render()
{
	cBuffer->Render();
	sCBuffer->SetConstantBuffer(cBuffer->GetBuffer());

	lightBuffer->Render();
	sLightBuffer->SetConstantBuffer(lightBuffer->GetBuffer());

	pointLightBuffer->Render();
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->GetBuffer());

	spotLightBuffer->Render();
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->GetBuffer());

	fogBuffer->Render();
	sFogBuffer->SetConstantBuffer(fogBuffer->GetBuffer());
}
