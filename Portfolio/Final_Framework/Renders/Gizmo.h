#pragma once

class Gizmo
{
public:
	static void Create();
	static void Delete();

	static Gizmo* Get();

private:
	Gizmo();
	~Gizmo();

public:
	void Update();
	void Render();

public:
	void SetTransform(class Transform* _transform);
	Transform* GetTransform() { return transform; }
private:
	static Gizmo* instance;

private:
	class Transform* transform = NULL;

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::WORLD;
};