#pragma once
#include "DxLib.h"
namespace MyEngine
{
	struct Matrix
	{
	public:
		Matrix();

		float Dot(const Matrix& mat, int line, int row) const;

		Matrix Mul(Matrix mat);

		MATRIX CastMATRIX();

		float m[4][4];
	};
}

