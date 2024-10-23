#include "PlayerStateGuard.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

namespace
{
	//�U��������ł��鎞��
	constexpr float kDodgeTime = 15;
	//�A�j���[�V�����u�����h�̑���
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
	//�K�[�h�{�^����������Ă���Ԃ���State�ɂ���
	if (!MyEngine::Input::GetInstance().IsPress("B"))
	{
		//�K�[�h�{�^���������ꂽ��
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
