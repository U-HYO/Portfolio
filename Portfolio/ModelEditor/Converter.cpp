#include "stdafx.h"
#include "Converter.h"
#include "Types.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"

Converter::Converter()
{
	importer = Debug_new Assimp::Importer();
}

Converter::~Converter()
{
	for (asBone* bone : bones)
		SAFE_DELETE(bone);

	for (asMesh* mesh : meshes)
	{
		for (asMeshPart* part : mesh->MeshParts)
			SAFE_DELETE(part);

		SAFE_DELETE(mesh);
	}

	for (asMaterial* material : materials)
		SAFE_DELETE(material);


	SAFE_DELETE(importer);
}

void Converter::ReadFile(std::wstring file)
{
	this->file = L"../../_Assets/" + file;

	scene = importer->ReadFile
	(
		String::ToString(this->file),
		aiProcess_ConvertToLeftHanded
		| aiProcess_Triangulate
		| aiProcess_GenUVCoords
		| aiProcess_GenNormals
		| aiProcess_CalcTangentSpace
	);

	if (scene == NULL)
	{
		std::string str = importer->GetErrorString();

		MessageBoxA(D3D::GetDesc().Handle, str.c_str(), "Assimp Error", MB_OK);
		exit(-1);
	}
}

void Converter::ExportMesh(std::wstring savePath)
{
	ReadBoneData(scene->mRootNode, -1, -1);
	ReadSkinData(0);

	savePath = L"../../_Models/" + savePath + L".mesh";
	WriteMeshData(savePath);
}

void Converter::ReadBoneData(aiNode * node, int index, int parent)
{
	asBone* bone = Debug_new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();

	Matrix transform(node->mTransformation[0]);
	D3DXMatrixTranspose(&bone->Transform, &transform);


	Matrix matParent;
	if (parent < 0)
		D3DXMatrixIdentity(&matParent);
	else
		matParent = bones[parent]->Transform;

	bone->Transform = bone->Transform * matParent;
	bones.push_back(bone);

	ReadMeshData(node, index);


	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Converter::ReadMeshData(aiNode * node, int index)
{
	if (node->mNumMeshes < 1) return;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		asMesh* mesh = Debug_new asMesh();
		mesh->BoneIndex = index;

		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];


		UINT startVertex = mesh->Vertices.size();
		UINT startIndex = mesh->Indices.size();

		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			Model::VertexModel vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));

			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.UV, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

			if (srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			if (srcMesh->HasTangentsAndBitangents())
				memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);
		}

		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
				mesh->Indices.push_back(face.mIndices[k] + startVertex);
		}


		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];

		asMeshPart* meshPart = Debug_new asMeshPart();
		meshPart->MaterialName = material->GetName().C_Str();
		if (meshPart->MaterialName == "")
		{
			meshPart->MaterialName = std::string(String::ToString(Path::GetFileNameWithoutExtension(this->file)) + "_" + std::to_string(i));
		}
		meshPart->StartVertex = startVertex;
		meshPart->StartIndex = startIndex;
		meshPart->VertexCount = srcMesh->mNumVertices;
		meshPart->IndexCount = srcMesh->mNumFaces * srcMesh->mFaces->mNumIndices;

		mesh->MeshParts.push_back(meshPart);

		meshes.push_back(mesh);
	}//for(i)
}

void Converter::ReadSkinData()
{
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones() == false) continue;


		asMesh* mesh = meshes[i];

		std::vector<asBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeights());

		for (UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			aiBone* aiMeshBone = aiMesh->mBones[b];

			UINT boneIndex = 0;
			for (asBone* bone : bones)
			{
				if (bone->Name == (std::string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;

					break;
				}
			}//for(bone)

			for (UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				UINT index = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				//index - 정점 번호
				//boneIndex - index의 정점이 영향 받을 본의 번호
				boneWeights[index].AddWeights(boneIndex, weight);
			}//for(w)
		}//for(b)

		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[w].Normalize();

			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);

			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

