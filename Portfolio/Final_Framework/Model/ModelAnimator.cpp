#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * _shader)
	: shader(_shader)
	, IComponent(nullptr, nullptr)
{
	model = Debug_new Model();
	transforms.clear();
	
	instanceBuffer = Debug_new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = Debug_new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

	//frameBuffer = Debug_new ConstantBuffer(&keyframeDesc, sizeof(KeyframeDesc));
	frameBuffer = Debug_new ConstantBuffer(&tweenDesc, sizeof(TweenDesc) * MAX_MODEL_INSTANCE);
	blendBuffer = Debug_new ConstantBuffer(&blendDesc, sizeof(BlendDesc) * MAX_MODEL_INSTANCE);

	sTransformSRV = _shader->AsSRV("TransformsMap");
	sFrameBuffer = _shader->AsConstantBuffer("CB_AnimationFrame");
	sBlendBuffer = _shader->AsConstantBuffer("CB_BlendingFrame");

	//Create Compute Shader
	{
		computeShader = Debug_new Shader(L"34_GetBones.fxo");
		
		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendingFrame");

		sComputeTransformsSRV = computeShader->AsSRV("TransformsMap");

		computeBoneBuffer = Debug_new StructuredBuffer(NULL, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones");

		computeWorldBuffer = Debug_new StructuredBuffer(NULL, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputWorldBuffer = computeShader->AsSRV("InputWorlds");

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));

		computeOutputBuffer = Debug_new TextureBuffer(texture);
		sComputeOutputBuffer = computeShader->AsUAV("Output");

		SAFE_RELEASE(texture);

		attachBones = Debug_new Matrix*[MAX_MODEL_INSTANCE];
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
			attachBones[i] = Debug_new Matrix[MAX_MODEL_TRANSFORMS];


	}
}

ModelAnimator::ModelAnimator(Actor * _actor, Transform * _transform)
	: IComponent(_actor, _transform)
	, shader(nullptr)
{
	model = Debug_new Model();
	transforms.clear();

	instanceBuffer = Debug_new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = Debug_new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

	//frameBuffer = Debug_new ConstantBuffer(&keyframeDesc, sizeof(KeyframeDesc));
	frameBuffer = Debug_new ConstantBuffer(&tweenDesc, sizeof(TweenDesc) * MAX_MODEL_INSTANCE);
	blendBuffer = Debug_new ConstantBuffer(&blendDesc, sizeof(BlendDesc) * MAX_MODEL_INSTANCE);

}

ModelAnimator::~ModelAnimator()
{
	SAFE_DELETE(model);

	for (Transform* transform : transforms)
		SAFE_DELETE(transform);

	SAFE_DELETE(instanceBuffer);
	SAFE_DELETE(instanceColorBuffer);

	SAFE_RELEASE(transformsSRV);

	SAFE_DELETE(frameBuffer);
	SAFE_DELETE(blendBuffer);

	SAFE_DELETE(computeShader);
	SAFE_DELETE(computeWorldBuffer);
	SAFE_DELETE(computeOutputBuffer);
	SAFE_DELETE(computeBoneBuffer);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		SAFE_DELETE_ARRAY(attachBones[i]);

	SAFE_DELETE_ARRAY(attachBones);

	SAFE_DELETE_ARRAY(clipTransforms);

}

void ModelAnimator::Update()
{
	if (texture == NULL)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS]; //원본 Bone값
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->GetTransform();

		computeBoneBuffer->CopyToInput(bones);
	}

	for (UINT i = 0; i < transforms.size(); i++)
	{
		if (!transforms[i]->GetRender())
		{
			//UpdateTransforms();
			continue;
		}
		
		if (blendDesc[i].Mode == 0)
			UpdateAnimationFrame(i);
		else
			UpdateBlendingFrame(i);
	}

	UpdateTransforms();

	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{

		frameBuffer->Render();
		blendBuffer->Render();


		sComputeFrameBuffer->SetConstantBuffer(frameBuffer->GetBuffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->GetBuffer());
		sComputeTransformsSRV->SetResource(transformsSRV);

		sComputeInputBoneBuffer->SetResource(computeBoneBuffer->GetSRV());
		sComputeInputWorldBuffer->SetResource(computeWorldBuffer->GetSRV());

		sComputeOutputBuffer->SetUnorderedAccessView(computeOutputBuffer->GetUAV());

		computeShader->Dispatch(0, 0, 1, MAX_MODEL_INSTANCE, 1);

		texture = computeOutputBuffer->CopyFromOutput();
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, 0, &subResource);
		{
			for (UINT i = 0; i < transforms.size(); i++)
			{
				BYTE* start = (BYTE*)subResource.pData + (i * subResource.RowPitch);//읽을 지점
		
				memcpy(attachBones[i], start, sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
			}
		
		}
		D3D::GetDC()->Unmap(texture, 0);
	}
	frameTime = fmod(frameTime, (1.0f / frameRate));



	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();

}

