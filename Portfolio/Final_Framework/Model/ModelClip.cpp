#include "Framework.h"
#include "ModelClip.h"

ModelClip::ModelClip()
{

}

ModelClip::~ModelClip()
{

}

std::shared_ptr<ModelKeyframe> ModelClip::GetKeyframe(std::wstring _name)
{
	if (keyframeMap.count(_name) < 1)
		return NULL;

	return keyframeMap[_name];
}
