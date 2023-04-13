#include "stdafx.h"
#include "AnimationDemo.h"
#include "Converter.h"

void AnimationDemo::Init()
{
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->SetRotationDegree(12, 0, 0);
	Context::Get()->GetCamera(Context::Get()->GetCameraIndex())->GetTransform()->SetPosition(2, 4, -12);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(10, 2);

	shader = new Shader(L"32_Instance.fxo");

	Kachujin();
	Paladin();
}



void AnimationDemo::Update()
{
	static UINT instance = 20;
	static int clip = 0;
	static float speed = 1.0f;
	static float takeTime = 1.0f;

	static bool bBlendMode = false;
	static float blendAlpha = 0.0f;

	ImGui::InputInt("Instance", (int*)&instance);
	ImGui::Checkbox("BlendMode", &bBlendMode);
	if (bBlendMode == false)
	{
		ImGui::InputInt("Clip", &clip);
		clip %= 5;

		ImGui::SliderFloat("Speed", &speed, 0.1f, 5.0f);
		ImGui::SliderFloat("TakeTime", &takeTime, 0.1f, 5.0f);

		if (ImGui::Button("Apply"))
		{
			kachujin->PlayTweenMode(instance, clip, speed, takeTime);
			paladin->PlayTweenMode(instance, clip, speed, takeTime);
		}
	}
	else
	{
		ImGui::SliderFloat("Alpha", &blendAlpha, 0.0f, 2.0f);
		kachujin->SetBlendAlpha(instance, blendAlpha);
		paladin->SetBlendAlpha(instance, blendAlpha);

		if (ImGui::Button("Apply"))
		{
			kachujin->PlayBlendMode(instance, 0, 1, 3);
			paladin->PlayBlendMode(instance, 0, 1, 3);
		}
	}
	static float test = 0;
	test += Time::Delta();
	if (kachujin != NULL)
	{
		Matrix bones[MAX_MODEL_TRANSFORMS];

		UINT count = kachujin->GetTransformsCount();
		for (UINT i = 0; i < count; i++)
		{
			kachujin->GetAttachBones(i, bones);

			Transform* weaponTransform = weapon->GetTransform(i);
			weaponTransform->SetWorld(weaponInitTransform->GetWorld() * bones[40]);

			colliderObject[i]->World->SetWorld(bones[40]);
			colliderObject[i]->Collision->Update();
		}

		if (test > 3.0f)
		{
			for (UINT i = 0; i < count; i++)
			{
				kachujin->SetColor(i, Math::RandomColor4());
			}
			test = 0;
		}
		weapon->UpdateTransforms();

		kachujin->Update();
		kachujin->UpdateTransforms();
		weapon->Update();
	}

	if (paladin != NULL)
	{
		paladin->Update();
		paladin->UpdateTransforms();
	}
}

void AnimationDemo::Render()
{

	if (kachujin != NULL)
	{
		UINT count = kachujin->GetTransformsCount();

		for (UINT i = 0; i < count; i++)
			colliderObject[i]->Collision->Render();

		kachujin->Pass(2);
		kachujin->Render();

		weapon->Pass(1);
		weapon->Render();
	}

	if (paladin != NULL)
	{
		paladin->Pass(2);
		paladin->Render();
	}

}

void AnimationDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Kachujin"); 
	kachujin->ReadMaterial(L"Kachujin/Kachujin"); 
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walk");
	kachujin->ReadClip(L"Kachujin/Run");
	kachujin->ReadClip(L"Kachujin/Slash");
	kachujin->ReadClip(L"Kachujin/Uprock");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = kachujin->AddTransform();
		transform->SetPosition(x, 0, -5);
		transform->SetScale(0.01f, 0.01f, 0.01f);
	}
	kachujin->UpdateTransforms();


	UINT count = kachujin->GetTransformsCount();

	//Weapon
	{
		weapon = new ModelRender(shader);
		weapon->ReadMesh(L"Weapon/Sword");
		weapon->ReadMaterial(L"Weapon/Sword");

		for (UINT i = 0; i < count; i++)
			weapon->AddTransform();


		weaponInitTransform = new Transform();
		weaponInitTransform->SetPosition(-2.9f, 1.45f, -6.45f);
		weaponInitTransform->SetScale(0.5f, 0.5f, 0.75f);
		weaponInitTransform->SetRotation(0, 0, 1);
	}

	//Collider
	{
		colliderObject = new ColliderObject*[count];
		for (UINT i = 0; i < count; i++)
		{
			colliderObject[i] = new ColliderObject();
			colliderObject[i]->Init->SetPosition(-2.9f, 1.45f, -50.0f);
			colliderObject[i]->Init->SetScale(5, 5, 75);
		}
	}
	
}

void AnimationDemo::Paladin()
{
	paladin = new ModelAnimator(shader);
	paladin->ReadMesh(L"Paladin/Paladin");
	paladin->ReadMaterial(L"Paladin/Paladin");

	paladin->ReadClip(L"Paladin/BlockIdle");
	paladin->ReadClip(L"Paladin/Idle");
	paladin->ReadClip(L"Paladin/Rolling");
	paladin->ReadClip(L"Paladin/Run");
	paladin->ReadClip(L"Paladin/Walk");


	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = paladin->AddTransform();
		transform->SetPosition(x, 0, -10);
		transform->SetScale(0.01f, 0.01f, 0.01f);
	}
	paladin->UpdateTransforms();


//	UINT count = paladin->GetTransformsCount();

}
