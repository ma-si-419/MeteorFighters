#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
	//格闘攻撃の攻撃持続フレーム
	constexpr int kPhysicalAttackLifeTime = 2;
	//格闘攻撃の判定の大きさ
	constexpr float kPhysicalAttackRadius = 5.0f;
	//気弾攻撃の攻撃判定が残る時間
	constexpr int kEnergyAttackLifeTime = 240;
	//気弾攻撃の判定の大きさ
	constexpr float kEnergyAttackRadius = 3.0f;
	//派生攻撃のスティックの傾き
	constexpr int kPhysicalAttackStickPower = 500;
	//上入力の強攻撃
	const std::string kUpperAttackName = "Upper";
	//中入力の強攻撃
	const std::string kStanAttackName = "Stan";
	//下入力の強攻撃
	const std::string kLegSweepAttackName = "LegSweep";
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

	CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(animName));

	m_pPlayer->ChangeAnim(anim, false);

	MyEngine::Vector3 shiftVec = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

	shiftVec *= kPhysicalAttackRadius;

	shiftVec.y = 0;

	m_moveTargetPos = GetEnemyPos() + shiftVec;

}

void PlayerStateNormalAttack::Update()
{
	//Stateにいるフレーム数を数えておく
	m_time++;

	auto attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

	MyEngine::Vector3 velo;

	//次の攻撃に移行できるフレームであれば
	if (m_time >= attackData.cancelFrame)
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

			//瞬間移動する攻撃であれば瞬間移動する
			if (attackData.isTeleportation)
			{
				//次の攻撃発生フレーム時に敵がいる場所を計算する
				MyEngine::Vector3 teleportationPos = GetEnemyPos() + (GetEnemyVelo() * (attackData.attackFrame));
				//瞬間移動先に攻撃の攻撃範囲分だけずれを足す
				MyEngine::Vector3 attackShiftVec = GetEnemyVelo();
				//attackShiftVec.y = 0;

				teleportationPos += attackShiftVec.Normalize() * (kPhysicalAttackRadius);

				SetPlayerPos(teleportationPos);
			}

			//攻撃を行う方向を設定する
			LocalPos attackPos;

			attackPos.SetCenterPos(GetEnemyPos());

			attackPos.SetFrontPos(GetEnemyPos() + (m_pPlayer->GetPos() - GetEnemyPos()).Normalize());

			attackPos.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, kPhysicalAttackRadius));

			MyEngine::Vector3 shiftVec = (attackPos.GetWorldPos() - m_pPlayer->GetPos()).Normalize();

			shiftVec *= kPhysicalAttackRadius;

			shiftVec.y = 0;

			m_moveTargetPos = GetEnemyPos() + shiftVec;
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(attackData.animationName));

			m_pPlayer->ChangeAnim(anim, false);

			m_isNextAttack = false;
		}
	}


	//攻撃の合計フレームを超えたら
	if (m_time >= attackData.totalFrame)
	{
		//アイドル状態に戻る
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}

	//格闘攻撃ならば
	if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
	{
		//攻撃を出すフレームまでは移動する
		if (m_time <= attackData.attackFrame)
		{

			MyEngine::Vector3 dir;

			dir = (m_moveTargetPos - m_pPlayer->GetPos()).Normalize();

			float speed = attackData.moveSpeed;

			//移動距離が行きたい座標までの距離よりも長ければ
			if ((m_moveTargetPos - m_pPlayer->GetPos()).Length() < speed * attackData.attackFrame)
			{
				//移動距離を補正する
				speed = (m_moveTargetPos - m_pPlayer->GetPos()).Length() / attackData.attackFrame;
			}

			velo = dir * speed;
		}
	}
	//気弾攻撃ならば
	else if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
	{
		//自由に移動できるようにする
		MyEngine::Input& input = MyEngine::Input::GetInstance();
		//移動方向ベクトル
		MyEngine::Vector3 dir;

		//スティックの情報取得
		MyEngine::Input::StickInfo stick = input.GetStickInfo();

		//左スティックの傾き取得
		MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
		//移動ベクトルが0じゃなければ
		if (leftStickDir.SqLength() > 0.001)
		{
			//移動方向
			dir = leftStickDir.Normalize();

			//エネミーの方向に移動方向を回転させる
			float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
			float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

			float angle = std::atan2f(vX, vZ);

			MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			dir = dir.MatTransform(mat);

			//移動方向にスピードをかける
			velo = dir * GetSpeed();
		}
		//ジャンプボタンが押されたら
		if (input.IsPress("RB"))
		{
			velo.y = GetSpeed();
		}
		//下降ボタンが押されたら
		else if (input.IsPushTrigger(true))
		{
			velo.y = -GetSpeed();
		}
	}

	//攻撃を出すフレームになったら
	if (m_time == attackData.attackFrame)
	{
		CharacterBase::AttackData attack;

		attack.damage = static_cast<int>(attackData.damageRate * m_pPlayer->GetPower());
		attack.attackHitKind = attackData.attackHitKind;
		attack.isPlayer = true;
		attack.speed = attackData.attackMoveSpeed;

		//格闘攻撃なら
		if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			attack.lifeTime = kPhysicalAttackLifeTime;
			attack.radius = kPhysicalAttackRadius;
		}
		//気弾攻撃なら
		else if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			attack.lifeTime = kEnergyAttackLifeTime;
			attack.radius = kEnergyAttackRadius;
		}

		m_pPlayer->CreateAttack(attack);
	}

	//次の攻撃を行うか判定する
	if (!m_isNextAttack)
	{
		//格闘攻撃なら
		if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			//Xボタンで次の通常攻撃に移行する
			if (MyEngine::Input::GetInstance().IsTrigger("X"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
			}
			//Yボタンで派生攻撃を出す
			else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
			{
				//上入力しながらの派生攻撃
				if (MyEngine::Input::GetInstance().GetStickInfo().leftStickY < -kPhysicalAttackStickPower)
				{
					m_nextAttackName = kUpperAttackName;
				}
				else if (MyEngine::Input::GetInstance().GetStickInfo().leftStickY > kPhysicalAttackStickPower)
				{
					m_nextAttackName = kLegSweepAttackName;
				}
				else
				{
					m_nextAttackName = kStanAttackName;
				}

				m_isNextAttack = true;
			}
		}
		//気弾攻撃なら
		if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			//Yボタンで次の攻撃に移行する
			if (MyEngine::Input::GetInstance().IsTrigger("Y"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
			}
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