#pragma once
#include "CharacterStateBase.h"
class CharacterStateIdle : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateIdle>
{
public:
	CharacterStateIdle(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// ステートの終了アニメーションを再生する(ブレンドの速さ固定)
	/// </summary>
	/// <param name="kind">再生するアニメーション番号</param>
	/// <param name="time">再生する時間</param>
	void SetEndAnim(int kind, int time);

	/// <summary>
	/// ステートの終了アニメーションを再生する(ブレンドの速さ可変)
	/// </summary>
	/// <param name="kind">再生するアニメーション番号</param>
	/// <param name="time">再生する時間</param>
	/// <param name="blendSpeed">アニメーションブレンドの速さ</param>
	void SetEndAnim(int kind, int time, float blendSpeed);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	bool m_isPlayEndAnim;

	int m_endAnimTime;
};

