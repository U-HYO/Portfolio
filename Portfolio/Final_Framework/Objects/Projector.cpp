#include "Framework.h"
#include "Projector.h"

Projector::Projector(Shader * shader, std::wstring mapFile, float width, float height)
	:shader(shader), width(width), height(height)
{
	camera = Debug_new Fixity();
	camera->GetTransform()->SetPosition(0, 0, -20);
	camera->GetTransform()->SetRotation(Math::PI *  0.5f, 0, 0);
	camera->GetTransform()->SetScale(width, height, 1);
	projection = Debug_new Orthographic(width, height);
	//projection = Debug_new Perspective(width, height, 10, 100, Math::PI * 0.25f);

	map = Debug_new Texture(mapFile);
	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));


	sMap = shader->AsSRV("ProjectorMap");
	sMap->SetResource(map->SRV());

	sBuffer = shader->AsConstantBuffer("CB_Projector");
	collider = Debug_new Collider(camera->GetTransform());
}

Projector::~Projector()
{
	SAFE_DELETE(camera);
	SAFE_DELETE(projection);

	SAFE_DELETE(map);
	SAFE_DELETE(buffer);

	SAFE_DELETE(collider);
}

void Projector::Update()
{
	Vector3 position;
	camera->GetTransform()->GetPosition(&position);
	
	ImGui::SliderFloat3("Position", position, -100, 100);
	camera->GetTransform()->SetPosition(position);
	
	ImGui::ColorEdit3("Color", desc.Color);


	//Perspective
	{
		static float width = this->width, height = this->height;
		static float n = 1.0f, f = 100.0f;
		static float fov = 0.25f;

		ImGui::SliderFloat("Width", &width, 0, 100);
		ImGui::SliderFloat("Height", &height, 0, 100);
		ImGui::SliderFloat("Near", &n, 0, 200);
		ImGui::SliderFloat("Far", &f, 0, 200);
		ImGui::SliderFloat("Fov", &fov, 0, Math::PI * 2.0f);

		((Perspective *)projection)->Set(width, height, n, f, Math::PI * fov);
		
	}

	camera->GetMatrix(&desc.View);
	camera->Update();
	projection->GetMatrix(&desc.Projection);

	collider->Update();
}

void Projector::Render()
{
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	collider->Render();
}
