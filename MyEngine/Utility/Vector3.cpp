#include "Vector3.h"
#include <cmath>
#include <cassert>
using namespace MyEngine;

float Vector3::Length() const
{
	float ans;

	float X = x * x;
	float Y = y * y;
	float Z = z * z;

	ans = X + Y + Z;

	ans = sqrtf(ans);
	return ans;
}

float Vector3::sqLength() const
{
	float ans;

	float X = x * x;
	float Y = y * y;
	float Z = z * z;

	ans = X + Y + Z;

	return ans;
}

Vector3 Vector3::Normalize()
{
	Vector3 ans;

	float len = Length();

	if (len != 0)
	{
		ans.x = x / len;
		ans.y = y / len;
		ans.z = z / len;
	}
	//0èúéZÇµÇÊÇ§Ç∆ÇµÇΩÇÁé~ÇﬂÇÈ
	else
	{
		assert(false);
	}

	return ans;
}

float Vector3::Dot(Vector3 right)
{
	float ans;

	ans = x * right.x + y * right.y + z * right.z;

	return ans;
}

Vector3 Vector3::Cross(Vector3 right)
{
	Vector3 ans;
	ans.x = y * right.z - z * right.y;
	ans.y = z * right.x - x * right.z;
	ans.z = x * right.y - y * right.x;


	return ans;
}

Vector3 Vector3::MatTransform(MATRIX& right)
{
	Vector3 ans;
	ans.x = x * right.m[0][0] + y * right.m[1][0] + z * right.m[2][0] + right.m[3][0];
	ans.y = x * right.m[0][1] + y * right.m[1][1] + z * right.m[2][1] + right.m[3][1];
	ans.z = x * right.m[0][2] + y * right.m[1][2] + z * right.m[2][2] + right.m[3][2];
	return ans;
}

Vector3 Vector3::operator+(Vector3 right)const
{
	Vector3 ans;

	ans.x = x + right.x;
	ans.y = y + right.y;
	ans.z = z + right.z;

	return ans;
}

Vector3 Vector3::operator+=(Vector3 right)
{
	x = x + right.x;
	y = y + right.y;
	z = z + right.z;

	return *this;
}

Vector3 Vector3::operator-(Vector3 right)const
{
	Vector3 ans;

	ans.x = x - right.x;
	ans.y = y - right.y;
	ans.z = z - right.z;

	return ans;
}

Vector3 Vector3::operator-=(Vector3 right)
{
	x = x - right.x;
	y = y - right.y;
	z = z - right.z;

	return *this;
}



Vector3 Vector3::operator*(float right) const
{
	Vector3 ans;

	ans.x = x * right;
	ans.y = y * right;
	ans.z = z * right;

	return ans;
}

Vector3 Vector3::operator*=(float right)
{
	x = x * right;
	y = y * right;
	z = z * right;

	return *this;
}

Vector3 Vector3::operator/(float right) const
{
	Vector3 ans;

	ans.x = x / right;
	ans.y = y / right;
	ans.z = z / right;

	return ans;
}

Vector3 Vector3::operator/=(float right)
{
	x = x / right;
	y = y / right;
	z = z / right;

	return *this;
}

Vector3 Vector3::operator=(VECTOR right)
{
	Vector3 ans;

	ans.x = right.x;
	ans.y = right.y;
	ans.z = right.z;

	return ans;
}