#pragma once
#include "Vector3.h"
#include <memory>
#include <string>
class EffectManager;
class Effect : public std::enable_shared_from_this<Effect>
{
public:
	enum class EffectKind
	{
		kCharge
	};

public:

	/// <summary>
	/// 何のエフェクトを再生するかを設定する
	/// </summary>
	/// <param name="kind">エフェクトの種類</param>
	Effect(EffectKind kind);

	/// <summary>
	/// エフェクトを
	/// </summary>
	/// <param name="manager">マネージャーのポインタ</param>
	/// <param name="pos">エフェクトの座標</param>
	void Init(std::shared_ptr<EffectManager> manager, MyEngine::Vector3 pos);

	/// <summary>
	/// エフェクトの再生をやめるタイミングで呼ぶ
	/// </summary>
	void End();

	/// <summary>
	/// エフェクトのパスを返す
	/// </summary>
	/// <returns>エフェクトのパス</returns>
	std::string GetPath() { return m_path; }

	/// <summary>
	/// ハンドルを設定する
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void SetHandle(int handle) { m_handle = handle; }
private:

	int m_handle;

	std::string m_path;

	MyEngine::Vector3 m_pos;
};

