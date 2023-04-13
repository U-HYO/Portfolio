#include "Framework.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::EPSILON = 0.000001f;

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	//float temp = (int)((degree * PI / 180.0f) * 100) % 314;
	//temp /= 100.0f;
	//값이 0~3.14로 제한
	return degree * PI / 180.0f;
	//return temp;
}

float Math::ToDegree(float radian)
{
	float temp = (int)((radian * 180.0f / PI) * 10) % 3600;
	temp /= 10.0f;
	//값이 0~180.0에서 움직이도록 제한
	// 소숫점을 늘리고 싶으면 10조절
	return temp;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;

	return r1 + val;
}

Vector2 Math::RandomVec2(float r1, float r2)
{
	Vector2 result;
	result.x = Random(r1, r2);
	result.y = Random(r1, r2);

	return result;
}

Vector3 Math::RandomVec3(float r1, float r2)
{
	Vector3 result;
	result.x = Random(r1, r2);
	result.y = Random(r1, r2);
	result.z = Random(r1, r2);

	return result;
}

Color Math::RandomColor3()
{
	Color result;
	result.r = Math::Random(0.0f, 1.0f);
	result.g = Math::Random(0.0f, 1.0f);
	result.b = Math::Random(0.0f, 1.0f);
	result.a = 1.0f;

	return result;
}

Color Math::RandomColor4()
{
	Color result;
	result.r = Math::Random(0.0f, 1.0f);
	result.g = Math::Random(0.0f, 1.0f);
	result.b = Math::Random(0.0f, 1.0f);
	result.a = Math::Random(0.0f, 1.0f);

	return result;
}

float Math::GetVec2Angle(Vector2 v1, Vector2 v2)
{
	Vector2 test1;
	Vector2 test2;
	D3DXVec2Normalize(&test1, &v1);
	D3DXVec2Normalize(&test2, &v2);

	return acosf(D3DXVec2Dot(&test1, &test2));
}

float Math::GetVec3Angle(Vector3 v1, Vector3 v2)
{
	Vector3 test1;
	Vector3 test2;
	D3DXVec3Normalize(&test1, &v1);
	D3DXVec3Normalize(&test2, &v2);

	return acosf(D3DXVec3Dot(&test1, &test2));
}

void Math::LerpMatrix(OUT Matrix & out, const Matrix & m1, const Matrix & m2, float amount)
{
	out._11 = m1._11 + (m2._11 - m1._11) * amount;
	out._12 = m1._12 + (m2._12 - m1._12) * amount;
	out._13 = m1._13 + (m2._13 - m1._13) * amount;
	out._14 = m1._14 + (m2._14 - m1._14) * amount;

	out._21 = m1._21 + (m2._21 - m1._21) * amount;
	out._22 = m1._22 + (m2._22 - m1._22) * amount;
	out._23 = m1._23 + (m2._23 - m1._23) * amount;
	out._24 = m1._24 + (m2._24 - m1._24) * amount;

	out._31 = m1._31 + (m2._31 - m1._31) * amount;
	out._32 = m1._32 + (m2._32 - m1._32) * amount;
	out._33 = m1._33 + (m2._33 - m1._33) * amount;
	out._34 = m1._34 + (m2._34 - m1._34) * amount;

	out._41 = m1._41 + (m2._41 - m1._41) * amount;
	out._42 = m1._42 + (m2._42 - m1._42) * amount;
	out._43 = m1._43 + (m2._43 - m1._43) * amount;
	out._44 = m1._44 + (m2._44 - m1._44) * amount;
}

Quaternion Math::LookAt(const Vector3 & origin, const Vector3 & target, const Vector3 & up)
{
	Vector3 f = (origin - target);
	D3DXVec3Normalize(&f, &f);

	Vector3 s;
	D3DXVec3Cross(&s, &up, &f);
	D3DXVec3Normalize(&s, &s);

	Vector3 u;
	D3DXVec3Cross(&u, &f, &s);

	float z = 1.0f + s.x + u.y + f.z;
	float fd = 2.0f * sqrtf(z);

	Quaternion result;

	if (z > Math::EPSILON)
	{
		result.w = 0.25f * fd;
		result.x = (f.y - u.z) / fd;
		result.y = (s.z - f.x) / fd;
		result.z = (u.x - s.y) / fd;
	}
	else if (s.x > u.y && s.x > f.z)
	{
		fd = 2.0f * sqrtf(1.0f + s.x - u.y - f.z);
		result.w = (f.y - u.z) / fd;
		result.x = 0.25f * fd;
		result.y = (u.x + s.y) / fd;
		result.z = (s.z + f.x) / fd;
	}
	else if (u.y > f.z)
	{
		fd = 2.0f * sqrtf(1.0f + u.y - s.x - f.z);
		result.w = (s.z - f.x) / fd;
		result.x = (u.x - s.y) / fd;
		result.y = 0.25f * fd;
		result.z = (f.y + u.z) / fd;
	}
	else
	{
		fd = 2.0f * sqrtf(1.0f + f.z - s.x - u.y);
		result.w = (u.x - s.y) / fd;
		result.x = (s.z + f.x) / fd;
		result.y = (f.y + u.z) / fd;
		result.z = 0.25f * fd;
	}

	return result;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

float Math::Gaussian(float val, UINT blurCount)
{
	float a = 1.0f / sqrtf(2 * PI * (float)blurCount * (float)blurCount);
	float c = 2.0f * (float)blurCount * (float)blurCount;
	float b = exp(-(val * val) / c);

	return a * b;
}

void Math::MatrixDecompose(const Matrix & m, OUT Vector3 & S, OUT Vector3 & R, OUT Vector3 & T)
{
	Quaternion rotation;
	D3DXMatrixDecompose(&S, &rotation, &T, &m);

	Matrix temp;
	D3DXMatrixRotationQuaternion(&temp, &rotation);

	R.x = asin(-temp._32);
	R.y = atan2(temp._31, temp._33);
	R.z = atan2(temp._12, temp._22);
}
