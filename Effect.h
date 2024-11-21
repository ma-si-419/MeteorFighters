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
		kCharge,
		kDash,
		kDashEnd,
		kEnergy
	};

public:

	/// <summary>
	/// 何のエフェクトを再生するかを設定する
	/// </summary>
	/// <param name="kind">エフェクトの種類</param>
	Effect(EffectKind kind);

	/// <summary>
	/// エフェクトの座標などを設定し、マネージャーにエフェクトを登録する
	/// </summary>
	/// <param name="manager">マネージャーのポインタ</param>
	/// <param name="pos">エフェクトの座標</param>
	void Init(std::shared_ptr<EffectManager> manager, MyEngine::Vector3 pos);

	/// <summary>
	/// 再生時間などを更新する
	/// </summary>
	void Update();

	/// <summary>
	/// エフェクトの再生をやめるタイミングで呼ぶ
	/// </summary>
	void End();

	/// <summary>
	/// エフェクトのパスを返す
	/// </summary>
	/// <returns>エフェクトのパス</returns>
	std::string GetPath() const { return m_path; }

	/// <summary>
	/// ハンドルを設定する
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void SetHandle(int handle) { m_handle = handle; }

	/// <summary>
	/// ハンドルを取得する
	/// </summary>
	/// <returns>ハンドル</returns>
	int GetHandle() {return m_handle; }

	/// <summary>
	/// 現在の座標を取得する
	/// </summary>
	/// <returns></returns>
	MyEngine::Vector3 GetPos() { return m_pos; }

	/// <summary>
	/// 現在の座標を設定する
	/// </summary>
	/// <param name="pos">設定したい座標</param>
	void SetPos(MyEngine::Vector3 pos) { m_pos = pos; }

	/// <summary>
	/// ループの設定をする
	/// </summary>
	/// <param name="time">ループする時間</param>
	void SetLoop(int loopStart,int loopEnd);

	/// <summary>
	/// ループをしない時に呼ぶ(エフェクトを変更した後などに使用)
	/// </summary>
	void SetLoop();

	/// <summary>
	/// ループの終了フレームになったかを取得する(ループしないものはfalseを返す)
	/// </summary>
	/// <returns>ループするフレームに来たかどうか</returns>
	bool IsEndLoop();
	
	/// <summary>
	/// ループのスタートフレームを取得する
	/// </summary>
	/// <returns>ループのスタート</returns>
	int GetLoopStartFrame() { return m_loopStartTime; }

	/// <summary>
	/// ループの時間をリセットする
	/// </summary>
	void ResetLoop() { m_time = m_loopStartTime; }

	/// <summary>
	/// 回転を設定する
	/// </summary>
	/// <param name="rotation">設定したい回転ベクトル</param>
	void SetRotation(MyEngine::Vector3 rotation) { m_rotation = rotation; }
	

	/// <summary>
	/// 回転を取得する
	/// </summary>
	/// <returns>回転ベクトル</returns>
	MyEngine::Vector3 GetRotation() { return m_rotation; }

private:

	int m_handle;

	std::string m_path;

	MyEngine::Vector3 m_pos;

	MyEngine::Vector3 m_rotation;

	int m_time;

	int m_loopStartTime;
	
	int m_loopEndTime;
};

