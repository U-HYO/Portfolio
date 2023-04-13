#include "stdafx.h"
#include "UI.h"
#include "../Helper/FileDialog.h"
#include "Utilities/Xml.h"

void UI::Init()
{
	showUI = true;

	showAdd = false;
	showInspector = false;
	showSetting = false;

	showSecond = false;
	showThird = false;

	selectedActor = nullptr;

	fileDialogView = Debug_new FileDialog(nullptr, FileDialogType::Browser, FileDialogOperation::Load, FileDialogFilter::All, false);
	fileDialogLoad = Debug_new FileDialog(nullptr, FileDialogType::FileSelection, FileDialogOperation::Load, FileDialogFilter::All, true);
	
	inspecFileDialogView = Debug_new FileDialog(nullptr, FileDialogType::Browser, FileDialogOperation::Load, FileDialogFilter::All, false);

	thirdType = ComponentType::Unknown;
	IconProvider::Get().Init(Context::Get());
	
	nodeClicked = std::numeric_limits<UINT>().max();

	showFileDialog = false;
	fileDialogUpdate = false;
}

void UI::Release()
{
	SAFE_DELETE(fileDialogLoad);
	SAFE_DELETE(fileDialogView);
	SAFE_DELETE(inspecFileDialogView);
}

void UI::Update()
{
	if (_KEYCODE->Down('U'))
		showUI = !showUI;



}

void UI::Render()
{
	if (showUI)
	{
		LeftSide();
		RightSide();
	}


	





}

void UI::LeftSide()
{
	LeftFirst();

	if (showSecond)
	{
		ImGui::SameLine(100);
		LeftSecond();



		LeftThird();
	}


}

void UI::LeftFirst()
{
	//First 
	ImGui::Begin
	(
		"LeftFirst", NULL,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		//ImGuiWindowFlags_NoSavedSettings |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus|
		ImGuiWindowFlags_NoBackground

	);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));

	ImGui::PushStyleColor
	(
		ImGuiCol_Button,
		ImGui::GetStyle().Colors[showAdd ? ImGuiCol_ButtonActive : ImGuiCol_Button]
	);
	if (ImGui::Button("Add", ImVec2(80, 80)))
	{
		showAdd = !showAdd;
		showSecond = showAdd;
		showInspector = false;
		showSetting = false;
		if (showThird)
			showThird = !showThird;
		if (!showSecond)
			showThird = false;
	}
	ImGui::PopStyleColor();

	ImGui::PushStyleColor
	(
		ImGuiCol_Button,
		ImGui::GetStyle().Colors[showInspector ? ImGuiCol_ButtonActive : ImGuiCol_Button]
	);
	if(ImGui::Button("Inspector", ImVec2(80, 80)))
	{
		showInspector = !showInspector;
		showSecond = showInspector;
		showAdd = false;
		showSetting = false;
		showThird = false;
		thirdType = ComponentType::Unknown;
	}
	ImGui::PopStyleColor();

	ImGui::PushStyleColor
	(
		ImGuiCol_Button,
		ImGui::GetStyle().Colors[showSetting ? ImGuiCol_ButtonActive : ImGuiCol_Button]
	);
	if(ImGui::Button("Settings", ImVec2(80, 80)))
	{
		showSetting = !showSetting;
		showSecond = showSetting;
		showAdd = false;
		showInspector = false;
		showThird = false;
		thirdType = ComponentType::Unknown;
	}
	ImGui::PopStyleColor();


	ImGui::PopStyleVar();

	ImGui::End();

}

