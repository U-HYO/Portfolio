#pragma once
#include "Component/IComponent.h"

#define MAX_MESH_INSTANCE 500
class MeshRender : public IComponent
{
public:
	MeshRender(Shader* _shader, Mesh* _mesh);
	MeshRender(class Actor* _actor, class Transform* _transform);
	~MeshRender();

	Mesh* GetMesh() { return mesh; }

	void Update();
	void Render();

	// IComponent을(를) 통해 상속됨
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnStop() override;
	void OnDestroy() override;
	void SetMesh(Mesh* _mesh, Shader* _shader);


	void Pass(UINT _val) { mesh->Pass(_val); }

	Transform* AddTransform();
	void SetTransform(UINT _index, Transform* _transform) { transforms[_index] = _transform; }
	Transform* GetTransform(UINT _index) { return transforms[_index]; }
	void UpdateTransforms();

	void SetColor(UINT _instance, Color& _color);
	void SetMaterial(std::string _fileName);

	std::string GetMaterialName() { return materialName; }

public:
	Material* GetMaterial() { return material; }


private:
	Mesh* mesh;

	std::vector<Transform*> transforms;
	Matrix worlds[MAX_MESH_INSTANCE];
	VertexBuffer* instanceBuffer;

	Color colors[MAX_MESH_INSTANCE];
	VertexBuffer* instanceColorBuffer;

	Material* material;
	std::string materialName;


};

