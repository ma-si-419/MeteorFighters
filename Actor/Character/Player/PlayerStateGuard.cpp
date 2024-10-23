#include "PlayerStateGuard.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

namespace
{
	//攻撃を回避できる時間
	constexpr float kDodgeTime = 15;
	//アニメーションブレンドの速さ
	constexpr float kBlendSpeed = 1.0f / kDodgeTime;
}

PlayerStateGuard::PlayerStateGuard(std::shared_ptr<Player> player):
	PlayerStateBase(player)
{
}

void PlayerStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kGuard,true,kBlendSpeed);
}

void PlayerStateGuard::Update()
{
	//ガードボタンが押されている間このStateにいる
	if (!MyEngine::Input::GetInstance().IsPress("B"))
	{
		//ガードボタンが離されたら
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Guard", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateGuard::Exit()
{
}

void PlayerStateGuard::OnCollide(std::shared_ptr<Collidable> collider)
{
}
