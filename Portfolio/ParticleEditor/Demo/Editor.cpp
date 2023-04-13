#include "stdafx.h"
#include "Editor.h"
#include "Utilities/Xml.h"

void Editor::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(40, 2);

	//shader = Debug_new Shader(L"38_AreaLighting.fxo");
	shader = Debug_new Shader(L"48_EnvCube.fxo");
	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);

	Mesh();

	UpdateParticleList();
	UpdateTextureList();
	
	for (auto a : textures)
		SAFE_DELETE(a);
	textures.clear();
}

void Editor::Release()
{

	SAFE_DELETE(shader);
	SAFE_DELETE(sky);
	SAFE_DELETE(particle);
	SAFE_DELETE(floor);
	SAFE_DELETE(stone);
	SAFE_DELETE(sphere);
	SAFE_DELETE(grid);

	for (auto a : textures)
		SAFE_DELETE(a);
	textures.clear();
}

void Editor::Update()
{
	OnGUI();

	sky->Update();

	grid->Update();
	sphere->Update();


	Vector3 position;
	sphere->GetTransform(0)->GetPosition(&position);

	if (Keyboard::Get()->Press('L'))
		position.x += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('J'))
		position.x -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('I'))
		position.z += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('O'))
		position.y += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('U'))
		position.y -= 20 * Time::Delta();

	sphere->GetTransform(0)->SetPosition(position);
	sphere->UpdateTransforms();

	if (particle != NULL)
	{
		particle->Add(position);
		particle->Update();
	}
}

void Editor::Render()
{
	sky->Pass(4);
	sky->Render();

	
	
	stone->Render();
	floor->Render();

	grid->Render();
	
	sphere->Pass(8);
	sphere->Render();

	if (particle != NULL)
		particle->Render();

	
}

void Editor::Mesh()
{
	//Create Material
	{
		floor = Debug_new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->NormalMap("Floor_Normal.png");
		floor->Specular(1, 1, 1, 20);
		floor->SpecularMap("Floor_Specular.png");
		floor->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		stone = Debug_new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->NormalMap("Stones_Normal.png");
		stone->Specular(1, 1, 1, 20);
		stone->SpecularMap("Stones_Specular.png");
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
		
	}


	//Create Mesh
	{
		Transform* transform = NULL;

		grid = Debug_new MeshRender(shader, Debug_new MeshPlane(5, 5));
		transform = grid->AddTransform();
		transform->SetPosition(0, 0, 0);
		transform->SetScale(12, 1, 12);

		sphere = Debug_new MeshRender(shader, Debug_new MeshSphere(0.5f, 20, 20));
		transform = sphere->AddTransform();
		transform->SetPosition(0, 5, 0);
		transform->SetScale(5, 5, 5);
	}

	sphere->UpdateTransforms();
	grid->UpdateTransforms();
}

void Editor::UpdateParticleList()
{
	particleList.clear();

	std::vector<std::wstring> files;
	Path::GetFiles(&particleList, L"../../_Textures/Particles/", L"*.xml", false);

	for (std::wstring& file : particleList)
		file = Path::GetFileNameWithoutExtension(file);

}

void Editor::UpdateTextureList()
{
	textureList.clear();
	for (auto a : textures)
		SAFE_DELETE(a);
	textures.clear();

	std::vector<std::wstring> files;
	Path::GetFiles(&files, L"../../_Textures/Particles/", L"*.*", false);

	for (std::wstring file : files)
	{
		std::wstring ext = Path::GetExtension(file);
		std::transform(ext.begin(), ext.end(), ext.begin(), toupper);
		
		file = Path::GetFileName(file);
		if (ext == L"PNG" || ext == L"TGA" || ext == L"JPG")
		{
			textureList.push_back(file);

			textures.push_back(Debug_new Texture(L"Particles/" + file));
			//auto tex = Debug_new Texture(L"Particles/" + file);
			//textures.push_back(std::shared_ptr<Texture>(tex));

		} 
	}
}

void Editor::OnGUI()
{
	float width = D3D::Width();
	float height = D3D::Height();

	bool bOpen = true;
	bOpen = ImGui::Begin("Particle", &bOpen);
	ImGui::SetWindowPos(ImVec2(width - windowWidth, 0));
	ImGui::SetWindowSize(ImVec2(windowWidth, height));
	{
		OnGUI_List();
		OnGUI_Settings();
	}
	ImGui::End();

}