void UI::LeftSecond()
{
	//Second
	ImGuiWindowFlags flag = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;


	if (showAdd)
	{
		ImGui::SetNextWindowSize(ImVec2(150, 300));
		flag |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("LeftSceond", NULL, flag);
		if(!showThird)
			thirdType = ComponentType::Unknown;


		//Models Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Animator ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Models", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/Models";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			showThird = !showThird;
			if (thirdType != ComponentType::Animator)
			{
				thirdType = ComponentType::Animator;
				showThird = true;
			}
			
		}
		ImGui::PopStyleColor();

		//Object Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::ObjectModel ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Objects", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/Objects";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			showThird = !showThird;
			if (thirdType != ComponentType::ObjectModel)
			{
				thirdType = ComponentType::ObjectModel;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//Skybox Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Skybox ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Skycube", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/CubeMaps";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			showThird = !showThird;
			if (thirdType != ComponentType::Skybox)
			{
				thirdType = ComponentType::Skybox;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Sky ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Sky", ImVec2(100, 20)))
		{
			showThird = !showThird;
			if (thirdType != ComponentType::Sky)
			{
				thirdType = ComponentType::Sky;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//Mesh Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::ObjectMesh ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Mesh", ImVec2(100, 20)))
		{
			showThird = !showThird;
			if (thirdType != ComponentType::ObjectMesh)
			{
				thirdType = ComponentType::ObjectMesh;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//Terrain Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Renderable ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Terrain", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/Terrain";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			fileDialogView->SetFilterName(".dds");
			showThird = !showThird;
			if (thirdType != ComponentType::Renderable)
			{
				thirdType = ComponentType::Renderable;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//Water Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Water ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Water", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/Water";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			showThird = !showThird;
			if (thirdType != ComponentType::Water)
			{
				thirdType = ComponentType::Water;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//TerrainLOD Button
		//ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Renderable ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		//if (ImGui::Button("TerrainLOD", ImVec2(100, 20)))
		//{
		//	clickedDirectory = "../../_Assets/Terrain";
		//	fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
		//	fileDialogView->SetFilterName(".dds");
		//	showThird = !showThird;
		//	if (thirdType != ComponentType::Renderable)
		//	{
		//		thirdType = ComponentType::Renderable;
		//		showThird = true;
		//	}
		//}
		//ImGui::PopStyleColor();

		//Particle Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Particle ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Particle", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/Particles";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			fileDialogView->SetFilterName(".xml");
			showThird = !showThird;
			if (thirdType != ComponentType::Particle)
			{
				thirdType = ComponentType::Particle;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//Weather Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Weather ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Weather", ImVec2(100, 20)))
		{
			clickedDirectory = "../../_Assets/Weather";
			fileDialogView->Show(&fileDialogUpdate, &clickedDirectory);
			fileDialogView->SetFilterName(".png");
			showThird = !showThird;
			if (thirdType != ComponentType::Weather)
			{
				thirdType = ComponentType::Weather;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		//Light Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[thirdType == ComponentType::Light ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::Button("Light", ImVec2(100, 20)))
		{
			showThird = !showThird;
			if (thirdType != ComponentType::Light)
			{
				thirdType = ComponentType::Light;
				showThird = true;
			}
		}
		ImGui::PopStyleColor();

		ImGui::End();
	}
	ShowInspector();
	

	if (showSetting)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 650));
		ImGui::Begin("LeftSceond", NULL, flag);

		ImGui::Spacing();

		Color fogColor = Lighting::Get()->FogColor();
		Vector2 fogDistance = Lighting::Get()->FogDistance();
		float fogDensity = Lighting::Get()->FogDensity();
		UINT fogType = Lighting::Get()->FogType();
		ImGui::ColorEdit4("FogColor", fogColor);
		ImGui::SliderFloat2("FogDistance", fogDistance, 0, 150);
		ImGui::SliderFloat("FogDensity", &fogDensity, 0.0f, 1.0f);
		
		ImGui::InputInt("FogType", (int*)&fogType);
		fogType = Math::Clamp<UINT>(fogType, 0, 2);

		Lighting::Get()->SetFogColor(fogColor);
		Lighting::Get()->SetFogDistance(fogDistance);
		Lighting::Get()->SetFogDensity(fogDensity);
		Lighting::Get()->SetFogType(fogType);
		ImGui::Separator();
		
		static UINT rainPass = 0;
		static UINT snowPass = 0;

		for (auto a : EditorHelper::Get()->GetActors())
		{
			if (a->HasRender<PostEffect>())
				a->SetIsActive(false);
		}

		const char* items[] = {"Default", "Bloom", "Blur", "ColorTone"};
		static int cur_item = 0;
		ImGui::Combo("", &cur_item, items, IM_ARRAYSIZE(items));


		switch (static_cast<PostEffectType>(cur_item))
		{
			case PostEffectType::Unknown:
				break;
			case PostEffectType::Bloom:
			{
			ImGui::InputInt("SrvIndex", (int *)&EditorHelper::Get()->srvIndex);
			EditorHelper::Get()->srvIndex %= 5;

			auto postEffect = EditorHelper::Get()->GetActors()[1]->GetRender<PostEffect>();
			ImGui::InputInt("BlurCount", (int *)&EditorHelper::Get()->blurCount, 2);
			EditorHelper::Get()->blurCount = Math::Clamp<UINT>(EditorHelper::Get()->blurCount, 1, 15);

			//Compositie
			ImGui::SliderFloat("BlurRatio", &EditorHelper::Get()->blurRatio, 0, 1);

			//Color Grading
			ImGui::InputFloat("Grading", &EditorHelper::Get()->grading, 0.01f);
			ImGui::InputFloat("Correlation", &EditorHelper::Get()->correlation, 0.01f);
			ImGui::InputFloat("Concentration", &EditorHelper::Get()->concentration, 0.01f);
			break;
		}
			case PostEffectType::Blur:
			{
			auto postEffect = EditorHelper::Get()->GetActors()[2]->GetRender<PostEffect>();

			static UINT Pass = postEffect->GetShader()->PassCount() - 1;
			ImGui::InputInt("Blur Pass", (int *)&Pass);
			Pass %= postEffect->GetShader()->PassCount();
			postEffect->Pass(Pass);

			//Blur
			{
				static UINT BlurCount = 1;
				ImGui::InputInt("Blur Count", (int *)&BlurCount);
				BlurCount %= 32;
				postEffect->GetShader()->AsScalar("BlurCount")->SetInt(BlurCount);
			}

			//RadialBlur
			{
				ImGui::Separator();

				static UINT BlurCount = 8;
				ImGui::InputInt("RadialBlurCount", (int *)&BlurCount);
				BlurCount %= 32;
				postEffect->GetShader()->AsScalar("RadialBlurCount")->SetInt(BlurCount);

				static float Radius = 0.6f;
				ImGui::InputFloat("RadialBlurRadius", &Radius, 0.01f);
				postEffect->GetShader()->AsScalar("RadialBlurRadius")->SetFloat(Radius);

				static float Amount = 0.04f;
				ImGui::InputFloat("RadialBlurAmount", &Amount, 0.001f);
				postEffect->GetShader()->AsScalar("RadialBlurAmount")->SetFloat(Amount);


				static float CenterX = 0.5f;
				ImGui::InputFloat("CenterX", &CenterX, 0.01f);

				static float CenterY = 0.5f;
				ImGui::InputFloat("CenterY", &CenterY, 0.01f);
				postEffect->GetShader()->AsVector("RadialCenter")->SetFloatVector(Vector2(CenterX, CenterY));
			}
			break;
		}
			case PostEffectType::ColorTone:
			{
				auto postEffect = EditorHelper::Get()->GetActors()[3]->GetRender<PostEffect>();

				const char* effectType[] = { "Default", "Inverse", "GrayScale", "GrayScale2", "Sepia", "Saturation", "Sharpness", "Wiggle", "Vignette", "Interace", "LensDistortion" };

				static int Pass = 0;
				//ImGui::InputInt("PostEffect Pass", (int *)&Pass);
				ImGui::Combo("ColorTone", &Pass, effectType, IM_ARRAYSIZE(effectType));
				Pass %= postEffect->GetShader()->PassCount();
				postEffect->Pass(Pass);

				static float thick = 1.0f;
				ImGui::InputFloat("Thick", &thick, 0.01f);

				Vector2 PixelSize = Vector2(thick / D3D::Width(), thick / D3D::Height());
				postEffect->GetShader()->AsVector("PixelSize")->SetFloatVector(PixelSize);

				//Saturation
				{
					ImGui::Separator();

					static float Saturation = 0.0f;
					//float Saturation = Math::Random(-5.0f, 5.0f);
					ImGui::InputFloat("Saturation", &Saturation, 0.1f);
					postEffect->GetShader()->AsScalar("Saturation")->SetFloat(Saturation);
				}

				//Sharpness
				{
					ImGui::Separator();

					static float Sharpness = 0.5f;
					ImGui::InputFloat("Sharpness", &Sharpness, 0.01f);
					postEffect->GetShader()->AsScalar("Sharpness")->SetFloat(Sharpness);

					static Color edgeColor = Color(1.0, 1.0f, 1.0f, 1.0f);
					ImGui::ColorEdit4("EdgeColor", edgeColor);
					postEffect->GetShader()->AsScalar("colorR")->SetFloat(edgeColor.r);
					postEffect->GetShader()->AsScalar("colorG")->SetFloat(edgeColor.g);
					postEffect->GetShader()->AsScalar("colorB")->SetFloat(edgeColor.b);
				}

				//Wiggle
				{
					ImGui::Separator();

					static float OffsetX = 10;
					ImGui::InputFloat("OffsetX", &OffsetX, 0.1f);

					static float OffsetY = 10;
					ImGui::InputFloat("OffsetY", &OffsetY, 0.1f);
					postEffect->GetShader()->AsVector("WiggleOffset")->SetFloatVector(Vector2(OffsetX, OffsetY));

					static float AmountX = 0.01f;
					ImGui::InputFloat("AmountX", &AmountX, 0.001f);

					static float AmountY = 0.01f;
					ImGui::InputFloat("AmountY", &AmountY, 0.001f);
					postEffect->GetShader()->AsVector("WiggleAmount")->SetFloatVector(Vector2(AmountX, AmountY));
				}

				//Vignette
				{
					ImGui::Separator();

					static float Power = 2.0f;
					ImGui::InputFloat("Power", &Power, 0.05f);
					postEffect->GetShader()->AsScalar("Power")->SetFloat(Power);

					static float ScaleX = 2.0f;
					ImGui::InputFloat("ScaleX", &ScaleX, 0.05f);

					static float ScaleY = 2.0f;
					ImGui::InputFloat("ScaleY", &ScaleY, 0.05f);
					postEffect->GetShader()->AsVector("Scale")->SetFloatVector(Vector2(ScaleX, ScaleY));
				}

				//Interace
				{
					ImGui::Separator();

					static float Strength = 1.0f;
					ImGui::InputFloat("Strength", &Strength, 0.01f);
					postEffect->GetShader()->AsScalar("Strength")->SetFloat(Strength);

					static int InteraceValue = 2;
					ImGui::InputInt("InteraceValue", &InteraceValue);
					postEffect->GetShader()->AsScalar("InteraceValue")->SetInt(InteraceValue);
				}

				//LensDistortion
				{
					ImGui::Separator();

					static float LensPower = 1.0f;
					ImGui::InputFloat("LensPower", &LensPower, 0.01f);
					postEffect->GetShader()->AsScalar("LensPower")->SetFloat(LensPower);

					static float Distortion = -0.02f;
					ImGui::InputFloat("Distortion", &Distortion, 0.001f);

					static float ChromaticDistortion = -0.01f;
					ImGui::InputFloat("ChromaticDistortion", &ChromaticDistortion, 0.001f);

					Vector3 temp = Vector3(Distortion, 0, 0);
					temp.y = Distortion + ChromaticDistortion;
					temp.z = Distortion + 2 * ChromaticDistortion;

					postEffect->GetShader()->AsVector("Distortion")->SetFloatVector(temp);
				}

			//	switch (Pass)
			//{
			//	case 0 :
			//		break;
			//	case 1:		//Saturation
			//	{
			//		ImGui::Separator();
			//
			//		static float Saturation = 0.0f;
			//		//float Saturation = Math::Random(-5.0f, 5.0f);
			//		ImGui::InputFloat("Saturation", &Saturation, 0.1f);
			//		postEffect->GetShader()->AsScalar("Saturation")->SetFloat(Saturation);
			//	}
			//	case 2: //Sharpness
			//	{
			//		ImGui::Separator();
			//
			//		static float Sharpness = 0.5f;
			//		ImGui::InputFloat("Sharpness", &Sharpness, 0.01f);
			//		postEffect->GetShader()->AsScalar("Sharpness")->SetFloat(Sharpness);
			//
			//		static Color edgeColor = Color(1.0, 1.0f, 1.0f, 1.0f);
			//		ImGui::ColorEdit4("EdgeColor", edgeColor);
			//		postEffect->GetShader()->AsScalar("colorR")->SetFloat(edgeColor.r);
			//		postEffect->GetShader()->AsScalar("colorG")->SetFloat(edgeColor.g);
			//		postEffect->GetShader()->AsScalar("colorB")->SetFloat(edgeColor.b);
			//	}
			//	case 3:	//Wiggle
			//	{
			//		ImGui::Separator();
			//
			//		static float OffsetX = 10;
			//		ImGui::InputFloat("OffsetX", &OffsetX, 0.1f);
			//
			//		static float OffsetY = 10;
			//		ImGui::InputFloat("OffsetY", &OffsetY, 0.1f);
			//		postEffect->GetShader()->AsVector("WiggleOffset")->SetFloatVector(Vector2(OffsetX, OffsetY));
			//
			//		static float AmountX = 0.01f;
			//		ImGui::InputFloat("AmountX", &AmountX, 0.001f);
			//
			//		static float AmountY = 0.01f;
			//		ImGui::InputFloat("AmountY", &AmountY, 0.001f);
			//		postEffect->GetShader()->AsVector("WiggleAmount")->SetFloatVector(Vector2(AmountX, AmountY));
			//	}
			//	case 4: //Vignette
			//	{
			//		ImGui::Separator();
			//
			//		static float Power = 2.0f;
			//		ImGui::InputFloat("Power", &Power, 0.05f);
			//		postEffect->GetShader()->AsScalar("Power")->SetFloat(Power);
			//
			//		static float ScaleX = 2.0f;
			//		ImGui::InputFloat("ScaleX", &ScaleX, 0.05f);
			//
			//		static float ScaleY = 2.0f;
			//		ImGui::InputFloat("ScaleY", &ScaleY, 0.05f);
			//		postEffect->GetShader()->AsVector("Scale")->SetFloatVector(Vector2(ScaleX, ScaleY));
			//	}
			//	case 5: //Interace
			//	{
			//		ImGui::Separator();
			//
			//		static float Strength = 1.0f;
			//		ImGui::InputFloat("Strength", &Strength, 0.01f);
			//		postEffect->GetShader()->AsScalar("Strength")->SetFloat(Strength);
			//
			//		static int InteraceValue = 2;
			//		ImGui::InputInt("InteraceValue", &InteraceValue);
			//		postEffect->GetShader()->AsScalar("InteraceValue")->SetInt(InteraceValue);
			//	}
			//	case 6:	//LensDistortion
			//	{
			//		ImGui::Separator();
			//
			//		static float LensPower = 1.0f;
			//		ImGui::InputFloat("LensPower", &LensPower, 0.01f);
			//		postEffect->GetShader()->AsScalar("LensPower")->SetFloat(LensPower);
			//
			//		static float Distortion = -0.02f;
			//		ImGui::InputFloat("Distortion", &Distortion, 0.001f);
			//
			//		static float ChromaticDistortion = -0.01f;
			//		ImGui::InputFloat("ChromaticDistortion", &ChromaticDistortion, 0.001f);
			//
			//		Vector3 temp = Vector3(Distortion, 0, 0);
			//		temp.y = Distortion + ChromaticDistortion;
			//		temp.z = Distortion + 2 * ChromaticDistortion;
			//
			//		postEffect->GetShader()->AsVector("Distortion")->SetFloatVector(temp);
			//	}
			//default:	break;
			//}
			//
				break;
			}
		default:
			break;
		}
		EditorHelper::Get()->GetActors()[cur_item]->SetIsActive(true);


		ImGui::End();
	}

}

void UI::LeftThird()
{
	switch (thirdType)
	{
	case ComponentType::Skybox:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);
		
		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadSkybox"))
		{
			//auto a = FileSystem::GetIntactFileNameFromPath(clickedPath);
			//std::wstring path = FileSystem::ToWString(a) + L"/" + FileSystem::ToWString(a);
			//EditorHelper::Get()->LoadSkycube(path);

			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
			EditorHelper::Get()->LoadSkycube(FileSystem::ToWString(clickedPath));
		}

		ImGui::End();
		break;
	}

	case ComponentType::Renderable:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadTerrain"))
		{
			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
			EditorHelper::Get()->LoadTerrain(FileSystem::ToWString(clickedPath));
		}

		if (ImGui::Button("LoadTerrainLOD"))
		{
			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
			EditorHelper::Get()->LoadTerrainLOD(FileSystem::ToWString(clickedPath));
		}
		ImGui::End();
		break;
	}

	case ComponentType::Water:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadWater"))
		{
			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
			EditorHelper::Get()->LoadWater(FileSystem::ToWString(clickedPath));
		}

		ImGui::End();
		break;
	}

	case ComponentType::Sky:
	{
		EditorHelper::Get()->LoadSky();
		showThird = false;
		break;
	}

	case ComponentType::Animator:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadModelAnim"))
		{
			auto a = FileSystem::GetIntactFileNameFromPath(clickedPath);

			std::wstring path = FileSystem::ToWString(a) + L"/" + FileSystem::ToWString(a);
			EditorHelper::Get()->LoadModel(path);

			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
		}

		ImGui::End();
		break;
	}
	case ComponentType::Light:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		if (ImGui::Button("LoadSpotLight"))
		{
			EditorHelper::Get()->LoadSpotLight();
		}
		if (ImGui::Button("LoadPointLight"))
		{
			EditorHelper::Get()->LoadPointLight();
		}
		ImGui::End();
		break;
	}
	case ComponentType::ObjectModel:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadModelObject"))
		{
			auto a = FileSystem::GetIntactFileNameFromPath(clickedPath);
			std::wstring path = FileSystem::ToWString(a) + L"/" + FileSystem::ToWString(a);
			EditorHelper::Get()->LoadMeshModel(path);

			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
		}
		ImGui::End();
		break;
	}
	case ComponentType::ObjectMesh:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		if (ImGui::Button("LoadCube"))
		{
			EditorHelper::Get()->LoadMesh(Debug_new MeshCube());
		}
		if (ImGui::Button("LoadCylinder"))
		{
			EditorHelper::Get()->LoadMesh(Debug_new MeshCylinder(0.5f, 3.0f, 20, 20));
		}
		if (ImGui::Button("LoadSphere"))
		{
			EditorHelper::Get()->LoadMesh(Debug_new MeshSphere(0.5f, 20, 20));
		}
		if (ImGui::Button("LoadPlane"))
		{
			EditorHelper::Get()->LoadMesh(Debug_new MeshPlane());
		}
		if (ImGui::Button("LoadQuad"))
		{
			EditorHelper::Get()->LoadMesh(Debug_new MeshQuad());
		}

		ImGui::End();
		break;
	}

	case ComponentType::Particle:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadParticle"))
		{
			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
			EditorHelper::Get()->LoadParticle(FileSystem::ToWString(clickedPath));
		}
		ImGui::End();
		break;
	}

	case ComponentType::Weather:
	{
		ImGui::SameLine(100);
		ImGui::Begin
		(
			"LeftThird", NULL
			, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			//ImGuiWindowFlags_NoScrollbar |
			//ImGuiWindowFlags_NoScrollWithMouse |
			//ImGuiWindowFlags_NoCollapse |
			//ImGuiWindowFlags_NoSavedSettings 
			//ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
			//ImGuiWindowFlags_NoBackground

		);

		fileDialogView->Show(&showThird, &clickedDirectory, &clickedPath);
		if (ImGui::Button("LoadWeather"))
		{
			if (clickedPath == "")
			{
				ImGui::End();
				return;
			}
			auto file = FileSystem::ToLower(FileSystem::GetFileNameFromPath(clickedPath));
			if (file.find("rain") != std::string::npos)
			{
				EditorHelper::Get()->LoadRain(FileSystem::ToWString(clickedPath));
			}
			else if (file.find("snow") != std::string::npos)
			{
				EditorHelper::Get()->LoadSnow(FileSystem::ToWString(clickedPath));
			}

		}
		ImGui::End();
		break;
	}

	default:
		break;
	}
	clickedDirectory = "";
}

