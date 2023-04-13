#pragma once
#include "stdafx.h"

//Bone == Model이 그려질 때 자기의 world matrix로 옮길 때 필요함(Mesh의 World)
struct asBone		//as == assimp
{
	int Index;
	std::string Name;

	int Parent;
	Matrix Transform;	//Mesh 가 참조할 World
};

struct asMeshPart
{
	std::string MaterialName;

	UINT StartVertex;
	UINT VertexCount;

	UINT StartIndex;
	UINT IndexCount;	         
};

struct asMesh
{
	int BoneIndex;

	std::vector<Model::VertexModel> Vertices;
	std::vector<UINT> Indices;

	std::vector<asMeshPart*> MeshParts;
};

struct asMaterial
{
	std::string Name;

	Color Ambient;
	Color Diffuse; // 모델의 색
	Color Specular;
	Color Emissive;

	std::string DiffuseFile;
	std::string SpecularFile;
	std::string NormalFile;
};


struct asBlendWeight
{
	Vector4 Indices = Vector4(0, 0, 0, 0);
	Vector4 Weights = Vector4(0, 0, 0, 0);

	void Set(UINT index, UINT boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: Indices.x = i; Weights.x = w; break;
		case 1: Indices.y = i; Weights.y = w; break;
		case 2: Indices.z = i; Weights.z = w; break;
		case 3: Indices.w = i; Weights.w = w; break;
		}
	}
};

struct asBoneWeights
{
private:
	typedef std::pair<int, float> Pair;
	std::vector<Pair> BoneWeights;

public:
	void AddWeights(UINT boneIndex, float boneWeights)
	{
		if (boneWeights <= 0.0f) return;

		bool bAdd = false;
		std::vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (boneWeights > it->second)
			{
				BoneWeights.insert(it, Pair(boneIndex, boneWeights));
				bAdd = true;

				break;
			}

			it++;
		} // while(it)

		if (bAdd == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeights));
	}

	void GetBlendWeights(asBlendWeight& blendWeights)
	{
		for (UINT i = 0; i < BoneWeights.size(); i++)
		{
			if (i >= 4) return;

			blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
		}
	}

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;
		std::vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				i++; it++;
			}
			else
				it = BoneWeights.erase(it);
		}

		float scale = 1.0f / totalWeight;

		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= scale;
			it++;
		}
	}
};

struct asKeyframeData
{
	float Frame;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

struct asKeyframe
{
	std::string BoneName;
	std::vector<asKeyframeData> Transforms;
};

struct asClip
{
	std::string Name;

	UINT FrameCount;
	float FrameRate;

	std::vector<asKeyframe *> Keyframes;
};

//aniNode의 원본 키프레임 저장
struct asClipNode
{
	aiString Name;
	std::vector<asKeyframeData> Keyframe;
};

