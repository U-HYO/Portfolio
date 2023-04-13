#include "Framework.h"
#include "Brush.h"

Brush::Brush(Shader * shader, Terrain * terrain)
	:shader(shader), terrain(terrain)
{
	brushBuffer = nullptr;
	lineBuffer = nullptr;

	brushBuffer = Debug_new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = this->shader->AsConstantBuffer("CB_TerrainBrush");

	lineBuffer = Debug_new ConstantBuffer(&lineDesc, sizeof(LineDesc));
	sLineBuffer = this->shader->AsConstantBuffer("CB_TerrainLine");


	Billborads();

}

Brush::~Brush()
{
	SAFE_DELETE(brushBuffer);
	SAFE_DELETE(lineBuffer);
	SAFE_DELETE(billboard);
	SAFE_DELETE(billboardShader);
}

void Brush::Update()
{
	//Matrix W = terrain->GetTransform()->GetWorld();
	//Matrix V = Context::Get()->View();
	//Matrix P = Context::Get()->Projection();
	//brushDesc.WVP = W * V * P;

	//Vector2 ndcMousePos = Vector2(
	//	(2.0 * Mouse::Get()->GetPosition().x / D3D::Width()) - 1.0,
	//	1.0 - (2.0 * Mouse::Get()->GetPosition().y / D3D::Height())
	//);
	//brushDesc.MousePos.x = Mouse::Get()->GetPosition().x;
	//brushDesc.MousePos.y = Mouse::Get()->GetPosition().y;
	//brushDesc.MousePos = ndcMousePos;
	//brushDesc.TextureSize.x = terrain->GetHeightMap()->GetWidth();
	//brushDesc.TextureSize.y = terrain->GetHeightMap()->GetHeight();

	if (brushDesc.Type > 0)
	{
		brushDesc.Location = terrain->GetRaycastPosition2();
		//brushDesc.Location = terrain->GetIntersectPos();
	
		if (brushDesc.Type == 1)
		{
			if (Mouse::Get()->Press(0))
				RaiseHeight(5);
		}
		if (brushDesc.Type == 2)
		{
			if (Mouse::Get()->Press(0))
				PaintBillboards();
		}
		//if (brushDesc.Type == 3)
		//{
		//	if (Mouse::Get()->Press(0))
		//		PaintColor(brushDesc.Color);
		//}
	}

	//PickingColc();


	billboard->Update();
}

void Brush::Render()
{
	//if (brushDesc.Type > 0)
	//{
	//	std::string str = "";
	//	str += std::to_string(brushDesc.Location.x);
	//	str += ", ";
	//	str += std::to_string(brushDesc.Location.y);
	//	str += ", ";
	//	str += std::to_string(brushDesc.Location.z);
	//
	//	Gui::Get()->RenderText(10, 65, 1, 0, 0, str);
	//}
	brushBuffer->Render();
	sBrushBuffer->SetConstantBuffer(brushBuffer->GetBuffer());

	lineBuffer->Render();
	sLineBuffer->SetConstantBuffer(lineBuffer->GetBuffer());

	billboard->Pass(3);
	billboard->Render();
}

void Brush::RaiseHeight(float intensity)
{
	UINT left = (UINT)brushDesc.Location.x - brushDesc.Range;
	UINT right = (UINT)brushDesc.Location.x + brushDesc.Range;
	UINT bottom = (UINT)brushDesc.Location.z - brushDesc.Range;
	UINT top = (UINT)brushDesc.Location.z + brushDesc.Range;


	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = terrain->GetWidth() * z + x;

			terrain->GetVertices()[index].Position.y += intensity * Time::Delta();
		}
	}

	terrain->CreateNormalData();
	terrain->UpdateVertexData();
}

void Brush::PaintColor(Color color)
{
	UINT left = (UINT)brushDesc.Location.x - brushDesc.Range;
	UINT right = (UINT)brushDesc.Location.x + brushDesc.Range;
	UINT bottom = (UINT)brushDesc.Location.z - brushDesc.Range;
	UINT top = (UINT)brushDesc.Location.z + brushDesc.Range;
	if (left < 0) left = 0;
	if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	if (bottom < 0) bottom = 0;
	if (top >= terrain->GetHeight()) top = terrain->GetHeight();

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = terrain->GetWidth() * z + x;
			
			terrain->GetVertices()[index].color = color;
		}
	}

	terrain->CreateNormalData();
	terrain->UpdateVertexData();

}

void Brush::PaintBillboards()
{
	//UINT left =		 (UINT)brushDesc.Location.x - brushDesc.Range;
	//UINT right =	 (UINT)brushDesc.Location.x + brushDesc.Range;
	//UINT bottom =	 (UINT)brushDesc.Location.z - brushDesc.Range;
	//UINT top =		 (UINT)brushDesc.Location.z + brushDesc.Range;
	//
	//
	//if (left < 0) left = 0;
	//if (right >= terrain->GetWidth()) right = terrain->GetWidth();
	//if (bottom < 0) bottom = 0;
	//if (top >= terrain->GetHeight()) top = terrain->GetHeight();
	//
	//for (UINT z = bottom; z <= top; z++)
	//{
	//	for (UINT x = left; x <= right; x++)
	//	{
	//		Vector2 scale = Math::RandomVec2(1, 5);
	//
	//		UINT index = terrain->GetWidth() * z + x;
	//		Vector3 pos = terrain->GetVertices()[index].Position;
	//
	//		//billboard->Add(terrain->GetVertices()[index].Position, scale, 1);
	//		billboard->Add(Vector3(pos.x, scale.y*0.5f, pos.y), scale, 1);
	//	}
	//}

	
	
	//Vector2 scale = Math::RandomVec2(1, 5);
	//
	//UINT index = terrain->GetWidth() * brushDesc.Location.z + brushDesc.Location.x;
	billboard->Add(Vector3(brushDesc.Location.x, brushDesc.Location.y + 5, brushDesc.Location.z), Vector2(10, 10), 1);
	//
	//terrain->CreateNormalData();
	//terrain->UpdateVertexData();

}

void Brush::Billborads()
{
	billboardShader = Debug_new Shader(L"42_Billboard.fxo");
	billboard = Debug_new Billboard(billboardShader);
	billboard->AddTexture(L"Terrain/grass_14.tga");
	billboard->AddTexture(L"Terrain/grass_07.tga");
	billboard->AddTexture(L"Terrain/grass_11.tga");
}
