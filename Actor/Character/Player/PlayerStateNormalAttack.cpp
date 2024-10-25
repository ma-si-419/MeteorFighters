#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

namespace
{
	//Ši“¬UŒ‚‚ÌUŒ‚‘±ƒtƒŒ[ƒ€
	constexpr int kNormalAttackLifeTime = 2;
	//Ši“¬UŒ‚‚Ì”»’è‚Ì‘å‚«‚³
	constexpr float kNormalAttackRadius = 5.0f;
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

	m_pPlayer->ChangeAnim(anim, false);

	MyEngine::Vector3 shiftVec = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

	shiftVec *= kNormalAttackRadius;

	shiftVec.y = 0;

	m_moveDir = ((GetEnemyPos() + shiftVec) - m_pPlayer->GetPos()).Normalize();

}

void PlayerStateNormalAttack::Update()
{
	//State‚É‚¢‚éƒtƒŒ[ƒ€”‚ğ”‚¦‚Ä‚¨‚­
	m_time++;

	auto attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

	MyEngine::Vector3 velo;

	//Ÿ‚ÌUŒ‚‚ÉˆÚs‚Å‚«‚éƒtƒŒ[ƒ€‚Å‚ ‚ê‚Î
	if (m_time >= attackData.cancelFrame)
	{
		//Ÿ‚ÉUŒ‚‚ğs‚¤‚ÆŒˆ’è‚µ‚Ä‚¢‚ê‚Î
		if (m_isNextAttack)
		{
			//ŠÔ‚ÌƒŠƒZƒbƒg
			m_time = 0;
			//Ÿ‚És‚¤UŒ‚‚Ìİ’è
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";

			//UŒ‚î•ñ‚ÌXV
			attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

			//uŠÔˆÚ“®‚·‚éUŒ‚‚Å‚ ‚ê‚ÎuŠÔˆÚ“®‚·‚é
			if (attackData.isTeleportation)
			{
				//Ÿ‚ÌUŒ‚”­¶ƒtƒŒ[ƒ€‚É“G‚ª‚¢‚éêŠ‚ğŒvZ‚·‚é
				MyEngine::Vector3 teleportationPos = GetEnemyPos() + (GetEnemyVelo() * (attackData.attackFrame - 1));
				//uŠÔˆÚ“®æ‚ÉUŒ‚‚ÌUŒ‚”ÍˆÍ•ª‚¾‚¯‚¸‚ê‚ğ‘«‚·
				MyEngine::Vector3 attackShiftVec = GetEnemyVelo();
				attackShiftVec.y = 0;
				
				teleportationPos += attackShiftVec.Normalize() * (kNormalAttackRadius);

				SetPlayerPos(teleportationPos);
			}

			//UŒ‚‚ğs‚¤•ûŒü‚ğİ’è‚·‚é
			MyEngine::Vector3 shiftVec = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

			shiftVec *= kNormalAttackRadius;

			shiftVec.y = 0;

			m_moveDir = ((GetEnemyPos() + shiftVec) - m_pPlayer->GetPos()).Normalize();
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAnimKind(attackData.animationName));

			m_pPlayer->ChangeAnim(anim, false);

			m_isNextAttack = false;
		}
	}


	//UŒ‚‚Ì‡ŒvƒtƒŒ[ƒ€‚ğ’´‚¦‚½‚ç
	if (m_time >= attackData.totalFrame)
	{
		//ƒAƒCƒhƒ‹ó‘Ô‚É–ß‚é
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}

	//UŒ‚‚ğo‚·ƒtƒŒ[ƒ€‚Ü‚Å‚ÍˆÚ“®‚·‚é
	if (m_time <= attackData.attackFrame)
	{
		velo = m_moveDir * attackData.moveSpeed;
	}

	//UŒ‚‚ğo‚·ƒtƒŒ[ƒ€‚É‚È‚Á‚½‚ç
	if (m_time == attackData.attackFrame)
	{
		CharacterBase::AttackData attack;

		attack.damage = static_cast<int>(attackData.damageRate * m_pPlayer->GetPower());
		attack.attackHitKind = attackData.attackHitKind;
		attack.isPlayer = true;
		attack.lifeTime = kNormalAttackLifeTime;
		attack.speed = 0;
		attack.radius = kNormalAttackRadius;

		m_pPlayer->CreateAttack(attack);
	}

	//Ÿ‚ÌUŒ‚‚ğs‚¤‚©”»’è‚·‚é
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

	SetPlayerVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Attack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void PlayerStateNormalAttack::Exit()
{
}

void PlayerStateNormalAttack::OnCollide(std::shared_ptr<Collidable> collider)
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