#pragma once
#include "Systems/IExecute.h"

class UI : public IExecute
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
	void LeftSide();
	void LeftFirst();
	void LeftSecond();
	void LeftThird();

	void RightSide();
	void RightBottom();

	void AddRightNodes(ModelBone* bone);

	//ImGuiTreeNodeFlags SelectNode();
	void ShowInspector();
	void TerrainEditor();
	void TerrainEditorLOD();
	void ParticleEditor();
	void ObjectModelInspector();
	void ObjectMeshInspector();
	void LightInspector();
public:
	bool showUI;

	//First
	bool showAdd;
	bool showInspector;
	bool showSetting;

	bool showSecond;
	bool showThird;

	ComponentType thirdType;

	std::vector<std::wstring> dataFileList;

	class FileDialog* fileDialogView;
	class FileDialog* fileDialogLoad;

	class FileDialog* inspecFileDialogView;

	std::string clickedDirectory;
	std::string clickedPath;

	Actor* selectedActor;
	UINT nodeClicked;

	bool showFileDialog;
	bool fileDialogUpdate;


private:

};

