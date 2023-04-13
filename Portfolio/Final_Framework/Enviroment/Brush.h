#pragma once

class Brush
{
public:
	Brush(class Shader* shader, class Terrain* terrain);
	~Brush();

	void Update();
	void Render(); 

private:
	void RaiseHeight(float intensity);
	void PaintColor(Color color);
	void PaintBillboards();

	void Billborads();

private:
	struct BrushDesc
	{
		Color Color = D3DXCOLOR(0, 1, 0, 1);
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 2;

		Vector2 MousePos;
		Vector2 TextureSize;
		float Padding[3];
	}brushDesc;

	struct LineDesc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);
		UINT Visible = 1;
		float Thickness = 0.01f;
		float Size = 5.0f;
		float Padding;
	} lineDesc;

private:
	Shader* shader;
	Terrain* terrain;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* lineBuffer;
	ID3DX11EffectConstantBuffer* sLineBuffer;

	Shader* billboardShader;
	Billboard* billboard;

public:
	LineDesc GetLineDesc() { return lineDesc; }
	BrushDesc GetBrushDesc() { return brushDesc; }
	void SetLineDesc(LineDesc   _lineDesc) { lineDesc = _lineDesc; }
	void SetBrushDesc(BrushDesc _brushDesc) { brushDesc = _brushDesc;}

};