void UI::RightSide()
{
	ImGui::Begin
	(
		"RIGHT", NULL,
		//ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove //|
		//ImGuiWindowFlags_NoScrollbar |
		//ImGuiWindowFlags_NoScrollWithMouse |
		//ImGuiWindowFlags_NoCollapse |
		//ImGuiWindowFlags_NoSavedSettings |
		//ImGuiWindowFlags_NoInputs |
		//ImGuiWindowFlags_NoFocusOnAppearing |
		//ImGuiWindowFlags_NoBringToFrontOnFocus |
		//ImGuiWindowFlags_NoNavFocus
	);

	static Transform* transform = NULL;
	static std::string selectModel = "";
	//int nodeClicked = -1;
	//UINT nodeClicked = std::numeric_limits<UINT>().max();

	//range based statement
	for (auto index = 0; index < EditorHelper::Get()->GetActors().size(); index++)
	{
		int selection_mask = EditorHelper::Get()->GetActors()[index]->GetSelectionMask();
		//UINT selection_mask = EditorHelper::Get()->GetActors()[index]->GetID();
		if (EditorHelper::Get()->GetActors()[index]->HasComponent<ModelAnimator>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();
			const auto parentNode = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);


			if (parentNode)
			{
				for (int i = 0; i < EditorHelper::Get()->GetActors()[index]->GetComponent<ModelAnimator>()->GetTransformsCount(); i++)
				{
					//label
					std::string label = EditorHelper::Get()->GetActors()[index]->GetName() + std::to_string(i);
					node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

					//Selected Check
					if (selection_mask == i && selection_mask != -1 && label == selectModel)
					{
						node_flags |= ImGuiTreeNodeFlags_Selected;

						transform = EditorHelper::Get()->GetActors()[index]->GetComponent<ModelAnimator>()->GetTransform(i);

						if (transform->GetRender())
						{
							Gizmo::Get()->SetTransform(transform);
							selectedActor = EditorHelper::Get()->GetActors()[index];
						}
						else
						{
							Gizmo::Get()->SetTransform(NULL);
							selectedActor = nullptr;
						}
					}

					const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, label.c_str());

					//Select
					if (ImGui::IsItemClicked())
					{
						nodeClicked = i;
						selectModel = label;
					}

					//Render Check
					bool tempb = EditorHelper::Get()->GetActors()[index]->GetComponent<ModelAnimator>()->GetTransform(i)->GetRender();
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					label = "##" + label;
					ImGui::Checkbox(label.c_str(), &tempb);
					EditorHelper::Get()->GetActors()[index]->GetComponent<ModelAnimator>()->GetTransform(i)->SetRender(tempb);
					/////////////////////////////////////////////////////


					//Add Bones Node
					if (node)
					{
						for (auto bones : EditorHelper::Get()->GetActors()[index]->GetComponent<ModelAnimator>()->GetModel()->Bones())
							//for (auto bones : EditorHelper::Get()->GetActors()[index].second->GetModel()->Bones())
						{
							if (bones->GetParentIndex() > -1 || bones->GetChilds().size() < 1)
								continue;
							AddRightNodes(bones);
						}
						ImGui::TreePop();
					}

				}
				ImGui::TreePop();
			}

		}
		else if (EditorHelper::Get()->GetActors()[index]->HasComponent<Lighting>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

																				   //Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;

				auto name = EditorHelper::Get()->GetActors()[index]->GetName();
				static std::string lightName = "";
				static int lightNum = 0;
				for (int i = 0; i < name.size(); i++)
				{
					if (std::isdigit(name[i]))
					{
						lightName = name.substr(0, i);
						lightNum = std::stoi(name.substr(i, name.size() - i));
					}
				}

				if (lightName == "SpotLight")
				{
					transform = Lighting::Get()->GetSpotLightTransform(lightNum);
				}
				else if (lightName == "PointLight")
				{
					transform = Lighting::Get()->GetPointLightTransform(lightNum);
				}


				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}

			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);
			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasComponent<CubeSky>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}

			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);
			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasComponent<Sky>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}

			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasComponent<ModelRender>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();
			const auto parentNode = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label + std::to_string(index);
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			if (parentNode)
			{
				for (int i = 0; i < EditorHelper::Get()->GetActors()[index]->GetComponent<ModelRender>()->GetTransforms().size(); i++)
				{
					//label
					std::string label = EditorHelper::Get()->GetActors()[index]->GetName() + std::to_string(i);
					node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

					//Selected Check
					if (selection_mask == i && selection_mask != -1 && label == selectModel)
					{
						node_flags |= ImGuiTreeNodeFlags_Selected;

						transform = EditorHelper::Get()->GetActors()[index]->GetComponent<ModelRender>()->GetTransform(i);

						if (transform->GetRender())
						{
							Gizmo::Get()->SetTransform(transform);
							selectedActor = EditorHelper::Get()->GetActors()[index];
						}
						else
						{
							Gizmo::Get()->SetTransform(NULL);
							selectedActor = nullptr;
						}
					}

					const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, label.c_str());

					//Select
					if (ImGui::IsItemClicked())
					{
						nodeClicked = i;
						selectModel = label;
					}

					//Render Check
					bool tempb = EditorHelper::Get()->GetActors()[index]->GetComponent<ModelRender>()->GetTransform(i)->GetRender();
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					label = "##" + label + std::to_string(index);
					ImGui::Checkbox(label.c_str(), &tempb);
					EditorHelper::Get()->GetActors()[index]->GetComponent<ModelRender>()->GetTransform(i)->SetRender(tempb);
					/////////////////////////////////////////////////////

					//Add Bones Node
					if (node)
					{
						for (auto bones : EditorHelper::Get()->GetActors()[index]->GetComponent<ModelRender>()->GetModel()->Bones())
							//for (auto bones : EditorHelper::Get()->GetActors()[index].second->GetModel()->Bones())
						{
							if (bones->GetParentIndex() > -1 || bones->GetChilds().size() < 1)
								continue;
							AddRightNodes(bones);
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasComponent<MeshRender>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}

			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasRender<TerrainLOD>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}

			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasRender<Terrain>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}
			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasRender<Water>())
		{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

		//label
		std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

		//Selected Check
		if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
			transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

			if (transform->GetRender())
			{
				Gizmo::Get()->SetTransform(transform);
				selectedActor = EditorHelper::Get()->GetActors()[index];
			}
			else
			{
				Gizmo::Get()->SetTransform(NULL);
				selectedActor = nullptr;
			}
		}

		const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

		//Select
		if (ImGui::IsItemClicked())
		{
			nodeClicked = index;
			selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
		}

		//Render Check
		//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
		bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		label = "##" + label;
		ImGui::Checkbox(label.c_str(), &tempb);
		//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
		EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

		/////////////////////////////////////////////////////
		if (node)
			ImGui::TreePop();
		}

		else if (EditorHelper::Get()->GetActors()[index]->HasRender<Particle>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}
			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasRender<Snow>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}
			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}
		else if (EditorHelper::Get()->GetActors()[index]->HasRender<Rain>())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;

			//label
			std::string label = EditorHelper::Get()->GetActors()[index]->GetName();// +std::to_string(i);

			//Selected Check
			if (selection_mask == index && EditorHelper::Get()->GetActors()[index]->GetName() == selectModel)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				transform = EditorHelper::Get()->GetActors()[index]->GetTransform();

				if (transform->GetRender())
				{
					Gizmo::Get()->SetTransform(transform);
					selectedActor = EditorHelper::Get()->GetActors()[index];
				}
				else
				{
					Gizmo::Get()->SetTransform(NULL);
					selectedActor = nullptr;
				}
			}

			const auto node = ImGui::TreeNodeEx(label.c_str(), node_flags, EditorHelper::Get()->GetActors()[index]->GetName().c_str());

			//Select
			if (ImGui::IsItemClicked())
			{
				nodeClicked = index;
				selectModel = EditorHelper::Get()->GetActors()[index]->GetName();
			}

			//Render Check
			//bool tempb = EditorHelper::Get()->GetActors()[index]->GetTransform()->GetRender();
			bool tempb = EditorHelper::Get()->GetActors()[index]->IsActive();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
			label = "##" + label;
			ImGui::Checkbox(label.c_str(), &tempb);
			//EditorHelper::Get()->GetActors()[index]->GetTransform()->SetRender(tempb);
			EditorHelper::Get()->GetActors()[index]->SetIsActive(tempb);

			/////////////////////////////////////////////////////
			if (node)
				ImGui::TreePop();
		}


		//Select cancel
		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsItemClicked())
		{
			nodeClicked = std::numeric_limits<UINT>().max();
			selection_mask = -1;
		}

		//Select
		if (nodeClicked != std::numeric_limits<UINT>().max() && (EditorHelper::Get()->GetActors()[index]->GetName() == selectModel || EditorHelper::Get()->GetActors()[index]->GetName() + std::to_string(nodeClicked) == selectModel))
		{
			selection_mask = nodeClicked;
		}
		EditorHelper::Get()->GetActors()[index]->SetSelectionMask(selection_mask);
	}



	ImGui::End();

	RightBottom();
}


