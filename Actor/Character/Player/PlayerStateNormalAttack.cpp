#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

namespace
{
	//格闘攻撃の攻撃持続フレーム
	constexpr int kNormalAttackLifeTime = 2;
	//格闘攻撃の判定の大きさ
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

	//次の攻撃に移行できるフレームであれば
	if (m_time > attackData.cancelFrame)
	{
		//次に攻撃を行うと決定していれば
		if (m_isNextAttack)
		{
			//時間のリセット
			m_time = 0;
			//次に行う攻撃の設定
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";
			//攻撃情報の更新
			attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);
			
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAnimKind(attackData.animationName));

			m_pPlayer->ChangeAnim(anim,false);

			m_isNextAttack = false;
		}
	}

	//攻撃の合計フレームを超えたら
	if (m_time > attackData.totalFrame)
	{
		//アイドル状態に戻る
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}

	//攻撃を出すフレームになったら
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
	//次の攻撃を行うか判定する
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

	//Stateにいるフレーム数を数えておく
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