#pragma once
#include "Systems/IExecute.h"

class Main : public IExecute
{

	// IExecute을(를) 통해 상속됨
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

