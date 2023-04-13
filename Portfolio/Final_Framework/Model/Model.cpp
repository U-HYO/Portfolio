#include "Framework.h"
#include "Model.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"

Model::Model()
{
}

Model::~Model()
{
	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);

	for (Material* material : materials)
		SAFE_DELETE(material);

	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

}

void Model::ReadMesh(std::wstring _file)
{
	_file = L"../../_Models/" + _file + L".mesh";

	BinaryReader* r = Debug_new BinaryReader(_file);

	UINT count = 0;
	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = Debug_new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = Debug_new ModelMesh();

		mesh->boneIndex = r->Int();

		//VertexData
		{
			UINT count = r->UInt();

			std::vector<Model::VertexModel> vertices;
			vertices.assign(count, Model::VertexModel());

			void* ptr = (void *)&(vertices[0]);
			r->Byte(&ptr, sizeof(Model::VertexModel) * count);


			mesh->vertices = Debug_new Model::VertexModel[count];
			mesh->vertexCount = count;
			copy
			(
				vertices.begin(), vertices.end(),
				stdext::checked_array_iterator<Model::VertexModel *>(mesh->vertices, count)
			);
		}

		//IndexData
		{
			UINT count = r->UInt();

			std::vector<UINT> indices;
			indices.assign(count, UINT());

			void* ptr = (void *)&(indices[0]);
			r->Byte(&ptr, sizeof(UINT) * count);


			mesh->indices = Debug_new UINT[count];
			mesh->indexCount = count;
			copy
			(
				indices.begin(), indices.end(),
				stdext::checked_array_iterator<UINT *>(mesh->indices, count)
			);
		}


		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = Debug_new ModelMeshPart();
			meshPart->materialName = String::ToWString(r->String());

			meshPart->startVertex = r->UInt();
			meshPart->vertexCount = r->UInt();

			meshPart->startIndex = r->UInt();
			meshPart->indexCount = r->UInt();

			mesh->meshParts.push_back(meshPart);
		}//for(k)

		meshes.push_back(mesh);
	}//for(i)
	SAFE_DELETE(r);

	BindBone();
}

void Model::ReadMaterial(std::wstring _file)
{
	_file = L"../../_Textures/" + _file + L".material";

	Xml::XMLDocument* document = Debug_new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(_file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* materialNode = root->FirstChildElement();

	do
	{
		Material* material = Debug_new Material();


		Xml::XMLElement* node = NULL;

		node = materialNode->FirstChildElement();
		material->Name(String::ToWString(node->GetText()));

		std::wstring directory = Path::GetDirectoryName(_file);
		String::Replace(&directory, L"../../_Textures", L"");


		std::wstring texture = L"";

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->DiffuseMap(directory + texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SpecularMap(directory + texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->NormalMap(directory + texture);


		D3DXCOLOR color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Ambient(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Diffuse(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Specular(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Emissive(color);

		materials.push_back(material);

		materialNode = materialNode->NextSiblingElement();
	} while (materialNode != NULL);

	BindMesh();
	
	SAFE_DELETE(document);
}

void Model::ReadClip(std::wstring _file, bool isPath)
{
	if (!isPath)
		_file = L"../../_Models/" + _file + L".clip";

	BinaryReader* r = Debug_new BinaryReader(_file);

	ModelClip* clip = Debug_new ModelClip();
	clip->name = String::ToWString(r->String());
	clip->frameRate = r->Float();
	clip->frameCount = r->UInt();

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		//ModelKeyframe* keyframe = Debug_new ModelKeyframe();
		std::shared_ptr<ModelKeyframe> keyframe = std::make_shared<ModelKeyframe>();

		keyframe->BoneName = String::ToWString(r->String());


		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->Transforms.assign(size, ModelKeyframeData());

			void* ptr = (void *)&keyframe->Transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyframeData) * size);
		}

		clip->keyframeMap[keyframe->BoneName] = keyframe;
	}
	SAFE_DELETE(r);

	clips.push_back(clip);
}

void Model::BindBone()
{
	root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
			bone->parent = NULL;
	}
}

void Model::BindMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		mesh->bone = bones[mesh->boneIndex];
		mesh->Binding(this);
	}
}

ModelBone * Model::BoneByName(std::wstring _name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->GetName() == _name)
			return bone;
	}

	return nullptr;
}

Material * Model::MaterialByName(std::wstring _name)
{
	for (Material* material : materials)
	{
		if (material->Name() == _name)
			return material;
	}

	return nullptr;
}

ModelClip * Model::ClipByName(std::wstring _name)
{
	return nullptr;
}
