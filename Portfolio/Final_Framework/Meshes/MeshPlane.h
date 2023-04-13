#pragma once

class MeshPlane : public Mesh
{
public:
	MeshPlane(float _offsetU = 1.0f, float _offsetV = 1.0f);
	~MeshPlane();

private:
	void Create() override;

	float offsetU, offsetV;
};