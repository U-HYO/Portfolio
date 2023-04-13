#pragma once
#include "Systems/IExecute.h"

//파일 읽어서 변환만
class Editor : public IExecute
{
	// IExecute을(를) 통해 상속됨
	virtual void Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Mesh();

	void UpdateParticleList();
	void UpdateTextureList();

	void OnGUI();
	void OnGUI_List();
	void OnGUI_Settings();
	void OnGUI_Write();
	void WriteFile(std::wstring files);

private:
	Shader* shader;
	CubeSky* sky;

	float windowWidth = 500;

	bool bLoop = false;
	UINT maxParticle = 0;

	std::vector<std::wstring> particleList;
	std::vector<std::wstring> textureList;

	std::wstring file = L"";
	Particle* particle = NULL;

	Material* floor;
	Material* stone;

	MeshRender* sphere;
	MeshRender* grid;

//==========================
	std::vector<Texture*> textures;
	//std::vector<std::shared_ptr<class Texture>> textures;

};