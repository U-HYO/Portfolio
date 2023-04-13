#pragma once
#include "Systems/IExecute.h"

class Main : public IExecute
{

	// IExecute��(��) ���� ��ӵ�
	void Init() override;
	void Release() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override;

private:
	void Push(IExecute* _exe);

private:
	std::vector<IExecute*> executes;

};

