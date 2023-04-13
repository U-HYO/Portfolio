#pragma once

struct ModelKeyframeData
{
	float Frame;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

struct ModelKeyframe
{
	std::wstring BoneName;
	std::vector<ModelKeyframeData> Transforms;
};

class ModelClip
{
public:
	friend class Model;

private:
	ModelClip();
	~ModelClip();

public:
	float GetFrameRate() { return frameRate; }
	UINT GetFrameCount() { return frameCount; }

	////////////////////////////////////////
	//TODO : ªË¡¶
	void SetFrameRate(float _v) { frameRate = _v; }
	void SetFrameCount(UINT _v) { frameCount = _v; }


	std::shared_ptr<ModelKeyframe> GetKeyframe(std::wstring _name);

private:
	std::wstring name;

	float frameRate;
	UINT frameCount;

	//std::unordered_map<std::wstring, ModelKeyframe *> keyframeMap;
	std::unordered_map<std::wstring,std::shared_ptr<ModelKeyframe>> keyframeMap;
};