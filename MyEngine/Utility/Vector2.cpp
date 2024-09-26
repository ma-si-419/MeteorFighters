#include "Vector2.h"
#include <cmath>

using namespace MyEngine;

float Vector2::Length() const
{
	float ans;

	float X = x * x;
	float Y = y * y;

	ans = X + Y;

	ans = sqrtf(ans);
	return ans;
}

float Vector2::sqLength() const
{
	float ans;

	float X = x * x;
	float Y = y * y;

	ans = X + Y;

	return ans;
}

Vector2 Vector2::Normalize()
{
	Vector2 ans;

	float len = Length();

	ans.x = x / len;
	ans.y = y / len;

	return ans;
}

Vector2 MyEngine::Vector2::MatTransform(MATRIX& right)
{
	Vector2 ans;
	ans.x = x * right.m[0][0] + y * right.m[1][0] + right.m[2][0];
	ans.y = x * right.m[0][1] + y * right.m[1][1] + right.m[2][1];
	return ans;
}

Vector2 Vector2::operator+(Vector2 right) const
{
	Vector2 ans;

	ans.x = x + right.x;
	ans.y = y + right.y;

	return ans;
}

Vector2 Vector2::operator+=(Vector2 right)
{
	x = x + right.x;
	y = y + right.y;

	return *this;
}

Vector2 Vector2::operator-(Vector2 right) const
{
	Vector2 ans;

	ans.x = x - right.x;
	ans.y = y - right.y;

	return ans;
}

Vector2 Vector2::operator-=(Vector2 right)
{
	x = x - right.x;
	y = y - right.y;

	return *this;
}

Vector2 Vector2::operator*(float right) const
{
	Vector2 ans;

	ans.x = x * right;
	ans.y = y * right;

	return ans;
}

Vector2 Vector2::operator*=(float right)
{
	x = x * right;
	y = y * right;

	return *this;
}

Vector2 Vector2::operator/(float right) const
{
	Vector2 ans;

	ans.x = x / right;
	ans.y = y / right;

	return ans;
}

Vector2 Vector2::operator/=(float right)
{
	x = x / right;
	y = y / right;

	return *this;
}

Vector2 MyEngine::Vector2::operator=(Vector2 right)
{
	x = right.x;
	y = right.y;
	return *this;
}
