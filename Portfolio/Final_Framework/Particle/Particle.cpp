#include "Framework.h"
#include "Particle.h"
#include "Utilities/Xml.h"

Particle::Particle(std::wstring file)
	: Renderer(L"41_Particle.fxo")
{
	ReadFile(L"../../_Textures/Particles/" + file + L".xml");

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Particle");

	sMap = shader->AsSRV("ParticleMap");


	Reset();
}

Particle::Particle(Actor * _actor, Transform * _transform, Shader * _shader)
	:Renderer(_actor, _transform, _shader)
{
	if (!_shader)
		SetShader(L"41_Particle.fxo");



}

Particle::~Particle()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(map);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
}

void Particle::Reset()
{
	
	currentTime = 0.0f;
	lastAddTime = _TIMER->Running();
	leadCount = gpuCount = activeCount = deactiveCount = 0;
	
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);

	vertices = Debug_new VertexParticle[data.MaxParticles * 4];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		vertices[i * 4 + 0].Corner = Vector2(-1, -1);
		vertices[i * 4 + 1].Corner = Vector2(-1, +1);
		vertices[i * 4 + 2].Corner = Vector2(+1, -1);
		vertices[i * 4 + 3].Corner = Vector2(+1, +1);
	}


	indices = Debug_new UINT[data.MaxParticles * 6];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	vertexBuffer = Debug_new VertexBuffer(vertices, data.MaxParticles * 4, sizeof(VertexParticle), 0, true);
	indexBuffer = Debug_new IndexBuffer(indices, data.MaxParticles * 6);
}

void Particle::Add(Vector3 & position)
{
	if (Time::Get()->Running() - lastAddTime < 60 / 1000.0f)
		return; //60프레임당 1번 추가

	lastAddTime = Time::Get()->Running();

	UINT next = leadCount + 1;

	if (next >= data.MaxParticles)
	{
		if (data.bLoop == true)
		{
			next = 0; // 끝까지 갔을때 맨 처음으로 보냄
		}
		else
		{
			next = data.MaxParticles;
			return;
		}
	}

	if (next == deactiveCount)
		return;


	Vector3 velocity = Vector3(1, 1, 1);
	velocity *= data.StartVelocity;

	float horizontalVelocity = Math::Lerp<float>(data.MinHorizontalVelocity, data.MaxHorizontalVelocity, Math::Random(0.0f, 1.0f));
	float horizontalAngle = Math::PI * 2.0f * Math::Random(0.0f, 1.0f);

	velocity.x += horizontalVelocity * cos(horizontalAngle);
	velocity.y += horizontalVelocity * sin(horizontalAngle);
	velocity.z += Math::Lerp<float>(data.MinVerticalVelocity, data.MaxVerticalVelocity, Math::Random(0.0f, 1.0f));

	Vector4 random = Math::RandomColor4();


	for (UINT i = 0; i < 4; i++)
	{
		vertices[leadCount * 4 + i].Position = position;
		vertices[leadCount * 4 + i].Velocity = velocity;
		vertices[leadCount * 4 + i].Random = random;
		vertices[leadCount * 4 + i].Time = currentTime;
	}

	leadCount = next;
}

void Particle::Add()
{
	Add(this->transform->GetPosition());

}

void Particle::Update()
{
	Add(this->transform->GetPosition());

	perFrame->Update();
	transform->Update();

	currentTime += Time::Delta();
	
	MapVertices();
	Activation();	// 활성화
	Deactivation();	// 수명시간이 다 된것을 비활성화

	if (activeCount == leadCount) // 전부 플레이 완료
		currentTime = 0.0f;		// 시간 초기화


	desc.MinColor = data.MinColor;
	desc.MaxColor = data.MaxColor;

	desc.Gravity = data.Gravity;
	desc.EndVelocity = data.EndVelocity;

	desc.RotateSpeed = Vector2(data.MinRotateSpeed, data.MaxRotateSpeed);
	desc.StartSize = Vector2(data.MinStartSize, data.MaxStartSize);
	desc.EndSize = Vector2(data.MinEndSize, data.MaxEndSize);

	desc.ReadyTime = data.ReadyTime;
	desc.ReadyRandomTime = data.ReadyRandomTime;
}