void Editor::OnGUI_List()
{
	if (ImGui::CollapsingHeader("Particle List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginGroup();
		for (UINT i = 0; i < particleList.size(); i++)
		{
			if (ImGui::Button(String::ToString(particleList[i]).c_str(), ImVec2(200, 0)))
			{
				SAFE_DELETE(particle);

				file = particleList[i];
				particle = Debug_new Particle(particleList[i]);

				bLoop = particle->GetData().bLoop;
				maxParticle = particle->GetData().MaxParticles;
			}
		}//for(i)
		ImGui::EndGroup();
		ImGui::SameLine();
		if (ImGui::Button("UpdateButton"))
		{
			UpdateParticleList();
			UpdateTextureList();
		}
		/*static bool a = false;
		a = ImGui::Button("Update Button", ImVec2(200, 0));

		if (a)
		{
			UpdateParticleList();
			UpdateTextureList();
		}*/

	}//ImGui::CollapsingHeader
}

void Editor::OnGUI_Settings()
{
	if (particle == NULL) return;

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Separator();

		ImGui::SliderInt("MaxParticles", (int *)&maxParticle, 1, 1000);
		ImGui::Checkbox("Loop", &bLoop);

		if (ImGui::Button("Apply"))
		{
			particle->GetData().bLoop = bLoop;
			particle->GetData().MaxParticles = maxParticle;
			particle->Reset();
		}


		ImGui::Separator();

		const char* types[] = { "Opaque", "Additive", "AlphaBlend" };
		ImGui::Combo("BlendType", (int *)&particle->GetData().Type, types, 3);

		static float dragSpeed = 0.01f;
		ImGui::DragFloat("DragSpeed", &dragSpeed, 0.001f, 0.0f, 10.0f);

		ImGui::DragFloat("ReadyTime", &particle->GetData().ReadyTime, dragSpeed, 0.1f, 10.0f);
		ImGui::DragFloat("ReadyRandomTime", &particle->GetData().ReadyRandomTime, dragSpeed, 0.0f, 100.0f);

		ImGui::DragFloat("StartVelocity", &particle->GetData().StartVelocity, dragSpeed, 0.0f, 10.0f);
		ImGui::DragFloat("EndVelocity", &particle->GetData().EndVelocity, dragSpeed, -100.0f, 100.0f);

		ImGui::DragFloat("MinHorizontalVelocity", &particle->GetData().MinHorizontalVelocity, dragSpeed, -100.0f, 100.0f);
		ImGui::DragFloat("MaxHorizontalVelocity", &particle->GetData().MaxHorizontalVelocity, dragSpeed, -100.0f, 100.0f);

		ImGui::DragFloat("MinVerticalVelocity", &particle->GetData().MinVerticalVelocity, dragSpeed, -100.0f, 100.0f);
		ImGui::DragFloat("MaxVerticalVelocity", &particle->GetData().MaxVerticalVelocity, dragSpeed, -100.0f, 100.0f);

		ImGui::DragFloat3("Gravity", particle->GetData().Gravity, dragSpeed, -100, 100);

		ImGui::ColorEdit4("MinColor", particle->GetData().MinColor);
		ImGui::ColorEdit4("MaxColor", particle->GetData().MaxColor);

		ImGui::DragFloat("MinRotateSpeed", &particle->GetData().MinRotateSpeed, dragSpeed, -10, 10);
		ImGui::DragFloat("MaxRotateSpeed", &particle->GetData().MaxRotateSpeed, dragSpeed, -10, 10);

		ImGui::DragFloat("MinStartSize", &particle->GetData().MinStartSize, dragSpeed, 0, 500);
		ImGui::DragFloat("MaxStartSize", &particle->GetData().MaxStartSize, dragSpeed, 0, 500);

		ImGui::DragFloat("MinEndSize", &particle->GetData().MinEndSize, dragSpeed, 0, 500);
		ImGui::DragFloat("MaxEndSize", &particle->GetData().MaxEndSize, dragSpeed, 0, 500);

		ImGui::Spacing();
		OnGUI_Write();


		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (std::wstring textureFile : textureList)
			{
				if (ImGui::Button(String::ToString(textureFile).c_str(), ImVec2(200, 0)))
				{
					particle->GetData().TextureFile = textureFile;
					particle->SetTexture(L"Particles/" + textureFile);
				}


			}//for(i)
		}

		for (auto testA : textures)
		{
			ImGui::Image(testA->SRV(), ImVec2(100, 100));

		}

	}
}

