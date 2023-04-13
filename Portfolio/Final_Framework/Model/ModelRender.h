#pragma once
#include "Component/IComponent.h"

class ModelRender : public IComponent
{
public:
	ModelRender(Shader* _shader);
	ModelRender(class Actor* _actor, class Transform* _transform);
	~ModelRender();

	void Update();
	void Render();

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnStop() override;
	void OnDestroy() override;

	void SetShader(Shader* _shader);
public:
	void ReadMesh(std::wstring _file);
	void ReadMaterial(std::wstring _file);

	Model* GetModel() { return model; }

	Transform* AddTransform();
	Transform* GetTransform(UINT _index) { return transforms[_index]; }
	std::vector<Transform*> GetTransforms() { return transforms; }
	UINT GetTransformCount() { return transforms.size(); }
	void UpdateTransforms();

	void SetColor(UINT _instance, Color& _color);

	void Pass(UINT _pass);

public:
	

private:
	void CreateTexture();

	//void UpdateBoneTransforms();

private:
	Shader* shader;
	Model* model;

	std::vector<Transform *> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer;

	Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceColorBuffer;


	Matrix boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS];

	ID3D11Texture2D* texture = NULL;
	ID3D11ShaderResourceView* transformsSRV;
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;


};

