#include "stdafx.h"
#include "PlayerDemo.h"

void PlayerDemo::Init()
{
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(1, 36, -85);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(50, 2);

	shader = Debug_new Shader(L"48_EnvCube.fxo");
	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);

	Mesh();
	Paladin();
	PostEffectInit();


}

void PlayerDemo::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(sky);

	SAFE_DELETE(floor);

	SAFE_DELETE(plane);

	animators.clear();
	meshes.clear();
	models.clear();
	
	SAFE_DELETE(dummyTransform);


	//SAFE_DELETE(weapon);
	//SAFE_DELETE(weaponTransform);

	SAFE_DELETE(paladin);

	//postEffect
	SAFE_DELETE(renderTarget);
	SAFE_DELETE(depthStencil);
	SAFE_DELETE(viewport);
	SAFE_DELETE(postEffect);


}

void PlayerDemo::Update()
{
	static UINT Pass = postEffect->GetShader()->PassCount() - 1;
	ImGui::InputInt("Blur Pass", (int *)&Pass);
	Pass %= postEffect->GetShader()->PassCount();
	postEffect->Pass(Pass);

	sky->Update();

	plane->Update();

	paladin->Update();
	PaladinMove();


	postEffect->Update();
}

void PlayerDemo::PreRender()
{
	viewport->RSSetViewport();
	{
		renderTarget->PreRender(depthStencil);

		sky->Pass(4);
		sky->Render();

		Pass(5);
		floor->Render();
		plane->Render();

		paladin->Render();
	}
}

void PlayerDemo::Render()
{
	//Main Render Target
	sky->Pass(4);
	sky->Render();

	Pass(5);
	floor->Render();
	plane->Render();

	paladin->Render();

	if (_KEYCODE->Press('O'))
	{
		postEffect->Render();
	}
}

void PlayerDemo::Mesh()
{
	{//Create Material
		floor = Debug_new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->NormalMap("Floor_Normal.png");
		floor->Specular(1, 1, 1, 20);
		floor->SpecularMap("Floor_Specular.png");
		floor->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}

	{//Create Mesh
		Transform* transform = NULL;
		plane = Debug_new MeshRender(shader, Debug_new MeshPlane(2.5f, 2.5f));
		transform = plane->AddTransform();
		transform->SetScale(12, 1, 12);
	}

	plane->UpdateTransforms();
	meshes.push_back(plane);
	
}

void PlayerDemo::Paladin()
{
	paladin = Debug_new ModelAnimator(shader);
	paladin->ReadMesh(L"Paladin/Paladin");
	paladin->ReadMaterial(L"Paladin/Paladin");

	paladin->ReadClip(L"Paladin/BlockIdle");
	paladin->ReadClip(L"Paladin/Idle");
	paladin->ReadClip(L"Paladin/Rolling");
	paladin->ReadClip(L"Paladin/Run");
	paladin->ReadClip(L"Paladin/Walk");

	dummyTransform = Debug_new Transform();
	dummyTransform->SetPosition(0, 0, -30);
	dummyTransform->SetScale(0.075f);

	Transform* transform = NULL;

	transform = paladin->AddTransform();
	transform->SetPosition(0, 0, -30);
	transform->SetScale(0.075f, 0.075f, 0.075f);
	paladin->PlayTweenMode(0, 0, 1.0f);

	paladin->UpdateTransforms();

	animators.push_back(paladin);
}

void PlayerDemo::Pass(UINT _val)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(_val);

	_val++;
	for (ModelRender* model : models)
		model->Pass(_val);

	_val++;
	for (ModelAnimator* animation : animators)
		animation->Pass(_val);
}