void ModelAnimator::UpdateAnimationFrame(UINT _instance)
{
	TweenDesc& desc = tweenDesc[_instance];
	ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

	desc.Curr.RunningTime += Time::Delta();

	float time = 1.0f / clip->GetFrameRate() / desc.Curr.Speed;

	if (desc.Curr.Time >= 1.0f)
	{
		desc.Curr.RunningTime = 0;
		desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->GetFrameCount();
		desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->GetFrameCount();
	}

	desc.Curr.Time = desc.Curr.RunningTime / time;

	if (desc.Next.Clip > -1) // Next가 존재
	{
		ModelClip* nextClip = model->ClipByIndex(desc.Next.Clip);
		
		desc.ChangeTime += Time::Delta();
		desc.TweenTime = desc.ChangeTime / desc.TakeTime;

		if (desc.TweenTime >= 1.0f) // 1이후(동작전환)
		{
			desc.Curr = desc.Next;

			desc.Next.Clip = -1;
			desc.Next.CurrFrame = 0;
			desc.Next.NextFrame = 0;
			desc.Next.Time = 0;
			desc.Next.RunningTime = 0.0f;

			desc.ChangeTime = 0.0f;
			desc.TweenTime = 0.0f;
		}
		else // 1이전(동시플레이)
		{
			desc.Next.RunningTime += Time::Delta();

			float time = 1.0f / nextClip->GetFrameRate() / desc.Next.Speed;

			if (desc.Next.Time >= 1.0f)
			{
				desc.Next.RunningTime = 0;
				desc.Next.CurrFrame = (desc.Next.CurrFrame + 1) % nextClip->GetFrameCount();
				desc.Next.NextFrame = (desc.Next.CurrFrame + 1) % nextClip->GetFrameCount();
			}

			desc.Next.Time = desc.Next.RunningTime / time;
		}
	}

}

void ModelAnimator::UpdateBlendingFrame(UINT _instance)
{
	BlendDesc& desc = blendDesc[_instance];

	for (UINT i = 0; i < 3; i++)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clip[i].Clip);

		desc.Clip[i].RunningTime += Time::Delta();

		float time = 1.0f / clip->GetFrameRate() / desc.Clip[i].Speed;

		if (desc.Clip[i].Time >= 1.0f)
		{
			desc.Clip[i].RunningTime = 0;
			desc.Clip[i].CurrFrame = (desc.Clip[i].CurrFrame + 1) % clip->GetFrameCount();
			desc.Clip[i].NextFrame = (desc.Clip[i].CurrFrame + 1) % clip->GetFrameCount();
		}

		desc.Clip[i].Time = desc.Clip[i].RunningTime / time;
	}

}

void ModelAnimator::Render()
{
	
	sFrameBuffer->SetConstantBuffer(frameBuffer->GetBuffer());
	sBlendBuffer->SetConstantBuffer(blendBuffer->GetBuffer());

	sTransformSRV->SetResource(transformsSRV);

	instanceBuffer->Render();
	instanceColorBuffer->Render();

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Render(transforms.size());
	}
	

}

void ModelAnimator::ReadMaterial(std::wstring _file)
{
	//_file = L"../../_Textures/" + _file + L".material";
	model->ReadMaterial(_file);
}

void ModelAnimator::ReadMesh(std::wstring _file)
{
	//_file = L"../../_Models/" + _file + L".mesh";
	model->ReadMesh(_file);
}

void ModelAnimator::ReadClip(std::wstring _file, bool isPath)
{
	model->ReadClip(_file, isPath);
}

void ModelAnimator::PlayTweenMode(UINT _instance, UINT _clip, float _speed, float _takeTime)
{
	blendDesc[_instance].Mode = 0;

	tweenDesc[_instance].TakeTime = _takeTime;
	tweenDesc[_instance].Next.Clip = _clip;
	tweenDesc[_instance].Next.Speed = _speed;
}

void ModelAnimator::PlayBlendMode(UINT _instance, UINT _clip0, UINT _clip1, UINT _clip2)
{
	blendDesc[_instance].Mode = 1;

	blendDesc[_instance].Clip[0].Clip = _clip0;
	blendDesc[_instance].Clip[1].Clip = _clip1;
	blendDesc[_instance].Clip[2].Clip = _clip2;
}

void ModelAnimator::SetBlendAlpha(UINT _instance, float _alpha)
{
	_alpha = Math::Clamp(_alpha, 0.0f, 2.0f);
	
	blendDesc[_instance].Alpha = _alpha;
}