void Particle::MapVertices()
{
	if (leadCount == gpuCount) return; //차이가 없음

	D3D11_MAPPED_SUBRESOURCE subResource;

	if (leadCount > gpuCount)
	{
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		{
			UINT start = gpuCount * 4;
			UINT size = (leadCount - gpuCount) * sizeof(VertexParticle) * 4;
			UINT offset = gpuCount * sizeof(VertexParticle) * 4;

			BYTE* p = (BYTE *)subResource.pData + offset;
			memcpy(p, vertices + start, size);
		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}
	else
	{
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		{
			UINT start = gpuCount * 4;
			UINT size = (data.MaxParticles - gpuCount) * sizeof(VertexParticle) * 4;
			UINT offset = gpuCount * sizeof(VertexParticle) * 4;

			BYTE* p = (BYTE *)subResource.pData + offset;
			memcpy(p, vertices + start, size);

			if (leadCount > 0)
			{
				UINT size = leadCount * sizeof(VertexParticle) * 4;

				memcpy(subResource.pData, vertices, size);
			}
		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}
	gpuCount = leadCount;
}

void Particle::Activation()
{
	while (activeCount != gpuCount)
	{
		float age = currentTime - vertices[activeCount * 4].Time;
		if (age < data.ReadyTime)
			return; // 대기시간에 도달하지 못함

		vertices[activeCount * 4].Time = currentTime; // 활성화 된 시간
		activeCount++;

		if (activeCount >= data.MaxParticles)
			activeCount = (data.bLoop == true) ? 0 : data.MaxParticles;
	}
}

void Particle::Deactivation()
{
	while (deactiveCount != activeCount)
	{
		float age = currentTime - vertices[activeCount * 4].Time;
		if (age > data.ReadyTime)
			return;

		deactiveCount++;

		if (deactiveCount >= data.MaxParticles)
			deactiveCount = (data.bLoop == true) ? 0 : data.MaxParticles;
	}
}

void Particle::GetParticleFile(std::wstring _path)
{
	ReadFile(_path);

	buffer = Debug_new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Particle");

	sMap = shader->AsSRV("ParticleMap");


	Reset();

}

void Particle::Render()
{
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Render();

		if (indexBuffer != NULL)
			indexBuffer->Render();
	}

	D3D::GetDC()->IASetPrimitiveTopology(topology);
	perFrame->Render();
	transform->Render();

	desc.CurrentTime = currentTime;

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->GetBuffer());

	sMap->SetResource(map->SRV());


	UINT pass = (UINT)data.Type;
	if (activeCount != gpuCount)
	{
		if (gpuCount > activeCount)
		{
			shader->DrawIndexed(0, pass, (gpuCount - activeCount) * 6, activeCount * 6);
		}
		else
		{
			shader->DrawIndexed(0, pass, (data.MaxParticles - activeCount) * 6, activeCount * 6);

			if (gpuCount > 0)
				shader->DrawIndexed(0, pass, gpuCount * 6);
		}
	}

}

void Particle::ReadFile(std::wstring file)
{
	Xml::XMLDocument* document = Debug_new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();

	Xml::XMLElement* node = root->FirstChildElement();
	data.Type = (ParticleData::BlendType)node->IntText();

	node = node->NextSiblingElement();
	data.bLoop = node->BoolText();

	node = node->NextSiblingElement();
	std::wstring textureFile = String::ToWString(node->GetText());
	data.TextureFile = L"Particles/" + textureFile;
	map = Debug_new Texture(data.TextureFile);


	node = node->NextSiblingElement();
	data.MaxParticles = node->IntText();

	node = node->NextSiblingElement();
	data.ReadyTime = node->FloatText();

	node = node->NextSiblingElement();
	data.ReadyRandomTime = node->FloatText();

	node = node->NextSiblingElement();
	data.StartVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.EndVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.Gravity.x = node->FloatAttribute("X");
	data.Gravity.y = node->FloatAttribute("Y");
	data.Gravity.z = node->FloatAttribute("Z");

	node = node->NextSiblingElement();
	data.MinColor.r = node->FloatAttribute("R");
	data.MinColor.g = node->FloatAttribute("G");
	data.MinColor.b = node->FloatAttribute("B");
	data.MinColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MaxColor.r = node->FloatAttribute("R");
	data.MaxColor.g = node->FloatAttribute("G");
	data.MaxColor.b = node->FloatAttribute("B");
	data.MaxColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MinRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MinStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MinEndSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxEndSize = node->FloatText();

	SAFE_DELETE(document);
}

void Particle::SetTexture(std::wstring file, bool bInTextureFolder)
{
	SAFE_DELETE(map);

	map = Debug_new Texture(file, bInTextureFolder);
}