void Editor::OnGUI_Write()
{
	ImGui::Separator();

	if (ImGui::Button("WriteParticle"))
	{
		D3DDesc desc = D3D::GetDesc();

		Path::SaveFileDialog
		(
			file, L"Particle file\0*.xml", L"../../_Textures/Particles",
			bind(&Editor::WriteFile, this, std::placeholders::_1),
			desc.Handle
		);
	}
}


void Editor::WriteFile(std::wstring file)
{
	Xml::XMLDocument* document = Debug_new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Particle");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);


	Xml::XMLElement* node = NULL;

	node = document->NewElement("BlendState");
	node->SetText((int)particle->GetData().Type);
	root->LinkEndChild(node);


	std::string textureFile = String::ToString(particle->GetData().TextureFile);
	String::Replace(&textureFile, "Particles/", "");

	node = document->NewElement("Loop");
	node->SetText(particle->GetData().bLoop);
	root->LinkEndChild(node);

	node = document->NewElement("TextureFile");
	node->SetText(textureFile.c_str());
	root->LinkEndChild(node);


	node = document->NewElement("MaxParticles");
	node->SetText(particle->GetData().MaxParticles);
	root->LinkEndChild(node);


	node = document->NewElement("ReadyTime");
	node->SetText(particle->GetData().ReadyTime);
	root->LinkEndChild(node);

	node = document->NewElement("ReadyRandomTime");
	node->SetText(particle->GetData().ReadyRandomTime);
	root->LinkEndChild(node);

	node = document->NewElement("StartVelocity");
	node->SetText(particle->GetData().StartVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("EndVelocity");
	node->SetText(particle->GetData().EndVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("MinHorizontalVelocity");
	node->SetText(particle->GetData().MinHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxHorizontalVelocity");
	node->SetText(particle->GetData().MaxHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MinVerticalVelocity");
	node->SetText(particle->GetData().MinVerticalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxVerticalVelocity");
	node->SetText(particle->GetData().MaxVerticalVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("Gravity");
	node->SetAttribute("X", particle->GetData().Gravity.x);
	node->SetAttribute("Y", particle->GetData().Gravity.y);
	node->SetAttribute("Z", particle->GetData().Gravity.z);
	root->LinkEndChild(node);


	node = document->NewElement("MinColor");
	node->SetAttribute("R", particle->GetData().MinColor.r);
	node->SetAttribute("G", particle->GetData().MinColor.g);
	node->SetAttribute("B", particle->GetData().MinColor.b);
	node->SetAttribute("A", particle->GetData().MinColor.a);
	root->LinkEndChild(node);

	node = document->NewElement("MaxColor");
	node->SetAttribute("R", particle->GetData().MaxColor.r);
	node->SetAttribute("G", particle->GetData().MaxColor.g);
	node->SetAttribute("B", particle->GetData().MaxColor.b);
	node->SetAttribute("A", particle->GetData().MaxColor.a);
	root->LinkEndChild(node);


	node = document->NewElement("MinRotateSpeed");
	node->SetText(particle->GetData().MinRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MaxRotateSpeed");
	node->SetText(particle->GetData().MaxRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MinStartSize");
	node->SetText((int)particle->GetData().MinStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxStartSize");
	node->SetText((int)particle->GetData().MaxStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MinEndSize");
	node->SetText((int)particle->GetData().MinEndSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxEndSize");
	node->SetText((int)particle->GetData().MaxEndSize);
	root->LinkEndChild(node);

	std::wstring folder = Path::GetDirectoryName(file);
	std::wstring fileName = Path::GetFileNameWithoutExtension(file);

	document->SaveFile(String::ToString(folder + fileName + L".xml").c_str());
	SAFE_DELETE(document);

	UpdateParticleList();
}
