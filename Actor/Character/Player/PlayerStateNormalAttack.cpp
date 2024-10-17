#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

namespace
{
	//�i���U���̍U�������t���[��
	constexpr int kNormalAttackLifeTime = 2;
	//�i���U���̔���̑傫��
	constexpr float kNormalAttackRadius = 4.0f;
}

PlayerStateNormalAttack::PlayerStateNormalAttack(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_nowAttackName("empty"),
	m_nextAttackName("empty"),
	m_isNextAttack(false)
{
}
void PlayerStateNormalAttack::SetAttack(std::string attackName)
{
	m_nowAttackName = attackName;
}
void PlayerStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kAttack;

	std::string animName = m_pPlayer->GetNormalAttackData(m_nowAttackName).animationName;

	CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAnimKind(animName));

	m_pPlayer->ChangeAnim(anim,false);

}

void PlayerStateNormalAttack::Update()
{
	auto attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

	//���̍U���Ɉڍs�ł���t���[���ł����
	if (m_time > attackData.cancelFrame)
	{
		//���ɍU�����s���ƌ��肵�Ă����
		if (m_isNextAttack)
		{
			//���Ԃ̃��Z�b�g
			m_time = 0;
			//���ɍs���U���̐ݒ�
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";
			//�U�����̍X�V
			attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);
			
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAnimKind(attackData.animationName));

			m_pPlayer->ChangeAnim(anim,false);

			m_isNextAttack = false;
		}
	}

	//�U���̍��v�t���[���𒴂�����
	if (m_time > attackData.totalFrame)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}

	//�U�����o���t���[���ɂȂ�����
	if (m_time == attackData.attackFrame)
	{
		CharacterBase::AttackData attack;

		attack.damage = attackData.damageRate * m_pPlayer->GetPower();
		attack.burstPower = attackData.burstPower;
		attack.hitDirection = attackData.hitDirection;
		attack.hitReaction = attackData.hitReaction;
		attack.isPlayer = true;
		attack.lifeTime = kNormalAttackLifeTime;
		attack.speed = 0;
		attack.radius = kNormalAttackRadius;

		m_pPlayer->CreateAttack(attack);
	}
	//���̍U�����s�������肷��
	if (!m_isNextAttack)
	{
		if (MyEngine::Input::GetInstance().IsTrigger("X"))
		{
			m_nextAttackName = attackData.nextLowComboName;
			m_isNextAttack = true;
		}
		if (MyEngine::Input::GetInstance().IsTrigger("Y"))
		{
			m_nextAttackName = attackData.nextHighComboName;
			m_isNextAttack = true;
		}
	}

	SetPlayerVelo(MyEngine::Vector3(0,0,0));


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Attack", GetColor(255, 255, 255));

#endif // _DEBUG

	//State�ɂ���t���[�����𐔂��Ă���
	m_time++;
}

void PlayerStateNormalAttack::Exit()
{
}

int PlayerStateNormalAttack::GetAnimKind(std::string animName)
{
	int ans = 0;

	if (animName == "LowAttack1")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack1);
	}
	else if (animName == "LowAttack2")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack2);
	}
	else if (animName == "LowAttack3")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack3);
	}
	else if (animName == "LowAttack4")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack4);
	}
	else if (animName == "LowAttack5")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack5);
	}
	else if (animName == "LowAttack6")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack6);
	}
	else if (animName == "LowAttack7")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack7);
	}
	else if (animName == "LowAttack8")
	{
		ans = static_cast<int>(CharacterBase::AnimKind::kLowAttack8);
	}


	return ans;
}