#include "Vector3.h"
#include "Matrix.h"
#include <cmath>
#include <cassert>
using namespace MyEngine;

VECTOR MyEngine::Vector3::CastVECTOR()
{
	VECTOR ans;

	ans.x = x;
	ans.y = y;
	ans.z = z;

	return ans;
}

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

float Vector3::SqLength() const
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
		//assert(false);
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

MATRIX MyEngine::Vector3::GetRotationMat()
{

	Matrix rotX;
	rotX.m[0][0] = 1; rotX.m[0][1] = 0;                    rotX.m[0][2] = 0;				   rotX.m[0][3] = 0;
	rotX.m[1][0] = 0; rotX.m[1][1] = std::cosf(x);		   rotX.m[1][2] = std::sinf(x);		   rotX.m[1][3] = 0;
	rotX.m[2][0] = 0; rotX.m[2][1] = -std::sinf(x);		   rotX.m[2][2] = std::cosf(x);		   rotX.m[2][3] = 0;
	rotX.m[3][0] = 0; rotX.m[3][1] = 0;                    rotX.m[3][2] = 0;                   rotX.m[3][3] = 1;

	Matrix rotY;
	rotY.m[0][0] = std::cosf(y); rotY.m[0][1] = 0; rotY.m[0][2] = -std::sinf(y); rotY.m[0][3] = 0;
	rotY.m[1][0] = 0;			 rotY.m[1][1] = 1; rotY.m[1][2] = 0;  		     rotY.m[1][3] = 0;
	rotY.m[2][0] = std::sinf(y); rotY.m[2][1] = 0; rotY.m[2][2] = std::cosf(y);  rotY.m[2][3] = 0;
	rotY.m[3][0] = 0;	  		 rotY.m[3][1] = 0; rotY.m[3][2] = 0;             rotY.m[3][3] = 1;

	Matrix rotZ;
	rotZ.m[0][0] = std::cosf(z);  rotZ.m[0][1] = std::sinf(z); rotZ.m[0][2] = 0; rotZ.m[0][3] = 0;
	rotZ.m[1][0] = -std::sinf(z); rotZ.m[1][1] = std::cosf(z); rotZ.m[1][2] = 0; rotZ.m[1][3] = 0;
	rotZ.m[2][0] = 0;             rotZ.m[2][1] = 0;			   rotZ.m[2][2] = 1; rotZ.m[2][3] = 0;
	rotZ.m[3][0] = 0; 			  rotZ.m[3][1] = 0;		  	   rotZ.m[3][2] = 0; rotZ.m[3][3] = 1;

	//âÒì]ÇçáÇÌÇπÇΩçsóÒ
	Matrix rotMat;
	//XÇÃçsóÒÇ∆YÇÃçsóÒÇèÊéZÇ∑ÇÈ
	rotMat = rotX.Mul(rotY);
	//èÊéZÇµÇΩÇ‡ÇÃÇ∆ZÇÃçsóÒÇèÊéZÇ∑ÇÈ
	rotMat = rotMat.Mul(rotZ);
	
	MATRIX ans = rotMat.CastMATRIX();

	return ans;
}

Vector3 MyEngine::Vector3::operator-() const
{
	return Vector3(-x,-y,-z);
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