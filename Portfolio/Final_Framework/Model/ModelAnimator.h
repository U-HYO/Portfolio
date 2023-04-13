#pragma once
#include "Component/IComponent.h"

class ModelAnimator : public IComponent
{
public:
	ModelAnimator(Shader* _shader);
	ModelAnimator(class Actor* _actor, class Transform* _transform);
	~ModelAnimator();

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;
	void OnDestroy() override;
	void OnRender() override;

	void Update();
	void Render();

private:
	void UpdateAnimationFrame(UINT _instance);
	void UpdateBlendingFrame(UINT _instance);

public:
	void ReadMaterial(std::wstring _file);
	void ReadMesh(std::wstring _file);
	//void ReadClip(std::wstring _file);
	void ReadClip(std::wstring _file, bool isPath = false);

	//	클립을 바꿔줌						플레이될 속도, 바뀌어갈 시간(비율)
	void PlayTweenMode(UINT _instance, UINT _clip, float _speed = 1.0f, float _takeTime = 1.0f);
	void PlayBlendMode(UINT _instance, UINT _clip0, UINT _clip1, UINT _clip2);
	void SetBlendAlpha(UINT _instance, float _alpha);

	UINT GetCurrentAnim() { return currentAnim; }

public:
	Model* GetModel() { return model; }

	Transform* AddTransform();
	Transform* GetTransform(UINT _index) { return transforms[_index]; }
	std::vector<Transform*> GetTransforms() { return transforms; }
	UINT GetTransformsCount() { return transforms.size(); }
	void UpdateTransforms();

	void SetColor(UINT _instance, Color& _color);
	
	

	void SetShader(Shader* _shader);
public:
	void Pass(UINT _pass);

	void GetAttachBones(UINT _instance, Matrix* _matrix);

	void UpdateClips();

private:
	void CreateTexture();
	void CreateClipTransform(UINT _index);

	struct ClipTransform
	{
		Matrix** Transform;

		ClipTransform()
		{
			Transform = Debug_new Matrix*[MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Transform[i] = Debug_new Matrix[MAX_MODEL_TRANSFORMS];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SAFE_DELETE_ARRAY(Transform[i]);

			SAFE_DELETE_ARRAY(Transform);
		}
	};

	ClipTransform* clipTransforms;

	ID3D11Texture2D* texture = NULL;
	ID3D11ShaderResourceView* transformsSRV;
	ID3DX11EffectShaderResourceVariable* sTransformSRV;


private:
	struct KeyframeDesc
	{
		int Clip = 0;			// 플레이 시킬 번호

		UINT CurrFrame = 0;		// 현재 플레이될 프레임
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Speed = 1.0f;

		Vector2 Padding;
	};// keyframeDesc;

private:
	struct TweenDesc
	{
		float TakeTime = 1.0f; //Speed 바뀌어가는 속도
		float TweenTime = 0.0f; //Time 바뀌는 시간
		float ChangeTime = 0.0f; //Running 바뀌는 현재 시간
		float Padding;

		KeyframeDesc Curr;	// 현재동작
		KeyframeDesc Next;	// 바뀔동작

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}
	} tweenDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;		// 1일때만 작동
		float Alpha = 0;	// 0이면 0번째 1이면 1번째 
		Vector2 Padding;

		KeyframeDesc Clip[3];	// 동작을 더 늘리고싶으면 더 넣어도됨
	} blendDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;


private:
	Shader* shader;
	Model* model;

	std::vector<Transform *> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer;

	Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceColorBuffer;

private:
	float frameRate = 60.0f;
	float frameTime = 0.0f;

	Matrix** attachBones;

	Shader* computeShader;

	ID3DX11EffectMatrixVariable* sComputeWorld;

	ID3DX11EffectConstantBuffer* sComputeFrameBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeTransformsSRV;

	StructuredBuffer* computeWorldBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputWorldBuffer;

	StructuredBuffer* computeBoneBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputBoneBuffer;

	TextureBuffer* computeOutputBuffer;
	ID3DX11EffectUnorderedAccessViewVariable* sComputeOutputBuffer;

	UINT currentAnim = 0;

private:


};

// 갯수가 많아서 Texture로 넘긴다
// x : BoneIndex
// y : KeyFrame
// z : Clip



