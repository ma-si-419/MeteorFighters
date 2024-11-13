#pragma once
#include "PlayerStateBase.h"
#include "LocalPos.h"
class PlayerStateRush : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateRush>
{
public:
	PlayerStateRush(std::shared_ptr<Player> player);

	virtual void SetMoveDir(MyEngine::Vector3 dir);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	//移動方向にあるベクトル(ここに向かって移動をしようとする)
	LocalPos m_moveTarget;

	//移動方向ベクトル
	MyEngine::Vector3 m_moveDir;

	//敵の後ろに向かって突撃するかどうか
	bool m_isRushEnemy;

	//ドラゴンダッシュの目的座標
	MyEngine::Vector3 m_rushTargetPos;

	//ドラゴンダッシュが終わったかどうか
	bool m_isEndRush;

	//ドラゴンダッシュが終わって何フレーム立ったか
	int m_rushEndTime;

	//攻撃入力されたかどうか
	bool m_isAttackInput;
};