void Converter::ReadSkinData(UINT _i)
{
	for (UINT i = 0; i < meshes.size(); i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones() == false) continue;


		asMesh* mesh = meshes[i];

		std::vector<asBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeights());

		for (UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			aiBone* aiMeshBone = aiMesh->mBones[b];

			UINT boneIndex = 0;
			for (asBone* bone : bones)
			{
				if (bone->Name == (std::string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;

					break;
				}
			}//for(bone)

			for (UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				UINT index = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				//index - 정점 번호
				//boneIndex - index의 정점이 영향 받을 본의 번호
				boneWeights[index].AddWeights(boneIndex, weight);
			}//for(w)
		}//for(b)

		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[w].Normalize();

			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);

			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

void Converter::WriteMeshData(std::wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = Debug_new BinaryWriter(savePath);

	w->UInt(bones.size());
	for (asBone* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);
	}

	w->UInt(meshes.size());
	for (asMesh* meshData : meshes)
	{
		w->Int(meshData->BoneIndex);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::VertexModel) * meshData->Vertices.size());

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());


		w->UInt(meshData->MeshParts.size());
		for (asMeshPart* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCount);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);
		}
	}

	SAFE_DELETE(w);
}

void Converter::ExportMaterial(std::wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Textures/" + savePath + L".material";

	if (bOverwrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}


	ReadMaterialData();
	WriteMaterialData(savePath);
}

void Converter::ReadMaterialData()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];

		std::string materialName = FoundMaterialData(srcMaterial, i);

		if (materialName == "")
			continue;

		asMaterial* material = Debug_new asMaterial();
		material->Name = materialName;


		aiColor3D color;

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->Ambient = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->Diffuse = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->Specular = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_SHININESS, material->Specular.a);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->Emissive = Color(color.r, color.g, color.b, 1.0f);


		aiString file;

		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();

		materials.push_back(material);
	}
}

std::string Converter::FoundMaterialData(aiMaterial* _material, UINT _i)
{
	std::string materialName = _material->GetName().C_Str();
	if (materialName == "")
		materialName = std::string(String::ToString(Path::GetFileNameWithoutExtension(this->file)) + "_" + std::to_string(_i));


	bool bFound = false;
	for (asMesh* mesh : meshes)
	{
		for (UINT i = 0; i < mesh->MeshParts.size(); i++)
		{
			if (mesh->MeshParts[i]->MaterialName == materialName)
				return materialName;
		}
	}

	return "";
}

void Converter::WriteMaterialData(std::wstring savePath)
{
	std::string folder = String::ToString(Path::GetDirectoryName(savePath));
	std::string file = String::ToString(Path::GetFileName(savePath));

	Path::CreateFolders(folder);


	Xml::XMLDocument* document = Debug_new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (asMaterial* material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);


		Xml::XMLElement* element = NULL;

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);


		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->DiffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->SpecularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->NormalFile).c_str());
		node->LinkEndChild(element);


		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r);
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->Emissive.r);
		element->SetAttribute("G", material->Emissive.g);
		element->SetAttribute("B", material->Emissive.b);
		element->SetAttribute("A", material->Emissive.a);
		node->LinkEndChild(element);
	}


	document->SaveFile((folder + file).c_str());
	SAFE_DELETE(document);
}

std::string Converter::WriteTexture(std::string saveFolder, std::string file)
{
	if (file.length() < 1) return "";


	std::string fileName = Path::GetFileName(file);
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());

	std::string path = "";
	if (texture != NULL)
	{
		path = saveFolder + Path::GetFileNameWithoutExtension(file) + ".png";

		BinaryWriter* w = Debug_new BinaryWriter(String::ToWString(path));
		w->Byte(texture->pcData, texture->mWidth);
		SAFE_DELETE(w);
	}
	else
	{
		std::string directory = Path::GetDirectoryName(String::ToString(this->file));
		std::string origin = directory + file;
		String::Replace(&origin, "\\", "/");

		if (Path::ExistFile(origin) == false)
			return "";


		path = saveFolder + fileName;
		CopyFileA(origin.c_str(), path.c_str(), FALSE);

		String::Replace(&path, "../../_Textures/", "");
	}

	return Path::GetFileName(path);
}

