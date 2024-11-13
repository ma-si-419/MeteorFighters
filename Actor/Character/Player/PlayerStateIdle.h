#pragma once
#include "PlayerStateBase.h"
class PlayerStateIdle : public PlayerStateBase , public std::enable_shared_from_this<PlayerStateIdle>
{
public:
	PlayerStateIdle(std::shared_ptr<Player> player);

	/// <summary>
	/// ステートの終了アニメーションを再生する(ブレンドの速さ固定)
	/// </summary>
	/// <param name="kind">再生するアニメーション番号</param>
	/// <param name="time">再生する時間</param>
	void SetEndAnim(int kind,int time);

	/// <summary>
	/// ステートの終了アニメーションを再生する(ブレンドの速さ可変)
	/// </summary>
	/// <param name="kind">再生するアニメーション番号</param>
	/// <param name="time">再生する時間</param>
	/// <param name="blendSpeed">アニメーションブレンドの速さ</param>
	void SetEndAnim(int kind,int time,float blendSpeed);

	virtual void Enter() override;

	virtual void Update() override;
	
	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	bool m_isPlayEndAnim;

	int m_endAnimTime;
};