void UI::RightBottom()
{
	const float DISTANCE = 10.0f;
	static int corner = 3;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		auto viewport = Context::Get()->GetViewport();
		//ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 window_pos =
			ImVec2((corner & 1) ? (viewport->GetPos().x + viewport->GetSize().x - DISTANCE) : (viewport->GetPos().x + DISTANCE),
			(corner & 2) ? (viewport->GetPos().y + viewport->GetSize().y - DISTANCE) : (viewport->GetPos().y + DISTANCE));
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		//ImGui::SetNextWindowViewport(viewport->ID);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("System Info", NULL, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		std::string str = "";

		str = std::string("FrameRate : ") + std::to_string(ImGui::GetIO().Framerate);
		ImGui::Text(str.c_str());

		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			//if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();


}

void UI::AddRightNodes(ModelBone* model)
{
	if (!model)
		return;

	auto childs = model->GetChilds();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	flags = childs.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

	//EditorHelper::Get()->
	
	const auto node = ImGui::TreeNodeEx(FileSystem::ToString(model->GetName()).c_str(), flags, FileSystem::ToString(model->GetName()).c_str());

	if (node)
	{
		for (const auto& child : childs)
		{
			AddRightNodes(child);
		}

		ImGui::TreePop();
	}



}

void UI::ShowInspector()
{
	ImGuiWindowFlags flag = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	if (showInspector && selectedActor != nullptr)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 650));
		ImGui::Begin("LeftSceond", NULL, flag);

		auto actorComponent = ComponentType::Unknown;
		auto actorRender = RenderType::Unknown;
		if (selectedActor->GetAllComponents().size() > 0)
		{
			actorComponent = selectedActor->GetAllComponents().back()->GetComponentType();
		}
		if (selectedActor->GetAllRenders().size() > 0)
		{
			actorRender = selectedActor->GetAllRenders().back()->GetRenderType();
		}

		switch (actorComponent)
		{
		case ComponentType::Unknown:			break;
		case ComponentType::Camera:				break;
		case ComponentType::FreeCamera:			break;
		case ComponentType::FallowCamera:		break;
		case ComponentType::Light:	
		{	
			LightInspector();
			break;
		}
		case ComponentType::Collider:			break;
		case ComponentType::Transform:			break;
		case ComponentType::Renderable:
		{

			break;
		}
		case ComponentType::Skybox:
		{
			ImGui::Text("SkyBoxImage");
			ImGui::Image
			(
				selectedActor->GetComponent<CubeSky>()->GetSrv(),
				ImVec2(80, 80),
				ImVec2(0, 0),
				ImVec2(1, 1),
				ImVec4(1, 1, 1, 1),
				ImColor(255, 255, 255, 128)
			);

			break;
		}
		case ComponentType::Sky:
		{

			bool b = selectedActor->GetComponent<Sky>()->GetRealTime();
			ImGui::Checkbox("RealTime", &b);
			selectedActor->GetComponent<Sky>()->SetRealTime(b);

			float theta = selectedActor->GetComponent<Sky>()->GetTheta();
			ImGui::SliderFloat("Theta", &theta, -Math::PI, Math::PI);
			selectedActor->GetComponent<Sky>()->SetTheta(theta);

			auto cloudDesc = selectedActor->GetComponent<Sky>()->GetCloudDesc();
			cloudDesc.Cover *= 100.0f;
			cloudDesc.Sharpness *= 10.0f;
			ImGui::SliderFloat("Cloud Tiles", &cloudDesc.Tiles, 0.0f, 1.0f, "%.2f", 1.0f);
			ImGui::SliderFloat("Cloud Cover", &cloudDesc.Cover, 0.01f, 1.0f, "%.3f");
			ImGui::SliderFloat("Cloud Sharpness", &cloudDesc.Sharpness, 0.0f, 10.0f, "%.2f", 1.0f);
			ImGui::SliderFloat("Cloud Speed", &cloudDesc.Speed, 0.00f, 0.1f, "%3f", 0.01f);
			cloudDesc.Sharpness /= 10.0f;
			cloudDesc.Cover /= 100.0f;
			selectedActor->GetComponent<Sky>()->SetCloudDesc(cloudDesc);

			auto cloudTransform = selectedActor->GetComponent<Sky>()->GetCloudTransform();
			auto cloudS = cloudTransform->GetScale();
			auto cloudR = cloudTransform->GetRotation();
			auto cloudT = cloudTransform->GetPosition();
			ImGui::DragFloat3("Cloud Scale", cloudS, 1.0f, 0.0f, 1000.0f);
			ImGui::DragFloat3("Cloud Rot", cloudR, 0.1f, 0.0f, Math::PI);
			ImGui::DragFloat3("Cloud Pos", cloudT, 10.0f, 0.0f, 1000.0f);
			cloudTransform->SetScale(cloudS);
			cloudTransform->SetRotation(cloudR);
			cloudTransform->SetPosition(cloudT);
			selectedActor->GetComponent<Sky>()->SetCloudTransform(cloudTransform);

			break;
		}
		case ComponentType::Animator:
		{
			auto actor = selectedActor->GetComponent<ModelAnimator>();

			//UINT instance = actor->GetTransforms().size();
			static int clip = 0;
			int clipCount = actor->GetModel()->ClipCount();
			static float speed = 1.0f;
			static float takeTime = 1.0f;
			static bool bBlendMode = false;
			static float blendAlpha = 0.0f;

			ImGui::Text("Instance : %d", nodeClicked);
			//ImGui::InputInt("Instance", (int*)&instance);
			ImGui::Checkbox("BlendMode", &bBlendMode);
			if (bBlendMode == false)
			{
				ImGui::InputInt("Clip", &clip);
				clip %= clipCount;

				ImGui::SliderFloat("Speed", &speed, 0.1f, 5.0f);
				ImGui::SliderFloat("TakeTime", &takeTime, 0.1f, 5.0f);

				if (ImGui::Button("Apply"))
				{
					actor->PlayTweenMode(nodeClicked, clip, speed, takeTime);
				}
			}
			else
			{
				ImGui::SliderFloat("Alpha", &blendAlpha, 0.0f, 2.0f);
				actor->SetBlendAlpha(nodeClicked, blendAlpha);

				if (ImGui::Button("Apply"))
				{
					//actor->PlayBlendMode(nodeClicked, 0, 1, 3);
				}
			}

			static std::string temp = "../../_Models";
			if (ImGui::Button("AddAnimation"))
			{
				showFileDialog = !showFileDialog;
				fileDialogLoad->SetFilterName(".clip");
				temp = "../../_Models";
				fileDialogLoad->SetExtraButtonName("Load Animation");
			}

			fileDialogLoad->Show(&showFileDialog, &temp, &clickedPath);
			if (showFileDialog)
			{
				ImGui::SameLine();
				if (fileDialogLoad->GetExtraButton())
				{
					actor->ReadClip(FileSystem::ToWString(clickedPath), true);
					actor->UpdateClips();
				}
			}


			break;
		}
		//TODO:
		case ComponentType::ObjectModel:
		{
			ObjectModelInspector();
			break;
		}
		case ComponentType::ObjectMesh:
		{
			ObjectMeshInspector();
			break;
		}
		default:
			break;
		}

		switch (actorRender)
		{
		case RenderType::Unknown:				break;
		case RenderType::Cloud:					break;
		case RenderType::Dome:					break;
		case RenderType::Moon:					break;
		case RenderType::Rain:
		{
			auto rainPass = selectedActor->GetRender<Rain>()->GetPass();
			ImGui::InputInt("RainPass", (int*)&rainPass);
			rainPass %= 4;
			selectedActor->GetRender<Rain>()->Pass(rainPass);

			auto desc = selectedActor->GetRender<Rain>()->GetDesc();
			ImGui::Separator();
			ImGui::SliderFloat3("Origin", desc.Origin, 0, 200);
			
			ImGui::SliderFloat3("Velocity", desc.Velocitty, -200, 200);
			ImGui::ColorEdit3("Color", desc.Color);
			ImGui::SliderFloat("Distance", &desc.DrawDistance, 0, desc.Extent.z * 2.0f);
			selectedActor->GetRender<Rain>()->SetDesc(desc);

			break;
		}
		case RenderType::Snow:	
		{
			auto snowPass = selectedActor->GetRender<Snow>()->GetPass();
			ImGui::InputInt("SnowPass", (int*)&snowPass);
			snowPass %= 4;
			selectedActor->GetRender<Snow>()->Pass(snowPass);

			auto desc = selectedActor->GetRender<Snow>()->GetDesc();

			ImGui::SliderFloat3("Velocity", desc.Velocity, -200, 200);
			ImGui::ColorEdit3("Color", desc.Color);
			ImGui::SliderFloat("Distance", &desc.DrawDistance, 0, desc.Extent.z * 2.0f);
			ImGui::InputFloat("Turbulence", &desc.Turbulence, 0.1f);

			selectedActor->GetRender<Snow>()->SetDesc(desc);

			//ImGui::Text(std::to_string(Time::Get()->Running()).c_str());
			break;
		}
		case RenderType::Terrain:
		{
			TerrainEditor();
			break;
		}
		case RenderType::TerrainLOD:
		{
			TerrainEditorLOD();
			break;
		}
		case RenderType::Water:
			
			break;
		case RenderType::Billboard:				break;
		case RenderType::PostEffect:			break;
		case RenderType::Render2D:				break;
		case RenderType::Particle:
		{
			ParticleEditor();
			break;
		}
		}

		ImGui::End();
	}


}