void Converter::GetClipList(std::vector<std::wstring>* list)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];

		list->push_back(String::ToWString(anim->mName.C_Str()));
	}
}

void Converter::ExportAnimClip(UINT index, std::wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".clip";

	asClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath);
}

asClip * Converter::ReadClipData(aiAnimation * animation)
{
	asClip* clip = Debug_new asClip();
	clip->Name = animation->mName.C_Str();
	clip->FrameRate = (float)animation->mTicksPerSecond;
	clip->FrameCount = (UINT)animation->mDuration + 1;

	std::vector<asClipNode> aniNodeInfos;
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		//scene->mRootNode : 씬의 루트(본) 노드 - ModelBone
		//aniNode : 애니메이션의 (본)노드
		aiNodeAnim* aniNode = animation->mChannels[i];

		asClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName;


		UINT keyCount = Math::Max(aniNode->mNumPositionKeys, aniNode->mNumRotationKeys);
		keyCount = Math::Max(keyCount, aniNode->mNumScalingKeys);

		asKeyframeData frameData;
		for (UINT k = 0; k < keyCount; k++)
		{
			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size();

			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mPositionKeys[k];
				memcpy_s(&frameData.Translation, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));
				frameData.Frame = (float)aniNode->mPositionKeys[k].mTime;

				bFound = true;
			}

			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiQuatKey key = aniNode->mRotationKeys[k];

				frameData.Rotation.x = key.mValue.x;
				frameData.Rotation.y = key.mValue.y;
				frameData.Rotation.z = key.mValue.z;
				frameData.Rotation.w = key.mValue.w;

				frameData.Frame = (float)aniNode->mRotationKeys[k].mTime;

				bFound = true;
			}

			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mScalingKeys[k];
				memcpy_s(&frameData.Scale, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));
				frameData.Frame = (float)aniNode->mScalingKeys[k].mTime;

				bFound = true;
			}

			if (bFound == true)
				aniNodeInfo.Keyframe.push_back(frameData);
		}//for(k)


		if (aniNodeInfo.Keyframe.size() < clip->FrameCount)
		{
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();
			asKeyframeData lastKeyframe = aniNodeInfo.Keyframe.back();

			for (UINT n = 0; n < count; n++)
				aniNodeInfo.Keyframe.push_back(lastKeyframe);
		}

		aniNodeInfos.push_back(aniNodeInfo);
	}

	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos);

	return clip;
}

void Converter::ReadKeyframeData(asClip * clip, aiNode * node, std::vector<struct asClipNode>& aniNodeInfos)
{
	asKeyframe* keyframe = Debug_new asKeyframe();
	keyframe->BoneName = node->mName.C_Str();


	asClipNode* asClipNode = NULL;
	for (UINT n = 0; n < aniNodeInfos.size(); n++)
	{
		if (aniNodeInfos[n].Name == node->mName)
		{
			asClipNode = &aniNodeInfos[n];

			break;
		}
	}

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		asKeyframeData frameData;
		if (asClipNode == NULL)
		{
			frameData.Frame = (float)i;

			Matrix transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform);

			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform);
		}
		else
		{
			frameData = asClipNode->Keyframe[i];
		}

		keyframe->Transforms.push_back(frameData);
	}

	clip->Keyframes.push_back(keyframe);


	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadKeyframeData(clip, node->mChildren[i], aniNodeInfos);
}

void Converter::WriteClipData(asClip * clip, std::wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = Debug_new BinaryWriter(savePath);

	w->String(clip->Name);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (asKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(asKeyframeData) * keyframe->Transforms.size());

		SAFE_DELETE(keyframe);
	}

	SAFE_DELETE(w);
}
