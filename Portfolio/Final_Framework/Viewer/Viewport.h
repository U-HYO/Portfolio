#pragma once

class Viewport
{
public:
	Viewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	~Viewport();

	void RSSetViewport();
	void Set(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);

	float GetWidth() { return width; }
	float GetHeight() { return height; }

	Vector3 Project(Vector3& _source, Matrix& _world, Matrix& _view, Matrix& _projection); // 투영
	Vector3 UnProject(Vector3& _source, Matrix& _world, Matrix& _view, Matrix& _projection); // 투영한걸 뒤집음

	void GetMouseRay(Vector3* _outPosition, Vector3* _outDirection, Matrix* _world = NULL);

	Vector2 GetPos() { return Vector2(x, y); }
	Vector2 GetSize() { return Vector2(width, height); }

private:
	float x, y;
	float width, height;
	float minDepth, maxDepth;

	D3D11_VIEWPORT viewport;
};