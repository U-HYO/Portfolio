#pragma once

class BinaryWriter
{
public:
	BinaryWriter(std::wstring filePath);
	~BinaryWriter();

	void Bool(bool data);
	void Word(WORD data);
	void Int(int data);
	void UInt(UINT data);
	void Float(float data);
	void Double(double data);

	void Vector2(const D3DXVECTOR2& data);
	void Vector3(const D3DXVECTOR3& data);
	void Vector4(const D3DXVECTOR4& data);
	void Color3f(const D3DXCOLOR& data);
	void Color4f(const D3DXCOLOR& data);
	void Matrix(const D3DXMATRIX& data);

	void String(const std::string& data);
	void Byte(void* data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};

//////////////////////////////////////////////////////////////////////////

class BinaryReader
{
public:
	BinaryReader(std::wstring filePath);
	~BinaryReader();

	bool Bool();
	WORD Word();
	int Int();
	UINT UInt();
	float Float();
	double Double();

	D3DXVECTOR2 Vector2();
	D3DXVECTOR3 Vector3();
	D3DXVECTOR4 Vector4();
	D3DXCOLOR Color3f();
	D3DXCOLOR Color4f();
	D3DXMATRIX Matrix();

	std::string String();
	void Byte(void** data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;

	std::vector<char*> charMange;
};