void UI::TerrainEditor()
{
	auto terrain = selectedActor->GetRender<Terrain>();
	auto imageFile = terrain->GetImageFile();
	static std::string temp = "../../_Assets/Terrain";
	static int selectedChannel = 0;

	imageFile = L"HeightMap File : " + imageFile;
	ImGui::Text(FileSystem::ToString(imageFile).c_str());

	if (terrain->GetHeightMap() != nullptr)
	{
		ImGui::Image(terrain->GetHeightMap()->SRV(), ImVec2(128, 128));
	}

	const char* items[] = { "Red", "Green", "Blue", "Alpha" };
	ImGui::Combo("Channel", &selectedChannel, items, 4);

	ImGui::Separator();

	if (ImGui::Button("Save to File"))
	{
		terrain->SaveHeightMap();
	}
	if (ImGui::Button("Load from File"))
	{
		FileSystem::OpenFileDialog([=](std::string path)
		{
			terrain->SetTerrainMap(FileSystem::ToWString(path));
		});
	}

	auto bd = terrain->brush->GetBrushDesc();
	auto ld = terrain->brush->GetLineDesc();

	ImGui::Text("");
	ImGui::Separator();
	ImGui::Text("Guid Line");
	ImGui::Separator();

	static bool bVisible = true;
	ImGui::Checkbox("LineVisible", &bVisible);
	ld.Visible = bVisible ? 1 : 0;

	ImGui::ColorEdit3("LineColor", ld.Color);
	ImGui::InputFloat("Thickness", &ld.Thickness, 0.01f);
	ImGui::InputFloat("LineSize", &ld.Size, 1.0f);

	ImGui::Separator();

	ImGui::ColorEdit3("BrushColor", bd.Color);
	ImGui::InputInt("BrushType", (int*)&bd.Type);
	bd.Type %= 3;

	ImGui::InputInt("BrushRange", (int*)&bd.Range);
	bd.Range = Math::Clamp<UINT>(bd.Range, 1, 10);
	

	terrain->brush->SetBrushDesc(bd);
	terrain->brush->SetLineDesc(ld);

}

