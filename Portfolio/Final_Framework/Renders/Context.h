#pragma once

class Context
{
public:
	static Context* Get();
	static void Create();
	static void Delete();

private:
	Context();
	~Context();

public:
	void ResizeScreen();

	void Update();
	void Render();

	Matrix View();
	Matrix Projection();

	class Perspective* GetPerspective() { return perspective; }
	class Viewport* GetViewport() { return viewport; } // 메인viewport로 적용

	class Camera* GetCamera(UINT _index) { return cameras[_index]; }
	void SetCameraIndex(UINT _index) { currentCameraIndex = _index; }
	UINT GetCameraIndex() { return currentCameraIndex; }

	Plane* GetCulling() { return culling; }
	Plane& GetCulling(UINT _index) { return culling[_index]; }
	Plane& GetClipping() { return clipping; }

private:
	static Context* instance;

private:
	class Perspective* perspective;
	class Viewport* viewport;
	//class Camera* camera;
	std::vector<class Camera*> cameras;
	UINT currentCameraIndex;

	Plane culling[4];
	Plane clipping;


};