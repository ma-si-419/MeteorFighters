#include "PlayerStateGuard.h"
#include "DxLib.h"
#include "Input.h"


PlayerStateGuard::PlayerStateGuard(std::shared_ptr<Player> player):
	PlayerStateBase(player)
{
}

void PlayerStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
}

void PlayerStateGuard::Update()
{
	//�K�[�h�{�^����������Ă���Ԃ���State�ɂ���
	if (!MyEngine::Input::GetInstance().IsPress("B"))
	{
		
		//�K�[�h�{�^���������ꂽ��
		ChangeState(CharacterStateKind::kIdle);

	}


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Guard", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateGuard::Exit()
{
}