void UI::TerrainEditorLOD()
{
	auto terrain = selectedActor->GetRender<TerrainLOD>();
	auto imageFile = terrain->GetImageFile();
	
	static bool bWireframe = false;
	ImGui::Checkbox("Wireframe", &bWireframe);
	
	//terrain->Pass(bWireframe ? 1 : 0);


	//static std::string temp = "../../_Assets/Terrain";
	//static int selectedChannel = 0;
	//
	imageFile = L"HeightMap File : " + imageFile;
	ImGui::Text(FileSystem::ToString(imageFile).c_str());
	//
	if (terrain->GetHeightMap() != nullptr)
	{
		ImGui::Image(terrain->GetHeightMap(), ImVec2(128, 128));
	}
	//
	//const char* items[] = { "Red", "Green", "Blue", "Alpha" };
	//ImGui::Combo("Channel", &selectedChannel, items, 4);
	//
	//ImGui::Separator();
	//
	//if (ImGui::Button("Save to File"))
	//{
	//	terrain->SaveHeightMap();
	//}
	//if (ImGui::Button("Load from File"))
	//{
	//	FileSystem::OpenFileDialog([=](std::string path)
	//	{
	//		terrain->SetTerrainMap(FileSystem::ToWString(path));
	//	});
	//}
	//
	//auto bd = terrain->brush->GetBrushDesc();
	//auto ld = terrain->brush->GetLineDesc();
	//
	//ImGui::Text("");
	//ImGui::Separator();
	//ImGui::Text("Guid Line");
	//ImGui::Separator();
	//
	//static bool bVisible = true;
	//ImGui::Checkbox("LineVisible", &bVisible);
	//ld.Visible = bVisible ? 1 : 0;
	//
	//ImGui::ColorEdit3("LineColor", ld.Color);
	//ImGui::InputFloat("Thickness", &ld.Thickness, 0.01f);
	//ImGui::InputFloat("LineSize", &ld.Size, 1.0f);
	//
	//ImGui::Separator();
	//
	//ImGui::ColorEdit3("BrushColor", bd.Color);
	//ImGui::InputInt("BrushType", (int*)&bd.Type);
	//bd.Type %= 3;
	//
	//ImGui::InputInt("BrushRange", (int*)&bd.Range);
	//bd.Range = Math::Clamp<UINT>(bd.Range, 1, 10);
	//
	//
	//terrain->brush->SetBrushDesc(bd);
	//terrain->brush->SetLineDesc(ld);
}

