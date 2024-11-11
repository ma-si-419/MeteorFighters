#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
	//次の攻撃の入力判定をはじめるフレーム
	constexpr float kNextAttackInputTime = 5.0f;
	//格闘攻撃の攻撃持続フレーム
	constexpr int kPhysicalAttackLifeTime = 2;
	//格闘攻撃の判定の大きさ
	constexpr float kPhysicalAttackRadius = 5.0f;
	//格闘攻撃で最低限離れる距離
	constexpr float kPhysicalAttackNearLange = 8.0f;
	//瞬間移動攻撃でずらす大きさ
	constexpr float kTeleportationShiftLange = 10.0f;
	//気弾攻撃の攻撃判定が残る時間
	constexpr int kEnergyAttackLifeTime = 240;
	//気弾攻撃の判定の大きさ
	constexpr float kEnergyAttackRadius = 3.0f;
	//派生攻撃のスティックの傾き
	constexpr int kPhysicalAttackStickPower = 500;
	//ため攻撃を行っているときの経過時間の経過速度
	constexpr float kOnChargeStateTimeCountSpeed = 0.1f;
	//ため攻撃を行っているときのアニメ再生速度
	constexpr float kOnChargeAnimPlaySpeed = 0.1f;
	//ため攻撃の貯められるマックスのフレーム数
	constexpr float kChargeMaxTime = kOnChargeStateTimeCountSpeed * 40;
	//上入力の強攻撃
	const std::string kUpperAttackName = "Upper";
	//中入力の強攻撃
	const std::string kStanAttackName = "Stan";
	//下入力の強攻撃
	const std::string kLegSweepAttackName = "LegSweep";
	//上入力のため攻撃
	const std::string kUpChargeAttack = "UpCharge";
	//中入力のため攻撃
	const std::string kMiddleChargeAttack = "MiddleCharge";
	//下入力のため攻撃
	const std::string kDownChargeAttack = "DownCharge";
	//気弾のため攻撃
	const std::string kEnergyChargeAttack = "EnergyCharge";
	//瞬間移動する攻撃
	const std::string kTeleportationAttack = "Teleportation";
}

PlayerStateNormalAttack::PlayerStateNormalAttack(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_nowAttackName("empty"),
	m_nextAttackName("empty"),
	m_isNextAttack(false),
	m_isCharge(false),
	m_attackKey("empty"),
	m_chargeTime(0.0f),
	m_isAttacked(false),
	m_isNextCharge(false)
{
}
void PlayerStateNormalAttack::SetAttack(std::string key, bool isCharge)
{
	//押されたキーを保存しておく
	m_attackKey = key;

	//チャージされていた場合
	if (isCharge)
	{
		//格闘攻撃ならば派生する
		if (key == "X")
		{
			//上入力の場合
			if (MyEngine::Input::GetInstance().GetStickInfo().leftStickY < -kPhysicalAttackStickPower)
			{
				m_nowAttackName = kUpChargeAttack;
			}
			//下入力の場合
			else if (MyEngine::Input::GetInstance().GetStickInfo().leftStickY > kPhysicalAttackStickPower)
			{
				m_nowAttackName = kDownChargeAttack;
			}
			//入力なしの場合
			else
			{
				m_nowAttackName = kMiddleChargeAttack;
			}

		}
		//気弾攻撃なら派生しない
		else if (key == "Y")
		{
			m_nowAttackName = kEnergyChargeAttack;
		}
		m_isCharge = true;
	}
	else
	{
		if (key == "X")
		{
			m_nowAttackName = "Low1";
		}
		else if (key == "Y")
		{
			m_nowAttackName = "Energy1";
		}
		m_isCharge = false;
	}

}
void PlayerStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kNormalAttack;

	//設定された攻撃のアニメーション取得
	std::string animName = m_pPlayer->GetNormalAttackData(m_nowAttackName).animationName;

	CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(animName));

	//アニメーションの変更
	m_pPlayer->ChangeAnim(anim, false);

	//向かう方向の設定
	MyEngine::Vector3 shiftVec = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

	shiftVec *= kPhysicalAttackNearLange;

	shiftVec.y = 0;

	m_moveTargetPos = GetEnemyPos() + shiftVec;

	m_pPlayer->LookTarget(true);

}