void PlayerDemo::PaladinMove()
{
	_KEYCODE->Toggle(VK_TAB) ? Context::Get()->SetCameraIndex(1) : Context::Get()->SetCameraIndex(0);
	((FallowCamera*)Context::Get()->GetCamera(1))->GetTransform()->SetParent(dummyTransform);
	((FallowCamera*)Context::Get()->GetCamera(1))->SetTarget(dummyTransform->GetPosition(), dummyTransform->GetForward());
	

	static int clip = 0;
	static float speed = 1.0f;
	static float takeTime = 1.0f;

	static bool bBlendMode = false;
	static float blendAlpha = 0.0f;

	Vector3 pos = paladin->GetTransform(0)->GetPosition();

	Vector3 f = dummyTransform->GetForward();
	Vector3 u = paladin->GetTransform(0)->GetUp();
	Vector3 r = dummyTransform->GetRight();

	if (Context::Get()->GetCameraIndex() == 1)
	{
		static float moveSpeed = 0.0f;

		if(!_KEYCODE->Press(VK_LMENU))
		{//Camera Rotation
			static float cameraSpeed = 5.0f;

			Vector3 rot;
			dummyTransform->GetRotationDegree(&rot);

			Vector3 val = Mouse::Get()->GetMoveValue();
			rot.x = Math::Clamp(rot.x - val.y * cameraSpeed * Time::Delta(), -50.0f, 50.0f);
			rot.y += val.x * cameraSpeed * Time::Delta();
			dummyTransform->SetRotationDegree(rot);
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Move
		{
			if (_KEYCODE->Press('W') || _KEYCODE->Press('A') || _KEYCODE->Press('S') || _KEYCODE->Press('D'))
			{
				moveSpeed += 10.0f;
			}
			else
				moveSpeed -= 30.0f;
			
			moveSpeed = Math::Clamp(moveSpeed, 0.0f, _KEYCODE->Press(VK_SHIFT) ? 150.0f : 100.0f);

			if (moveSpeed < 10.0f)
				modelState = PaladinAnim::Idle;
			else if (moveSpeed > 10.0f && moveSpeed <= 100.0f)
				modelState = PaladinAnim::Walk;
			else if(moveSpeed > 100.0f)
				modelState = PaladinAnim::Run;

			modelNextState = modelState;
			
			static float changeTime = 1.0f, changeSpeed = 1.0f;


			if (modelNextState != modelCurrState)
			{
				modelCurrState = modelNextState;
				paladin->PlayTweenMode(0, (UINT)modelCurrState, changeSpeed, changeTime);
			}

			Vector3 playerRot;
			paladin->GetTransform(0)->GetRotationDegree(&playerRot);

			Vector3 PxC;
			D3DXVec3Cross(&PxC, &paladin->GetTransform(0)->GetForward(), &dummyTransform->GetForward());

			float playerToCamAngle = D3DXToDegree(
				Math::GetVec2Angle(Vector2(paladin->GetTransform(0)->GetForward().x, paladin->GetTransform(0)->GetForward().z), 
					Vector2(dummyTransform->GetForward().x, dummyTransform->GetForward().z)
				)
			);


			if (_KEYCODE->Press('W'))
			{
				pos -= Vector3(f.x * moveSpeed * Time::Delta(), 0, f.z * moveSpeed * Time::Delta());
				if (!(fabs(PxC.y * 1000.0f) < 1.0f && playerToCamAngle < 10.0f))// return; // 團馬紫(っ薑彰嬪)
				{
					if (PxC.y > 0.0f)
					{//謝難
						playerRot.y += 120.0f * Time::Delta();
					}
					else if (PxC.y < 0.0f)
					{//辦難
						playerRot.y -= 120.0f * Time::Delta();
					}
				}
			}

			if (_KEYCODE->Press('S'))
			{
				pos += Vector3(f.x * moveSpeed * Time::Delta(), 0, f.z * moveSpeed * Time::Delta());
				if (!(fabs(PxC.y * 1000.0f) < 1.0f && playerToCamAngle > 170.0f)) // 團馬紫(っ薑彰嬪)
				{
					if (PxC.y > 0.0f)
					{//謝難
						playerRot.y -= 120.0f * Time::Delta();
					}
					else if (PxC.y < 0.0f)
					{//辦難
						playerRot.y += 120.0f * Time::Delta();
					}
				}
			}

			if (_KEYCODE->Press('D'))
			{
				pos -= r * moveSpeed * Time::Delta();
				if (!((int)(PxC.y * 1000.0f) == -5)) // 團馬紫(っ薑彰嬪)&& (playerToCamAngle > 100.0f || playerToCamAngle < 10.0f)
				{
					if (PxC.y > 0.0f)
					{
						if (playerToCamAngle < 90.0f)
							playerRot.y += 120.0f * Time::Delta();
						else if (playerToCamAngle > 90.0f)
							playerRot.y -= 120.0f * Time::Delta();
					}
					else if (PxC.y < 0.0f)
					{
						if (playerToCamAngle < 90.0f)
							playerRot.y += 120.0f * Time::Delta();
						else if (playerToCamAngle > 90.0f)
							playerRot.y -= 120.0f * Time::Delta();
					}
				}
			}

			if (_KEYCODE->Press('A'))
			{
				pos += r * moveSpeed * Time::Delta();
				if (!((int)(PxC.y * 1000.0f) == 5)) // 團馬紫(っ薑彰嬪)&& playerToCamAngle > 100.0f
				{
					if (PxC.y > 0.0f)
					{
						if (playerToCamAngle < 90.0f)
							playerRot.y -= 120.0f * Time::Delta();
						else if (playerToCamAngle > 90.0f)
							playerRot.y += 120.0f * Time::Delta();
					}
					else if (PxC.y < 0.0f)
					{
						if (playerToCamAngle < 90.0f)
							playerRot.y -= 120.0f * Time::Delta();
						else if (playerToCamAngle > 90.0f)
							playerRot.y += 120.0f * Time::Delta();
					}
				}
			}

			//paladin->GetTransform(0)->SetPosition(pos);
			dummyTransform->SetPosition(pos);
			paladin->GetTransform(0)->SetRotationDegree(playerRot);
			paladin->UpdateTransforms();
		}
	}
}

void PlayerDemo::PostEffectInit()
{
	float width = D3D::Width(), height = D3D::Height();

	renderTarget = Debug_new RenderTarget(width, height);

	depthStencil = Debug_new DepthStencil(width, height);
	viewport = Debug_new Viewport(width, height);

	postEffect = Debug_new PostEffect(L"46_Blur.fxo");
	postEffect->SRV(renderTarget->GetSRV());
}
