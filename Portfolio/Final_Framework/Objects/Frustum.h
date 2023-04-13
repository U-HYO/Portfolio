#pragma once
class Frustum
{
public:
	Frustum(Camera* camera = NULL, Perspective* perspective = NULL);
	~Frustum();

	void Update();
	void Planes(Plane* planes);
	void PlanesTerrain(Plane* planes);

	void GetPlanes(Plane* planes, UINT count);

	bool CheckPoint(Vector3& position);
	bool CheckCube(Vector3& center, Vector3& size); // ������ü
	bool CheckCube(Vector3& center, float radius); // ������ü

	void SetPerspective(Perspective* perspective);

private:
	Plane planes[6];

	Camera* camera;
	Perspective* perspective;

	int camIndex;
};

