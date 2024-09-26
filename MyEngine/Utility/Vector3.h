#pragma once
#include "DxLib.h"

namespace MyEngine
{
	struct Vector3
	{
		float x, y, z;

		Vector3()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		Vector3(int X, int Y, int Z)
		{
			x = static_cast<float>(X);
			y = static_cast<float>(Y);
			z = static_cast<float>(Z);
		}
		Vector3(float X, float Y, float Z)
		{
			x = X;
			y = Y;
			z = Z;
		}
		Vector3(VECTOR vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}
		/// <summary>
		/// DxLibのVECTORにキャスト
		/// </summary>
		/// <returns>VECTOR型のデータ</returns>
		VECTOR CastVECTOR()
		{
			VECTOR retVec;

			retVec.x = x;
			retVec.y = y;
			retVec.z = z;

			return retVec;
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
		Vector3 Normalize();
		/// <summary>
		/// 2つベクトルの内積を出す
		/// </summary>
		/// <param name="right">計算するベクトル</param>
		/// <returns>ベクトルの内積</returns>
		float Dot(Vector3 right);
		/// <summary>
		/// 2つのベクトルの外積を出す
		/// </summary>
		/// <param name="right">計算するベクトル</param>
		/// <returns>ベクトルの外積</returns>
		Vector3 Cross(Vector3 right);
		/// <summary>
		/// 引数で受け取った回転行列を使用して平行移動
		/// </summary>
		/// <param name="right">回転行列</param>
		/// <returns>回転行列の向いている方向を正面とした移動ベクトル</returns>
		Vector3 MatTransform(MATRIX& right);


		/*演算子オーバーロード*/
		Vector3 operator+(Vector3 right)const;
		Vector3 operator+=(Vector3 right);
		Vector3 operator-(Vector3 right)const;
		Vector3 operator-=(Vector3 right);
		Vector3 operator*(float right)const;
		Vector3 operator*=(float right);
		Vector3 operator/(float right)const;
		Vector3 operator/=(float right);
		Vector3 operator=(VECTOR right);

	};
}