void PlayerStateNormalAttack::Update()
{
	//チャージ中はフレームを数える速度を変える
	if (m_isCharge)
	{
		m_time += kOnChargeStateTimeCountSpeed;
		//どのくらいチャージしたかを数える
		m_chargeTime += kOnChargeStateTimeCountSpeed;
		//マックスまで貯めたらチャージをやめる
		if (m_time > kChargeMaxTime)
		{
			m_isCharge = false;
		}
		//アニメーションの再生速度も変える
		m_pPlayer->SetAnimPlaySpeed(kOnChargeAnimPlaySpeed);
	}
	//チャージしていない状態
	else
	{
		//Stateにいるフレーム数を数えておく
		m_time++;
		//アニメーションの再生速度を通常にする
		m_pPlayer->SetAnimPlaySpeed();
	}

	//ボタンを押している間はチャージを続ける
	if (m_isCharge)
	{
		//ボタンが離されたらチャージをやめる
		if (!MyEngine::Input::GetInstance().IsPress(m_attackKey))
		{
			m_isCharge = false;
		}
	}

	auto attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

	MyEngine::Vector3 velo;



	//攻撃の合計フレームを超えたら
	if (m_time >= attackData.totalFrame)
	{
		//アイドル状態に戻る
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}

	//次の攻撃に移行できるフレームであれば
	if (m_time >= attackData.cancelFrame)
	{
		//次に攻撃を行うと決定していれば
		if (m_isNextAttack)
		{
			//次の攻撃がない攻撃だったら
			if (m_nextAttackName == "None") return;
			//時間のリセット
			m_time = 0;
			//攻撃を行っていない状態に変化
			m_isAttacked = false;
			//次に行う攻撃の設定
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";

			//次の攻撃がチャージできるかどうか
			m_isCharge = m_isNextCharge;
			m_isNextCharge = false;

			//攻撃情報の更新
			attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

			//瞬間移動する攻撃であれば瞬間移動する
			if (attackData.isTeleportation)
			{
				m_pPlayer->StartFastCameraMove();

				//次の攻撃発生フレーム時に敵がいる場所を計算する
				MyEngine::Vector3 teleportationPos = GetEnemyPos() + (GetEnemyVelo() * (attackData.attackFrame));
				//瞬間移動先に攻撃の攻撃範囲分だけずれを足す
				MyEngine::Vector3 attackShiftVec = GetEnemyVelo();

				teleportationPos += attackShiftVec.Normalize() * (kTeleportationShiftLange);

				attackShiftVec.y = 0;

				teleportationPos += attackShiftVec.Normalize() * (kPhysicalAttackRadius);

				SetPlayerPos(teleportationPos);
			}

			//攻撃を行う方向を設定する
			LocalPos attackPos;

			//ローカル座標の中心を設定
			attackPos.SetCenterPos(GetEnemyPos());
			//ローカル座標の正面座標を設定
			attackPos.SetFrontPos(GetEnemyPos() + (m_pPlayer->GetPos() - GetEnemyPos()).Normalize());
			//ローカル座標を設定
			attackPos.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, kPhysicalAttackRadius));

			MyEngine::Vector3 shiftVec = (attackPos.GetWorldPos() - m_pPlayer->GetPos()).Normalize();

			shiftVec *= kPhysicalAttackNearLange;

			shiftVec.y = 0;

			m_moveTargetPos = GetEnemyPos() + shiftVec;
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(attackData.animationName));

			m_pPlayer->ChangeAnim(anim, false);

			m_isNextAttack = false;

			m_pPlayer->LookTarget(true);
		}
	}

	//格闘攻撃ならば
	if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
	{
		//攻撃を出すフレームまでは移動する
		if (m_time <= attackData.attackFrame)
		{

			MyEngine::Vector3 dir;

			dir = (m_moveTargetPos - m_pPlayer->GetPos()).Normalize();

			//プレイヤーの移動速度
			float speed = attackData.moveSpeed;

			//チャージ中であれば
			if (m_isCharge)
			{
				//移動速度をアニメーションの再生速度と同じ倍率にする
				speed *= kOnChargeAnimPlaySpeed;
			}

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
	if (m_time > attackData.attackFrame &&
		!m_isAttacked)
	{
		m_isAttacked = true;
		CharacterBase::AttackData attack;


		//チャージした時のダメージ
		if (m_chargeTime > 0.0f)
		{
			//チャージした時間によってダメージを上昇させる
			attack.damage = static_cast<int>((attackData.damageRate + m_chargeTime) * m_pPlayer->GetPower());
		}
		//チャージされていないときのダメージ
		else
		{
			attack.damage = static_cast<int>(attackData.damageRate * m_pPlayer->GetPower());
		}

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

		//攻撃を作成
		m_pPlayer->CreateAttack(attack);
	}

	//次の攻撃を行うか判定する
	if (!m_isNextAttack &&
		m_time > kNextAttackInputTime)
	{
		//格闘攻撃なら
		if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			//Xボタンで次の通常攻撃に移行する
			if (MyEngine::Input::GetInstance().IsTrigger("X"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
				m_attackKey = "X";
			}
			//Yボタンで派生攻撃を出す
			else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
			{
				//Yボタンの派生技は基本チャージができる
				m_isNextCharge = true;
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
				//敵が吹っ飛び状態の時に派生攻撃をしていたら
				CharacterBase::HitReactionKind kind = static_cast<CharacterBase::HitReactionKind>(GetEnemyHitReaction());
				if (kind == CharacterBase::HitReactionKind::kUpBurst ||
					kind == CharacterBase::HitReactionKind::kFarBurst ||
					kind == CharacterBase::HitReactionKind::kDownBurst)
				{
					m_nextAttackName = kTeleportationAttack;
					//瞬間移動攻撃だけチャージができない
					m_isNextCharge = false;
				}
				m_isNextAttack = true;
				m_attackKey = "Y";
			}
		}
		//気弾攻撃なら
		if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			//Yボタンで次の攻撃に移行する
			if (MyEngine::Input::GetInstance().IsRelease("Y"))
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