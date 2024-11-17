#pragma once
#include "Vector3.h"

namespace MyEngine
{

	struct LocalPos
	{

		LocalPos();

		/// <summary>
		/// ローカル座標をワールド座標に変換
		/// </summary>
		/// <returns>ワールド座標</returns>
		MyEngine::Vector3 GetWorldPos();

		/// <summary>
		/// 中心座標を取得する
		/// </summary>
		/// <returns>中心としている座標</returns>
		MyEngine::Vector3 GetCenterPos() { return m_centerPos; };

		/// <summary>
		/// 中心とする座標を設定
		/// </summary>
		/// <param name="pos">中心座標(ワールド座標)</param>
		void SetCenterPos(MyEngine::Vector3 pos);

		/// <summary>
		/// ローカル座標を設定
		/// </summary>
		/// <param name="pos">中心からのベクトル</param>
		void SetLocalPos(MyEngine::Vector3 pos);

		/// <summary>
		/// 設定しているローカル座標を取得する
		/// </summary>
		/// <returns>ローカル座標</returns>
		MyEngine::Vector3 GetLocalPos() { return m_localPos; }

		/// <summary>
		/// 中心の向いている方向を設定する
		/// </summary>
		/// <param name="frontPos">中心座標の前方向とする座標</param>
		void SetFrontPos(MyEngine::Vector3 frontPos);

		/// <summary>
		/// ワールド座標をローカル座標に変換
		/// </summary>
		/// <param name="worldPos">変換したいワールド座標</param>
		/// <returns>変換後のローカル座標</returns>
		MyEngine::Vector3 ChangeWorldToLocal(MyEngine::Vector3 worldPos);

	private:

		MyEngine::Vector3 m_centerPos;
		MyEngine::Vector3 m_centerRotation;

		MyEngine::Vector3 m_localPos;

	};

}