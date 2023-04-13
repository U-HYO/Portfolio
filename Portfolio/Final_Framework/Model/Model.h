#pragma once

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500		// 키프레임 최대갯수
#define MAX_MODEL_INSTANCE 500
class ModelBone;
class ModelMesh;
class ModelMeshPart;
class ModelClip;

class Model
{
public:
	friend class ModelRender;
	friend class ModelAnimator;
	struct VertexModel;

private:
	Model();
	~Model();
	
private:
	void ReadMesh(std::wstring _file);
	void ReadMaterial(std::wstring _file);
	void ReadClip(std::wstring _file, bool isPath = false);

private:
	void BindBone();
	void BindMesh();

public:
	UINT BoneCount() { return bones.size(); }
	std::vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT _index) { return bones[_index]; }
	ModelBone* BoneByName(std::wstring name);

	UINT MeshCount() { return meshes.size(); }
	std::vector<ModelMesh *>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT _index) { return meshes[_index]; }

	UINT MaterialCount() { return materials.size(); }
	std::vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT _index) { return materials[_index]; }
	Material* MaterialByName(std::wstring _name);

	UINT ClipCount() { return clips.size(); }
	std::vector<ModelClip *>& Clips() { return clips; }
	ModelClip* ClipByIndex(UINT _index) { return clips[_index]; }
	ModelClip* ClipByName(std::wstring _name);



public:
	struct VertexModel
	{
		Vector3 Position;
		Vector2 UV;
		Vector3 Normal;
		Vector3 Tangent;
		Vector4 BlendIndices;
		Vector4 BlendWeights;
		
		VertexModel()
		{
			Position = Vector3(0, 0, 0);
			UV = Vector2(0, 0);
			Normal = Vector3(0, 0, 0);
			Tangent = Vector3(0, 0, 0);
			BlendIndices = Vector4(0, 0, 0, 0);
			BlendWeights = Vector4(0, 0, 0, 0);
		}
	};

private:
	ModelBone* root;

	std::vector<ModelBone*> bones;
	std::vector<ModelMesh*> meshes;
	std::vector<ModelClip*> clips;
	std::vector<Material*> materials;


};