void ModelAnimator::SetShader(Shader * _shader)
{
	shader = _shader;
	sTransformSRV = _shader->AsSRV("TransformsMap");
	sFrameBuffer = _shader->AsConstantBuffer("CB_AnimationFrame");
	sBlendBuffer = _shader->AsConstantBuffer("CB_BlendingFrame");

	//Create Compute Shader
	{
		computeShader = Debug_new Shader(L"34_GetBones.fxo");

		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendingFrame");

		sComputeTransformsSRV = computeShader->AsSRV("TransformsMap");

		computeBoneBuffer = Debug_new StructuredBuffer(NULL, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones");

		computeWorldBuffer = Debug_new StructuredBuffer(NULL, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputWorldBuffer = computeShader->AsSRV("InputWorlds");

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));

		computeOutputBuffer = Debug_new TextureBuffer(texture);
		sComputeOutputBuffer = computeShader->AsUAV("Output");

		SAFE_RELEASE(texture);

		attachBones = Debug_new Matrix*[MAX_MODEL_INSTANCE];
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
			attachBones[i] = Debug_new Matrix[MAX_MODEL_TRANSFORMS];
	}
}

void ModelAnimator::Pass(UINT _pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(_pass);
}

void ModelAnimator::GetAttachBones(UINT _instance, Matrix * _matrix)
{
	memcpy(_matrix, attachBones[_instance], sizeof(Matrix) * MAX_MODEL_TRANSFORMS);

}

void ModelAnimator::UpdateClips()
{
	SAFE_DELETE_ARRAY(clipTransforms);

	for (ModelMesh* mesh : model->Meshes())
		mesh->SetShader(shader);

	CreateTexture();

	Matrix bones[MAX_MODEL_TRANSFORMS]; //원본 Bone값
	for (UINT i = 0; i < model->BoneCount(); i++)
		bones[i] = model->BoneByIndex(i)->GetTransform();

	computeBoneBuffer->CopyToInput(bones);
}

void ModelAnimator::CreateTexture()
{
	// Matrix transform[500][250]; //StackOverflow

	clipTransforms = Debug_new ClipTransform[model->ClipCount()];
	for (UINT i = 0; i < model->ClipCount(); i++)
		CreateClipTransform(i);

	//Create Texture (animation texture)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;		// 행렬 하나에 4개의 픽셀필요
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// 16BYTE
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		
		//UINT pageSize = MAX_MODEL_TRANSFORMS * sizeof(Matrix) * MAX_MODEL_KEYFRAMES;
		UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;

		void* p = VirtualAlloc(NULL, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);	// Memory를 예약해놓고 빈곳에 채워넣음 ==> 단편화가 적음

		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT start = c * pageSize;

			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				void* temp = (BYTE*)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start;

				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}

		D3D11_SUBRESOURCE_DATA* subResource = Debug_new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
			subResource[c].SysMemSlicePitch = pageSize;
		}
		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResource, &texture));

		SAFE_DELETE_ARRAY(subResource);
		VirtualFree(p, 0, MEM_RELEASE);
	}//Create Texture ==> V-Ram에 Memory로 있음

	//Create Shader Resource View
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = model->ClipCount();

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &transformsSRV));
	}//Create SRV


}

void ModelAnimator::CreateClipTransform(UINT _index)
{
	Matrix* bones = Debug_new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(_index);
	for (UINT f = 0; f < clip->GetFrameCount(); f++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix inv = bone->GetTransform();
			D3DXMatrixInverse(&inv, NULL, &inv);

			Matrix parent;
			int parentIndex = bone->GetParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];

			Matrix animation;
			std::shared_ptr<ModelKeyframe> frame = clip->GetKeyframe(bone->GetName()); 

			if (frame != NULL)
			{
				ModelKeyframeData& data = frame->Transforms[f]; //fbx에서 빼온 데이터

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T;
			}
			else
			{
				D3DXMatrixIdentity(&animation);
			}//if(frame)

			bones[b] = animation * parent;	// ==> animation된 행렬이 부모만큼 이동(실체는 없다)
			clipTransforms[_index].Transform[f][b] = inv * bones[b]; // 해당 정점(Mesh)이 보여질 행렬
												//자기 Bone의 값
		}
	}
	SAFE_DELETE_ARRAY(bones);

}
void ModelAnimator::OnInit()
{
}
void ModelAnimator::OnStart()
{
}
void ModelAnimator::OnUpdate()
{
	Update();
}
void ModelAnimator::OnStop()
{
}
void ModelAnimator::OnDestroy()
{
}
void ModelAnimator::OnRender()
{
	Render();
}
Transform * ModelAnimator::AddTransform()
{
	Transform* transform = Debug_new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);
	
	return transform;
}

void ModelAnimator::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
	{
		transforms[i]->Update();
		//if (!transforms[i]->GetRender()) continue;
		memcpy(worlds[i], transforms[i]->GetWorld(), sizeof(Matrix));
	}

	computeWorldBuffer->CopyToInput(worlds);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);


	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);
}

void ModelAnimator::SetColor(UINT _instance, Color & _color)
{
	colors[_instance] = _color;
}

