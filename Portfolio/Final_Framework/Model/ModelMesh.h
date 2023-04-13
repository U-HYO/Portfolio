#pragma once

class ModelBone
{
public:
	friend class Model;

private:
	ModelBone();
	~ModelBone();

public:
	int Index() { return index; }

	int GetParentIndex() { return parentIndex; }
	ModelBone* GetParent() { return parent; }

	std::wstring GetName() { return name; }

	Matrix& GetTransform() { return transform; }
	void SetTransform(Matrix& matrix) { transform = matrix; }

	std::vector<ModelBone *>& GetChilds() { return childs; }

private:
	int index;
	std::wstring name;

	int parentIndex;
	ModelBone* parent;

	Matrix transform;
	std::vector<ModelBone *> childs;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMesh
{
public:
	friend class Model;

private:
	ModelMesh();
	~ModelMesh();

	void Binding(Model * _model);

public:
	void Pass(UINT _val);
	void SetShader(Shader* _shader);

	void Update();
	void Render();
	void Render(UINT _drawCount);
	void Render(Transform _transform);

	int BoneIndex() {return boneIndex; }
	class ModelBone* Bone() { return bone; }

	void Transforms(Matrix* _transforms);
	void SetTransform(Transform* _transform);

private:
	struct BoneDesc
	{
		//Matrix Transform[MAX_MODEL_TRANSFORMS];

		UINT BoneIndex;
		float Padding[3];
	} boneDesc;

private:
	Shader* shader;
	
	Transform* transform = NULL; 
	PerFrame* perFrame = NULL;

	int boneIndex;
	ModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::VertexModel* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;

	std::vector<class ModelMeshPart*> meshParts;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMeshPart
{
public:
	friend class Model;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

	void Update();
	void Render();
	void Render(UINT _drawCount);

	void SetBinding(Model* _model);
	void SetShader(Shader* _shader);

	void SetPass(UINT _val) { pass = _val; }

private:
	Shader* shader;
	UINT pass = 0;

	Material* material = NULL;
	std::wstring materialName;

	UINT startVertex;
	UINT vertexCount;

	UINT startIndex;
	UINT indexCount;
};