#pragma once

class Perspective : public Projection
{
public:
	Perspective(float width, float height, float zn = 0.1f, float zf = std::numeric_limits<float>::max(), float fov = (float)D3DX_PI * 0.25f);
	~Perspective();

	void Set(float width, float height, float zn = 0.1f, float zf = std::numeric_limits<float>::max(), float fov = (float)D3DX_PI * 0.25f) override;
	

private:
	float aspect;
};