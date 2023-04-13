#pragma once
#include "Systems/IExecute.h"

//���� �о ��ȯ��
class MapEditor : public IExecute
{
	// IExecute��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};



private:
	void ToDataMapFile();
	void UpdateDataMapFileList();

private:
	Shader* shader;
	Shadow* shadow;
	CubeSky* sky;

private:
	std::wstring heightMapFileDirectory;
	std::wstring heightMapFileName;
	Texture* heightMapFile = NULL;

private:
	enum class Channel
	{
		Red = 0, Green, Blue, Alpha, Max,
	};
	int selectedChannel = 0;

	std::wstring terrainMapfileName;

	std::vector<std::wstring> dataMapFileList;

private:
	
	Terrain* terrain = NULL;
	class Brush* brush = NULL;
};
