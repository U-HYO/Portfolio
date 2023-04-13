#include "Framework.h"
#include "Sky.h"
#include "Scattering.h"
#include "Dome.h"
#include "Moon.h"
#include "Cloud.h"

Sky::Sky(Shader * shader)
	: shader(shader)
	, IComponent(nullptr, nullptr)
{
	scatterDesc.InvWaveLength.x = 1.0f / powf(scatterDesc.WaveLength.x, 4.0f);
	scatterDesc.InvWaveLength.y = 1.0f / powf(scatterDesc.WaveLength.y, 4.0f);
	scatterDesc.InvWaveLength.z = 1.0f / powf(scatterDesc.WaveLength.z, 4.0f);

	scatterDesc.WaveLengthMie.x = powf(scatterDesc.WaveLength.x, -0.84f);
	scatterDesc.WaveLengthMie.y = powf(scatterDesc.WaveLength.y, -0.84f);
	scatterDesc.WaveLengthMie.z = powf(scatterDesc.WaveLength.z, -0.84f);

	scattering = Debug_new Scattering(shader);
	scatterBuffer = Debug_new ConstantBuffer(&scatterDesc, sizeof(ScatterDesc));
	sScatterBuffer = shader->AsConstantBuffer("CB_Scattering");


	sRayleighMap = shader->AsSRV("RayleighMap");
	sMieMap = shader->AsSRV("MieMap");


	dome = Debug_new Dome(shader, Vector3(0, 16, 0), Vector3(800, 800, 800));
	moon = Debug_new Moon(shader);

	cloud = Debug_new Cloud(shader);
	cloudBuffer = Debug_new ConstantBuffer(&cloudDesc, sizeof(CloudDesc));
	sCloudBuffer = shader->AsConstantBuffer("CB_Cloud");
}

Sky::Sky(Actor * _actor, Transform * _transform)
	:IComponent(_actor, _transform)
{

}

Sky::~Sky()
{
	SAFE_DELETE(scattering);
	SAFE_DELETE(scatterBuffer);

	SAFE_DELETE(dome);
	SAFE_DELETE(moon);

	SAFE_DELETE(cloud);
	SAFE_DELETE(cloudBuffer);
}

void Sky::Pass(UINT scatteringPass, UINT domePass, UINT moonPass, UINT cloudPass)
{
	scattering->Pass(scatteringPass);
	dome->Pass(domePass);
	moon->Pass(moonPass);
	cloud->Pass(cloudPass);
}

void Sky::Pass(UINT pass)
{
	scattering->Pass(pass++);
	dome->Pass(pass++);
	moon->Pass(pass++);
	cloud->Pass(pass);
}

void Sky::Update()
{
	if (bRealTime == true)
	{
		theta += Time::Delta() * timeFactor;

		if (theta > Math::PI)
			theta -= Math::PI * 2.0f;


		float x = sinf(theta);
		float y = cosf(theta);

		Lighting::Get()->GetDirection() = Vector3(x, y, 0.0f);
	}
	else
	{
		ImGui::SliderFloat("Theta", &theta, -Math::PI, Math::PI);

		float x = sinf(theta);
		float y = cosf(theta);

		Lighting::Get()->GetDirection() = Vector3(x, y, 0.0f);
	}

	scattering->Update();


	Vector3 domePosition = Vector3(0, 0, 0);
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->GetPosition(&domePosition);

	dome->GetTransform()->SetPosition(domePosition);
	dome->Update();

	moon->Update();

	cloud->GetTransform()->SetPosition(domePosition);
	cloud->Update();
}

void Sky::PreRender()
{
	scatterBuffer->Render();
	sScatterBuffer->SetConstantBuffer(scatterBuffer->GetBuffer());

	scattering->PreRender();
}

void Sky::Render()
{
	//Dome
	{
		sRayleighMap->SetResource(scattering->RayleighRTV()->GetSRV());
		sMieMap->SetResource(scattering->MieRTV()->GetSRV());

		dome->Render();
	}

	//Moon
	{
		moon->Render(theta);
	}

	//Cloud
	{
		cloudBuffer->Render();
		sCloudBuffer->SetConstantBuffer(cloudBuffer->GetBuffer());

		cloud->Render();
	}
}

void Sky::PostRender()
{
	scattering->PostRender();
	cloud->PostRender();
}

void Sky::RealTime(bool val, float theta, float timeFactor)
{
	bRealTime = val;

	this->theta = theta;
	this->timeFactor = timeFactor;
}

void Sky::SetShader(Shader * shader)
{
	this->shader = shader;
	scatterDesc.InvWaveLength.x = 1.0f / powf(scatterDesc.WaveLength.x, 4.0f);
	scatterDesc.InvWaveLength.y = 1.0f / powf(scatterDesc.WaveLength.y, 4.0f);
	scatterDesc.InvWaveLength.z = 1.0f / powf(scatterDesc.WaveLength.z, 4.0f);

	scatterDesc.WaveLengthMie.x = powf(scatterDesc.WaveLength.x, -0.84f);
	scatterDesc.WaveLengthMie.y = powf(scatterDesc.WaveLength.y, -0.84f);
	scatterDesc.WaveLengthMie.z = powf(scatterDesc.WaveLength.z, -0.84f);

	scattering = Debug_new Scattering(shader);
	scatterBuffer = Debug_new ConstantBuffer(&scatterDesc, sizeof(ScatterDesc));
	sScatterBuffer = shader->AsConstantBuffer("CB_Scattering");


	sRayleighMap = shader->AsSRV("RayleighMap");
	sMieMap = shader->AsSRV("MieMap");


	dome = Debug_new Dome(shader, Vector3(0, 16, 0), Vector3(800, 800, 800));
	moon = Debug_new Moon(shader);

	cloud = Debug_new Cloud(shader);
	cloudBuffer = Debug_new ConstantBuffer(&cloudDesc, sizeof(CloudDesc));
	sCloudBuffer = shader->AsConstantBuffer("CB_Cloud");
}

void Sky::OnInit()
{
}

void Sky::OnStart()
{
}

void Sky::OnUpdate()
{
	Update();
}

void Sky::OnRender()
{
	Render();
}

void Sky::OnStop()
{
}

void Sky::OnDestroy()
{
}

Transform * Sky::GetCloudTransform()
{
	return cloud->GetTransform();
}

void Sky::SetCloudTransform(Transform * _transform)
{
	cloud->SetTrasnform(_transform);
}