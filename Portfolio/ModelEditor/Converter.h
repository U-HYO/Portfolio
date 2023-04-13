#pragma once

class Converter
{
public:
	Converter();
	~Converter();

	void ReadFile(std::wstring _file);

public:
	void ExportMesh(std::wstring _file);


private:
	void ReadBoneData(aiNode* _node, int _index, int _parent);
	void ReadMeshData(aiNode* _node, int _index);
	void ReadSkinData();
	void ReadSkinData(UINT _i);
	void WriteMeshData(std::wstring _savePath);

public:
	void ExportMaterial(std::wstring _file, bool bOverwrite = true);

private:
	void ReadMaterialData();
	//bool FoundMaterialData(aiMaterial* _material);
	std::string FoundMaterialData(aiMaterial* _material, UINT _i);

	std::string WriteTexture(std::string _saveFolder, std::string _file);
	void WriteMaterialData(std::wstring _file);

public:
	void GetClipList(std::vector<std::wstring>* _list);
	void ExportAnimClip(UINT _index, std::wstring _savePath);

private:
	struct asClip* ReadClipData(aiAnimation* _animation);
	void ReadKeyframeData(struct asClip* _clip, aiNode* _aniNode, std::vector<struct asClipNode>& _aniNodeInfos);
	void WriteClipData(struct asClip* _clip, std::wstring _savePath);


private:
	std::wstring file; // 불러올 파일(모델) 이름

	Assimp::Importer* importer;
	const aiScene* scene;

	std::vector<struct asBone*> bones;
	std::vector<struct asMesh*> meshes;
	std::vector<struct asMaterial*> materials;

};

