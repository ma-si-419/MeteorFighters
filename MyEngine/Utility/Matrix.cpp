#include "Matrix.h"
using namespace MyEngine;

Matrix::Matrix() :
	m{ 0 }
{
}

float Matrix::Dot(const Matrix& mat2, int line, int row) const
{
	float ans = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		float result = m[row][i] * mat2.m[i][line];
		ans += result;
	}
	return ans;
}

Matrix Matrix::Mul(Matrix mat)
{
	Matrix ans;
	for (int row = 0; row < 4; row++)
	{
		for (int line = 0; line < 4; line++)
		{
			ans.m[row][line] = Dot(mat, line, row);
		}
	}
	return ans;
}

MATRIX MyEngine::Matrix::CastMATRIX()
{
	MATRIX ans;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ans.m[i][j] = m[i][j];
		}
	}

	return ans;
}
