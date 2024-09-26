#pragma once
#include "DxLib.h"

namespace MyEngine
{
	struct Vector2
	{
		float x, y;

		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2(int X, int Y)
		{
			x = static_cast<float>(X);
			y = static_cast<float>(Y);
		}
		Vector2(float X, float Y)
		{
			x = X;
			y = Y;
		}

		Vector2(VECTOR Vec)
		{
			x = Vec.x;
			y = Vec.y;
		}

		/// <summary>
		/// ベクトルの長さを取得
		/// </summary>
		/// <returns>ベクトルの長さ</returns>
		float Length() const;

		/// <summary>
		/// ベクトルの長さの2乗を取得
		/// </summary>
		/// <returns>ベクトルの長さの2乗</returns>
		float sqLength() const;
		/// <summary>
		/// 正規化したベクトルを返す
		/// </summary>
		/// <returns>正規化したベクトル</returns>
		Vector2 Normalize();

		Vector2 MatTransform(MATRIX& right);

		/*演算子オーバーロード*/
		Vector2 operator+(Vector2 right)const;
		Vector2 operator+=(Vector2 right);
		Vector2 operator-(Vector2 right)const;
		Vector2 operator-=(Vector2 right);
		Vector2 operator*(float right)const;
		Vector2 operator*=(float right);
		Vector2 operator/(float right)const;
		Vector2 operator/=(float right);
		Vector2 operator=(Vector2 right);

	};
}