void UI::ParticleEditor()
{
	Particle* particle = nullptr;
	particle = selectedActor->GetRender<Particle>();
	if (particle == nullptr) return;


	static int maxParticle = 1;
	static bool bLoop = true;

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
		
		////////////////////////Wrtie
		ImGui::Separator();

		if (ImGui::Button("WriteParticle"))
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
			textureFile = FileSystem::GetFileNameFromPath(textureFile);

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

			FileSystem::SaveFileDialog([=](std::string path)
			{
				std::string out;
				out = path;
				out += ".xml";

				document->SaveFile(out.c_str());

			});
			SAFE_DELETE(document);
		}
		////////////////////
	}


	ImGui::Spacing();
	ImGui::Separator();
	auto textureList = (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_None));
	//if (textureList)
	//{
		static auto direct = std::string("../../_Assets/Particles");
		static auto textureFile = std::string("");
		inspecFileDialogView->SetFilterName(".png");
		inspecFileDialogView->Show(&textureList, &direct, &textureFile);
		

		if (ImGui::Button("Change Texture"))
		{
			particle->GetData().TextureFile = FileSystem::ToWString(textureFile);
			particle->SetTexture(FileSystem::ToWString(textureFile), false);
		}
	//}

}

void UI::ObjectMeshInspector()
{
	auto mesh = selectedActor->GetComponent<MeshRender>();
	auto material = mesh->GetMaterial();

	ImGui::Text("Diffuse Map");
	ImGui::Image(material->DiffuseMap()->SRV(), ImVec2(64, 64));
	ImGui::Separator();
	ImGui::Text("Specular Map");
	ImGui::Image(material->SpecularMap()->SRV(), ImVec2(64, 64));
	ImGui::Separator();
	ImGui::Text("Normal Map");
	ImGui::Image(material->NormalMap()->SRV(), ImVec2(64, 64));


	auto textureList = (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_None));
	static auto direct = std::string("../../_Textures/Material");
	static auto textureFile = std::string("");
	inspecFileDialogView->SetFilterName("!_");
	inspecFileDialogView->Show(&textureList, &direct, &textureFile);
	
	if (ImGui::Button("Change Texture"))
	{
		mesh->SetMaterial(textureFile);
	}

}

