#pragma once
class Math
{
public:
	static const float PI;
	static const float EPSILON;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);
	
	static Vector2 RandomVec2(float r1, float r2);
	static Vector3 RandomVec3(float r1, float r2);
	static Color RandomColor3();
	static Color RandomColor4();

	static float GetVec2Angle(Vector2 v1, Vector2 v2);
	static float GetVec3Angle(Vector3 v1, Vector3 v2);

	static void LerpMatrix(OUT Matrix& out, const Matrix& m1, const Matrix& m2, float amount);

	static Quaternion LookAt(const Vector3& origin, const Vector3& target, const Vector3& up);
	static float Gaussian(float val, UINT blurCount);

	static void MatrixDecompose(const Matrix& m, OUT Vector3& S, OUT Vector3& R, OUT Vector3& T);

	template<typename T> static T Clamp(T value, T min, T max)
	{
		value = value > max ? max : value;
		value = value < min ? min : value;

		return value;
	}

	template <typename T>
	static constexpr T Max(const T& lhs, const T& rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	template <typename T>
	static constexpr T Min(const T& lhs, const T& rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}

	template<typename T> static T Lerp(T value1, T value2, T t)
	{
		return value1 + (value2 - value1) * t;
	}

};