void UI::LightInspector()
{
	auto light = selectedActor->GetComponent<Lighting>();
	
	auto isSpot = selectedActor->GetName().find("Spot") != std::string::npos ? true : false ;

	//int index = atoi(selectedActor->GetName().c_str());
	//int index = stoi()
	
	auto name = selectedActor->GetName();
	static std::string lightName = "";
	static int index= 0;
	for (int i = 0; i < name.size(); i++)
	{
		if (std::isdigit(name[i]))
		{
			lightName = name.substr(0, i);
			index = std::stoi(name.substr(i, name.size() - i));
		}
	}


	if (isSpot)
	{
		SpotLight& spotLight = Lighting::Get()->GetSpotLight(index);

		ImGui::ColorEdit3("SpotLight Ambient", spotLight.Ambient);
		ImGui::ColorEdit3("SpotLight Diffuse", spotLight.Diffuse);
		ImGui::ColorEdit3("SpotLight Specular", spotLight.Specular);
		ImGui::ColorEdit3("SpotLight Emissive", spotLight.Emissive);

		ImGui::SliderFloat3("SpotLight Direciton", spotLight.Direction, -1, 1);
		ImGui::SliderFloat("SpotLight Range", &spotLight.Range, 1, 50);
		ImGui::SliderFloat("SpotLight Angle", &spotLight.Angle, 1, 90);

		ImGui::SliderFloat("SpotLight Intensity", &spotLight.Intensity, 0, 1);
	}
	else
	{
		PointLight& pointLight = Lighting::Get()->GetPointLight(index);

		ImGui::ColorEdit3("PointLight Ambient", pointLight.Ambient);
		ImGui::ColorEdit3("PointLight Diffuse", pointLight.Diffuse);
		ImGui::ColorEdit3("PointLight Specular", pointLight.Specular);
		ImGui::ColorEdit3("PointLight Emissive", pointLight.Emissive);

		ImGui::SliderFloat("PointLight Range", &pointLight.Range, 0, 20);
		ImGui::SliderFloat("PointLight Intensity", &pointLight.Intensity, 0, 1);
	}





	

}

void UI::ObjectModelInspector()
{
	auto mesh = selectedActor->GetComponent<ModelRender>();
	auto materials = mesh->GetModel()->Materials();
	
	for (auto mat : materials)
	{
		if (mat->DiffuseMap() != nullptr)
		{
			std::string temp = " " + FileSystem::ToString(mat->Name()) + "Diffuse Map";
			ImGui::Text(temp.c_str());
			ImGui::Image(mat->DiffuseMap()->SRV(), ImVec2(64, 64));
			ImGui::Separator();
		}

		if (mat->SpecularMap() != nullptr)
		{
			std::string temp = " " + FileSystem::ToString(mat->Name()) + "Specular Map";
			ImGui::Text(temp.c_str());
			ImGui::Image(mat->SpecularMap()->SRV(), ImVec2(64, 64));
			ImGui::Separator();
		}

		if (mat->NormalMap() != nullptr)
		{
			std::string temp = " " + FileSystem::ToString(mat->Name()) + "Normal Map";
			ImGui::Text(temp.c_str());
			ImGui::Image(mat->NormalMap()->SRV(), ImVec2(64, 64));
			ImGui::Separator();
		}




	}
	/*ImGui::Text("Diffuse Map");
	ImGui::Image(material->DiffuseMap()->SRV(), ImVec2(64, 64));
	ImGui::Separator();
	ImGui::Text("Specular Map");
	ImGui::Image(material->SpecularMap()->SRV(), ImVec2(64, 64));
	ImGui::Separator();
	ImGui::Text("Normal Map");
	ImGui::Image(material->NormalMap()->SRV(), ImVec2(64, 64));


	auto textureList = (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_None));
	static auto direct = std::string("../../_Textures/Material");
	static auto textureFile = std::string("");
	inspecFileDialogView->SetFilterName("!_");
	inspecFileDialogView->Show(&textureList, &direct, &textureFile);

	if (ImGui::Button("Change Texture"))
	{
		mesh->SetMaterial(textureFile);
	}
*/
}





//bool ret = false;
//ImGui::BulletText(
//	"Return value = %d\n"
//	"IsItemFocused() = %d\n"
//	"IsItemHovered() = %d\n"
//	"IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
//	"IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
//	"IsItemHovered(_AllowWhenOverlapped) = %d\n"
//	"IsItemHovered(_RectOnly) = %d\n"
//	"IsItemActive() = %d\n"
//	"IsItemEdited() = %d\n"
//	"IsItemActivated() = %d\n"
//	"IsItemDeactivated() = %d\n"
//	"IsItemDeactivatedAfterEdit() = %d\n"
//	"IsItemVisible() = %d\n"
//	"IsItemClicked() = %d\n"
//	"GetItemRectMin() = (%.1f, %.1f)\n"
//	"GetItemRectMax() = (%.1f, %.1f)\n"
//	"GetItemRectSize() = (%.1f, %.1f)",
//	ret,
//	ImGui::IsItemFocused(),
//	ImGui::IsItemHovered(),
//	ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup),
//	ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem),
//	ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped),
//	ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly),
//	ImGui::IsItemActive(),
//	ImGui::IsItemEdited(),
//	ImGui::IsItemActivated(),
//	ImGui::IsItemDeactivated(),
//	ImGui::IsItemDeactivatedAfterEdit(),
//	ImGui::IsItemVisible(),
//	ImGui::IsItemClicked(),
//	ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y,
//	ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y,
//	ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y
//);
