#include "Character.h"
#include "SphereColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include "Input.h"
#include "EnemyInput.h"
#include <cassert>
#include <cmath>
#include "BattleManager.h"
#include "SoundManager.h"
#include "LocalPos.h"
#include "SceneGame.h"
#include "Game.h"
#include "GameSceneConstant.h"
#include "CharacterStateIdle.h"

namespace
{
	//キャラクターの身長の高さ
	constexpr float kCharacterHeight = 4.5f;
	//攻撃を発生させる座標
	constexpr float kAttackPopPos = GameSceneConstant::kCharacterRadius * 3.0f;
	//攻撃を上下に補正する際にどこまで補正するか
	constexpr float kAttackMaxShiftPosY = kCharacterHeight * 1.5f;
	//基本的なアニメーションブレンドの速さ
	constexpr float kAnimBlendSpeed = 0.08f;
	//アニメーションの再生速度
	constexpr float kAnimPlaySpeed = 1.0f;
	//向いている方向を設定する際に使用する定数
	constexpr float kFrontPosDistance = 3.0f;
	//1P側のキャラクターの初期座標
	const MyEngine::Vector3 kOnePlayerInitPos(-50, 150, -50);
	//2P側のキャラクターの初期座標
	const MyEngine::Vector3 kTwoPlayerInitPos(50, 150, 50);

	//敵が正面にいないときのターゲット座標までの距離
	constexpr float kTargetDistance = 30.0f;

	//ボイスの大きさ(255がMAX)
	constexpr int kVoiceVolume = 40;

	//攻撃の種類を外部ファイルの文字列から内部のAttackHitKindに変換する際に使用する
	const std::map<std::string, Character::AttackHitKind> kAttackHitKindMap =
	{
		{"弱",Character::AttackHitKind::kLow},
		{"中",Character::AttackHitKind::kMiddle},
		{"奥吹っ飛び",Character::AttackHitKind::kFarBurst},
		{"上吹っ飛び",Character::AttackHitKind::kUpBurst},
		{"下吹っ飛び",Character::AttackHitKind::kDownBurst},
		{"中スタン",Character::AttackHitKind::kMiddleStan},
		{"下スタン",Character::AttackHitKind::kBottomStan}
	};

	//必殺技の種類を外部ファイルの文字列から内部のAttackKindに変換する際に使用する
	const std::map<std::string, Character::AttackKind> kSpecialAttackKindMap =
	{
		{"ビーム",Character::AttackKind::kLaser},
		{"格闘",Character::AttackKind::kRush},
		{"気弾",Character::AttackKind::kEnergy},
		{"投げ",Character::AttackKind::kThrow},
		{"体当たり",Character::AttackKind::kAssault}
	};

	//キャラクターの声をVoiceKindから文字列に変換する際に使用する
	const std::map<Character::VoiceKind, std::string> kVoiceKindMap =
	{
		{Character::VoiceKind::kStart,"Start"},
		{Character::VoiceKind::kLowAttack,"LowAttack"},
		{Character::VoiceKind::kMiddleAttack,"MiddleAttack"},
		{Character::VoiceKind::kHighAttack,"HighAttack"},
		{Character::VoiceKind::kSpecialAttack,"SpecialAttack"},
		{Character::VoiceKind::kDodge1,"Dodge1"},
		{Character::VoiceKind::kDodge2,"Dodge2"},
		{Character::VoiceKind::kWinBashing,"WinBashing"}
	};

	//攻撃のアニメーションを外部ファイルの文字列から内部のAnimKindに変換する際に使用する
	const std::map<std::string, Character::AnimKind> kAttackAnimKindMap =
	{
		{"LowAttack1",Character::AnimKind::kLowAttack1},
		{"LowAttack2",Character::AnimKind::kLowAttack2},
		{"LowAttack3",Character::AnimKind::kLowAttack3},
		{"LowAttack4",Character::AnimKind::kLowAttack4},
		{"LowAttack5",Character::AnimKind::kLowAttack5},
		{"LowAttack6",Character::AnimKind::kLowAttack6},
		{"LowAttack7",Character::AnimKind::kLowAttack7},
		{"LowAttack8",Character::AnimKind::kLowAttack8},
		{"EnergyAttackRight",Character::AnimKind::kEnergyAttackRight},
		{"EnergyAttackLeft",Character::AnimKind::kEnergyAttackLeft},
		{"UpperAttack",Character::AnimKind::kUpperAttack},
		{"StanAttack",Character::AnimKind::kStanAttack},
		{"LegSweepAttack",Character::AnimKind::kLegSweepAttack},
		{"UpChargeAttack",Character::AnimKind::kUpChargeAttack},
		{"MiddleChargeAttack",Character::AnimKind::kMiddleChargeAttack},
		{"DownChargeAttack",Character::AnimKind::kDownChargeAttack},
		{"EnergyChargeAttack",Character::AnimKind::kEnergyChargeAttack},
		{"DashAttack",Character::AnimKind::kDashAttack}
	};
}

Character::Character(ObjectTag tag, CharacterKind kind) :
	Collidable(tag, ColliderData::Kind::kSphere),
	m_modelHandle(-1),
	m_attachAnim(-1),
	m_totalAnimTime(-1),
	m_nowPlayAnimTime(-1),
	m_animPlaySpeed(0),
	m_isLoop(false),
	m_characterKind(kind),
	m_lastAnim(-1),
	m_animBlendRate(1.0f),
	m_animBlendSpeed(kAnimBlendSpeed),
	m_isEndAnimationBlend(true),
	m_isEndAnim(false),
	m_modelPath("empty"),
	m_nowHitReaction(Character::HitReactionKind::kNone),
	m_isDrawCharacter(true)
{
	auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);

	sphereData->m_radius = GameSceneConstant::kCharacterRadius;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	SetNormalAttackData(data);

	m_playAnimKind = AnimKind::kIdle;
}

Character::~Character()
{
}

void Character::Init()
{

	//入力情報の設定
	if (m_playerNumber == Character::PlayerNumber::kOnePlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(0);
	}
	else if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(1);
	}

	std::string path;

	//2Pなら最初にEnemyを付ける
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		path = "Enemy";
	}

	//キャラクターの種類によってパスを変更する
	if (m_characterKind == CharacterKind::kMouse)
	{
		path += "Mouse";
	}
	else if (m_characterKind == CharacterKind::kTheBlue)
	{
		path += "BigB";
	}

	path = "data/model/" + path + ".mv1";

	m_modelPath = path;

	Collidable::Init();

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

	//初期ステートの設定
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();

	//初期座標の決定
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		m_rigidbody.SetPos(kOnePlayerInitPos);
		m_targetLocalPos.SetCenterPos(kOnePlayerInitPos);
		m_lookPos.SetCenterPos(kOnePlayerInitPos);
		SetFrontPos(kTwoPlayerInitPos);
	}
	else if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_rigidbody.SetPos(kTwoPlayerInitPos);
		m_targetLocalPos.SetCenterPos(kTwoPlayerInitPos);
		m_lookPos.SetCenterPos(kTwoPlayerInitPos);
		SetFrontPos(kOnePlayerInitPos);
	}

	//敵側の処理
	if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
	{
		//エネミーのステートを設定する
		m_pEnemyInput->SetEnemyState(m_pState);
		//プレイヤーのステートを設定する
		m_pEnemyInput->SetPlayerState(m_pBattleManager->GetOnePlayerPointer()->m_pState);
	}
}

void Character::SetEnemyInput(int level)
{
	m_pEnemyInput = std::make_shared<EnemyInput>(MyEngine::Input::GetInstance().GetInputData(1));

	m_pEnemyInput->SetGameManager(m_pBattleManager);

	m_pEnemyInput->SetAiLevel(static_cast<EnemyInput::AiLevel>(level));
}

void Character::Update()
{
	m_targetLocalPos.SetCenterPos(GetPos());

	//エネミーの入力情報を更新する
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->Update();

		//チュートリアルの時はMPが減らないようにする
		if (m_pBattleManager->GetGameKind() == GameManagerBase::GameKind::kTutorial)
		{
			m_nowMp = GameSceneConstant::kMaxMp;
		}
	}

	//常に更新するもの
	m_pState->UpdateCommon();

	//EnemyInputにプレイヤーのステートを設定する
	if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->SetPlayerState(m_pBattleManager->GetOnePlayerPointer()->m_pState);
	}

	//Stateが変更されていたらStateを入れ替える
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;

		//敵側の処理
		if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
		{
			m_pEnemyInput->SetEnemyState(m_pState);
		}
	}

	//状況によって変化するアップデート
	(this->*m_updateSituationFunc)();

	//ローカル座標の中心座標の更新
	m_lookPos.SetCenterPos(m_rigidbody.GetPos());

	//描画座標の設定
	SetDrawPos(m_rigidbody.GetPos());
}

void Character::Draw()
{
	//リザルト時に2P側を消す
	if (m_pBattleManager->GetNowSituation() == GameManagerBase::BattleSituation::kResult ||
		m_pBattleManager->GetNowSituation() == GameManagerBase::BattleSituation::kRetry)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer) return;
	}

	//残像の描画
	for (auto item : m_afterImageList)
	{
		MV1DrawModel(item.handle);
	}

	MyEngine::Vector3 pos = (GetFrontPos() - m_rigidbody.GetPos()).Normalize() * 10.0f + m_rigidbody.GetPos();

	//描画するとされていたら
	if (m_isDrawCharacter)
	{
		//モデルの描画
		MV1DrawModel(m_modelHandle);
	}
}

void Character::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_pState->OnCollide(collider);
}

void Character::Final()
{
	Collidable::Final();
	MV1DeleteModel(m_modelHandle);
	for (auto& item : m_afterImageList)
	{
		MV1DeleteModel(item.handle);
	}
}

void Character::RetryInit()
{
	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	m_knockOutVelo = MyEngine::Vector3();

	auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

	//初期座標の決定
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		m_rigidbody.SetPos(kOnePlayerInitPos);
		m_lookPos.SetCenterPos(kOnePlayerInitPos);
		SetFrontPos(kTwoPlayerInitPos);
	}
	else if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_rigidbody.SetPos(kTwoPlayerInitPos);
		m_lookPos.SetCenterPos(kTwoPlayerInitPos);
		SetFrontPos(kOnePlayerInitPos);
	}

	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));

	if (m_pState)
	{
		//Stateの終了処理
		m_pState->Exit();
	}
	//初期ステートの設定
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();
}

void Character::UpdateAfterImage()
{
	//残像を消す数
	int deleteNum = 0;

	for (auto& item : m_afterImageList)
	{
		item.nowOpacityRate -= item.DeleteSpeed;

		float rate = fmin(item.maxOpacityRate, item.nowOpacityRate);

		MV1SetOpacityRate(item.handle, rate);

		if (item.nowOpacityRate < 0.0f)
		{
			MV1DeleteModel(item.handle);

			deleteNum++;
		}
	}

	for (int i = 0; i < deleteNum; i++)
	{
		m_afterImageList.pop_front();
	}
}

void Character::SetGameManager(std::shared_ptr<GameManagerBase> manager)
{
	m_pBattleManager = manager;
}

MyEngine::Vector3 Character::GetPos()
{
	return m_rigidbody.GetPos();
}

MyEngine::Vector3 Character::GetVelo()
{
	return m_rigidbody.GetVelo();
}

bool Character::SubMp(float subMp)
{
	if (m_nowMp >= subMp)
	{
		m_nowMp -= subMp;
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void Character::ChargeMp()
{
	m_nowMp += m_status.chargeSpd;

	//最大を超えないようにクランプ
	m_nowMp = std::fmin(m_nowMp, GameSceneConstant::kMaxMp);
}

void Character::ChangeAnim(AnimKind animKind, bool loop)
{
	ChangeAnim(animKind, loop, kAnimBlendSpeed);
}

void Character::ChangeAnim(AnimKind animKind, bool loop, float blendSpeed)
{

	//ひとつ前のアニメーションをけしておく
	if (m_lastAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_lastAnim);
	}

	//アニメーション番号を設定
	int animNumber = static_cast<int>(animKind);
	//今再生していたアニメーションを保存する
	m_lastAnim = m_attachAnim;
	//今から再生するアニメーションを設定する
	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	//今から再生するアニメーションが何かを保存する
	m_playAnimKind = animKind;
	//アニメーションの合計再生時間を設定する
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	//さっきまで再生していたアニメーションの再生時間を保存する(ブレンドの際におかしくならないように)
	m_lastPlayAnimTime = m_nowPlayAnimTime;
	//再生時間をリセット
	m_nowPlayAnimTime = 0;
	//アニメーションの再生時間を設定
	m_animPlaySpeed = 1.0f;
	//アニメーションがループするかどうかを設定
	m_isLoop = loop;
	//アニメーションブレンド率のリセット
	m_animBlendRate = 0.0f;
	//アニメーションをブレンドする速さを設定
	m_animBlendSpeed = blendSpeed;
	//ブレンドが終わったかどうかをfalseにする
	m_isEndAnimationBlend = false;
	//アニメーションが終わったフラグをfalseにする
	m_isEndAnim = false;
}

void Character::PlayAnim()
{
	//アニメーションの再生フレームを設定
	MV1SetAttachAnimTime(m_modelHandle, m_lastAnim, m_lastPlayAnimTime);
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_nowPlayAnimTime);
	//再生中のアニメーションを進める
	m_nowPlayAnimTime += m_animPlaySpeed;
	//アニメーションが最後まで言った場合
	if (m_nowPlayAnimTime > m_totalAnimTime)
	{
		//ループするなら
		if (m_isLoop)
		{
			//再生時間をリセットする
			m_nowPlayAnimTime = 0;
		}
		//ループしないなら
		else
		{
			//アニメーションが終わったフラグをtrueにする
			m_isEndAnim = true;
		}
	}

	//ブレンド率の調整
	m_animBlendRate += m_animBlendSpeed;

	//アニメーションのブレンドが終わったら
	if (m_animBlendRate > 1.0f)
	{
		//変な値にならないようにする
		m_animBlendRate = 1.0f;
		//アニメーションブレンドが終わったフラグをtrueにする
		m_isEndAnimationBlend = true;
		//ひとつ前のアニメーションを消す
		m_lastPlayAnimTime = 0;
		MV1DetachAnim(m_modelHandle, m_lastAnim);
	}

	//アニメーションのブレンド
	MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnim, 1.0f - m_animBlendRate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_attachAnim, m_animBlendRate);
}

void Character::SetAnimPlaySpeed(float speed)
{
	m_animPlaySpeed = speed;
}

void Character::SetAnimPlaySpeed()
{
	m_animPlaySpeed = kAnimPlaySpeed;
}

bool Character::IsAnimEnd()
{
	return false;
}

Character::AttackKind Character::GetSpecialAttackKind(std::string kind)
{
	return kSpecialAttackKindMap.at(kind);
}

void Character::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto& item : normalAttackData)
	{
		NormalAttackData pushData;

		pushData.attackName = item[static_cast<int>(NormalAttackDataSort::kAttackName)];
		pushData.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
		pushData.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
		pushData.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
		pushData.cancelFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kCancelFrame)]);
		pushData.moveSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kMoveSpeed)]);
		pushData.attackMoveSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kAttackMoveSpeed)]);
		pushData.isTeleportation = static_cast<bool>(stoi(item[static_cast<int>(NormalAttackDataSort::kIsTeleportation)]));
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAnimationName)];
		pushData.attackKind = static_cast<AttackKind>(stoi(item[static_cast<int>(NormalAttackDataSort::kAttackKind)]));
		pushData.nextComboName = item[static_cast<int>(NormalAttackDataSort::kNextComboName)];
		pushData.targetHitReaction = item[static_cast<int>(NormalAttackDataSort::kTargetHitReaction)];
		pushData.attackHitKind = kAttackHitKindMap.at(item[static_cast<int>(NormalAttackDataSort::kAttackHitKind)]);
		pushData.animationSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kAnimationSpeed)]);
		pushData.effectName = item[static_cast<int>(NormalAttackDataSort::kEffectName)];
		pushData.soundName = item[static_cast<int>(NormalAttackDataSort::kSoundName)];

		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

std::shared_ptr<Attack> Character::CreateAttack(AttackData attackData)
{
	//攻撃クラス
	std::shared_ptr<Attack> ans;

	//攻撃のタグ
	ObjectTag tag;

	//プレイヤーとエネミーどちらの攻撃か判断
	if (attackData.isPlayer)
	{
		tag = ObjectTag::kOnePlayerAttack;
	}
	else
	{
		tag = ObjectTag::kTwoPlayerAttack;
	}
	//攻撃座標の設定
	MyEngine::Vector3 attackPos;

	//キャラクターの正面方向に攻撃を出す
	MyEngine::LocalPos localPos;

	//自身を中心とするローカル座標を作成
	localPos.SetCenterPos(m_rigidbody.GetPos());

	//正面方向を設定
	MyEngine::Vector3 toTarget;
	MyEngine::Vector3 targetPos;

	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		auto player1 = m_pBattleManager->GetOnePlayerPointer();

		targetPos = m_pBattleManager->GetTargetPos(player1);
		toTarget = targetPos - m_rigidbody.GetPos();
	}
	else
	{
		auto player2 = m_pBattleManager->GetTwoPlayerPointer();

		targetPos = m_pBattleManager->GetTargetPos(player2);
		toTarget = targetPos - m_rigidbody.GetPos();
	}

	//もし敵が正面にいたら
	if (IsFrontTarget())
	{
		localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget.Normalize());
	}
	//敵が正面にいない場合
	else
	{
		localPos.SetFrontPos(GetFrontPos());

		//ターゲット座標を自身の正面方向にする
		targetPos = (GetFrontPos() - m_rigidbody.GetPos()).Normalize() * kTargetDistance;
	}
	//どのくらいずらすかを設定
	MyEngine::Vector3 localAttackPos;

	localAttackPos.z = std::fmin(std::fabs(toTarget.z), kAttackPopPos);

	localPos.SetLocalPos(localAttackPos);

	//ローカル座標からワールド座標に変換
	attackPos = localPos.GetWorldPos();

	//y座標のずれを一定値までは補正
	float shiftY = toTarget.y;

	shiftY = std::fmin(shiftY, kAttackMaxShiftPosY);
	shiftY = std::fmax(shiftY, -kAttackMaxShiftPosY);

	attackPos.y += shiftY;

	//攻撃のポインタ作成
	ans = std::make_shared<Attack>(tag, attackPos);

	Attack::AttackStatus status;

	status.targetPos = targetPos;
	status.damage = attackData.damage;
	status.speed = attackData.speed;
	status.lifeTime = attackData.lifeTime;
	status.radius = attackData.radius;
	status.attackHitKind = attackData.attackHitKind;
	status.attackKind = attackData.attackKind;
	status.effectName = attackData.effectName;
	status.attackName = attackData.attackName;
	status.hitSoundName = attackData.hitSoundName;

	ans->Init(status, m_pBattleManager->GetEffectManagerPointer());

	m_pBattleManager->AddAttack(ans);

	return ans;
}

Character::NormalAttackData Character::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}

Character::SpecialAttackData Character::GetSpecialAttackData(int specialNumber)
{

	//一つ目の必殺技を取得する
	if (specialNumber == 1)
	{
		return m_status.firstSpecialAttackData;
	}
	//二つ目の必殺技を取得する
	else if (specialNumber == 2)
	{
		return m_status.secondSpecialAttackData;
	}

	//ここまで来たら止める
	assert(false);

	return SpecialAttackData();
}

void Character::SetFrontPos(MyEngine::Vector3 frontPos)
{
	//frontPosへの方向
	MyEngine::Vector3 dir = (frontPos - m_rigidbody.GetPos()).Normalize();

	MyEngine::Vector3 pos = (dir * kFrontPosDistance) + m_rigidbody.GetPos();

	//ローカル座標の前方向を修正
	m_targetLocalPos.SetFrontPos(pos);

	//前方向を保存する
	m_lookPos.SetLocalPos(m_lookPos.ChangeWorldToLocal(pos));

	//モデルの前方向を修正する
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - pos).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
}

MyEngine::Vector3 Character::GetFrontPos()
{
	return m_lookPos.GetWorldPos();
}

bool Character::IsFrontTarget()
{

	MyEngine::Vector3 toTargetDir;

	//プレイヤーなら
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		auto player1 = m_pBattleManager->GetOnePlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pBattleManager->GetTargetPos(player1));
	}
	//2P側なら(基本CPU)
	else
	{
		auto player2 = m_pBattleManager->GetTwoPlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pBattleManager->GetTargetPos(player2));
	}

	toTargetDir = toTargetDir.Normalize();

	if (toTargetDir.z > 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}

	//警告けしのため
	return true;

}

void Character::SetDrawPos(MyEngine::Vector3 pos)
{
	pos.y -= kCharacterHeight;

	pos += m_drawShiftVec;

	MV1SetPosition(m_modelHandle, pos.CastVECTOR());
}

Character::AnimKind Character::GetAttackAnimKind(std::string animName)
{
	return kAttackAnimKindMap.at(animName);
}

int Character::GetStateKind()
{
	return static_cast<int>(m_pState->GetKind());
}

void Character::LookTarget()
{
	auto pointer = std::dynamic_pointer_cast<Character>(shared_from_this());


	//ターゲットの方向を向く
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pBattleManager->GetTargetPos(pointer)).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	m_lookPos.SetLocalPos(m_lookPos.ChangeWorldToLocal(m_pBattleManager->GetTargetPos(pointer)));

	//正面方向を設定
	SetFrontPos(m_pBattleManager->GetTargetPos(pointer));
}

void Character::CreateAfterImage()
{
	AfterImage data;

	data.DeleteSpeed = GameSceneConstant::kAfterImageDeleteSpeed;

	data.nowOpacityRate = GameSceneConstant::kAfterImageInitOpacityRate;

	data.maxOpacityRate = GameSceneConstant::kAfterImageMaxOpacityRate;

	CreateAfterImage(data);
}

void Character::CreateAfterImage(AfterImage afterImageInfo)
{
	Character::AfterImage ans;

	//モデルをコピーしスケールや座標を設定する
	int handle = MV1DuplicateModel(m_modelHandle);
	MyEngine::Vector3 drawPos = MV1GetPosition(m_modelHandle);
	MV1SetScale(handle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));
	MV1SetPosition(handle, drawPos.CastVECTOR());
	MV1SetRotationZYAxis(handle, (m_rigidbody.GetPos() - m_lookPos.GetWorldPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);

	//アニメーションブレンドが終わっていなければ
	if (!m_isEndAnimationBlend)
	{
		MV1AttachAnim(handle, MV1GetAttachAnim(m_modelHandle, m_lastAnim));
		MV1SetAttachAnimTime(handle, m_lastAnim, m_lastPlayAnimTime);
		MV1SetAttachAnimBlendRate(handle, m_lastAnim, 1.0f - m_animBlendRate);
	}

	//アニメーションを設定する
	int anim = MV1AttachAnim(handle, MV1GetAttachAnim(m_modelHandle, m_attachAnim));
	MV1SetAttachAnimTime(handle, anim, m_nowPlayAnimTime);
	MV1SetAttachAnimBlendRate(handle, anim, m_animBlendRate);

	//残像の情報を設定
	ans.handle = handle;

	ans.nowOpacityRate = afterImageInfo.nowOpacityRate;

	ans.maxOpacityRate = afterImageInfo.maxOpacityRate;

	ans.DeleteSpeed = afterImageInfo.DeleteSpeed;

	//残像リストに登録
	m_afterImageList.push_back(ans);
}

void Character::StartFastCameraMove()
{
	m_pBattleManager->StartFastCameraMove();
}

MyEngine::Vector3 Character::GetBackPos(float distance)
{
	MyEngine::Vector3 toBackPos = m_rigidbody.GetPos() - m_lookPos.GetWorldPos();

	toBackPos = toBackPos.Normalize();

	MyEngine::Vector3 ans = m_rigidbody.GetPos() + (toBackPos * (distance + GameSceneConstant::kCharacterRadius));

	MyEngine::LocalPos local;

	local.SetCenterPos(m_rigidbody.GetPos());

	MyEngine::Vector3 pos = m_rigidbody.GetPos();

	return ans;
}

void Character::SetIsTrigger(bool flag)
{
	m_pColData->SetIsTrigger(flag);
}

void Character::ChangeSituationUpdate(int situation)
{
	auto sit = static_cast<GameManagerBase::BattleSituation>(situation);

	//リトライが押されたとき
	if (sit == GameManagerBase::BattleSituation::kRetry)
	{
		//アニメーションを止める
		m_updateSituationFunc = &Character::UpdateNone;
	}
	//1Pの開始演出をしているとき
	else if (sit == GameManagerBase::BattleSituation::kStart1P)
	{
		InitStart();
		if (m_playerNumber == PlayerNumber::kOnePlayer)
		{
			//1P側はスタート処理を行う
			m_updateSituationFunc = &Character::UpdateStart;
		}
		else
		{
			//2P側は何もしないように設定する
			m_updateSituationFunc = &Character::UpdateNone;
		}
	}
	//2Pの開始演出をしているとき
	else if (sit == GameManagerBase::BattleSituation::kStart2P)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer)
		{
			//2P側はスタート処理を行う
			m_updateSituationFunc = &Character::UpdateStart;
		}
		else
		{
			//1P側は何もしないように設定する
			m_updateSituationFunc = &Character::UpdateNone;
		}
	}
	//バトル時
	else if (sit == GameManagerBase::BattleSituation::kBattle)
	{
		m_updateSituationFunc = &Character::UpdateBattle;
	}
	//ノックアウトの演出時
	else if (sit == GameManagerBase::BattleSituation::kKnockOut)
	{
		m_updateSituationFunc = &Character::UpdateKnockOut;
	}
	//リザルト時
	else if (sit == GameManagerBase::BattleSituation::kResult)
	{
		m_updateSituationFunc = &Character::UpdateResult;
	}
	//メニューや一時停止を行っているとき
	else if (sit == GameManagerBase::BattleSituation::kMenu)
	{
		m_updateSituationFunc = &Character::UpdateNone;
	}
	//操作を受け付けないとき
	else if (sit == GameManagerBase::BattleSituation::kIdle)
	{
		m_updateSituationFunc = &Character::UpdateIdle;
	}
}

void Character::SetModelHandle(int handle)
{
	m_modelHandle = handle;

	MV1SetScale(m_modelHandle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));
}

MyEngine::Vector3 Character::GetNearStagePos()
{
	MyEngine::Vector3 pos = m_rigidbody.GetPos();
	pos.y = 0;
	return pos;
}

std::shared_ptr<EnemyInput> Character::GetEnemyInput()
{
	//2P側なら
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		return m_pEnemyInput;
	}

	//1Pの場合は何も返さない
	return std::shared_ptr<EnemyInput>();
}

void Character::PlayVoice(VoiceKind kind)
{
	//もしボイスが再生されていたら
	if (m_voiceHandle != -1)
	{
		//ボイスを止める
		StopSoundMem(m_voiceHandle);
	}

	//ボイスの名前
	std::string voiceName = kVoiceKindMap.at(kind);

	//キャラクターによってボイスのパスを変更する
	if (m_characterKind == CharacterKind::kMouse)
	{
		voiceName = "Mouse" + voiceName;
	}
	else if (m_characterKind == CharacterKind::kTheBlue)
	{
		voiceName = "Blue" + voiceName;
	}

	//ボイスの再生
	m_voiceHandle = SoundManager::GetInstance().PlayOnceSound(voiceName);

}

void Character::InitStart()
{
	//初期座標の決定
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		m_rigidbody.SetPos(kOnePlayerInitPos);
		m_lookPos.SetCenterPos(kOnePlayerInitPos);
		SetFrontPos(kTwoPlayerInitPos);
	}
	else if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_rigidbody.SetPos(kTwoPlayerInitPos);
		m_lookPos.SetCenterPos(kTwoPlayerInitPos);
		SetFrontPos(kOnePlayerInitPos);
	}
}

void Character::UpdateStart()
{
	//アニメーションを変更していなければ変更する
	if (m_playAnimKind != AnimKind::kStartPose)
	{
		ChangeAnim(AnimKind::kStartPose, false);
		//このタイミングで一緒に音声も再生する
		PlayVoice(VoiceKind::kStart);
	}
	PlayAnim();
}

void Character::UpdateBattle()
{
	//Stateの更新
	m_pState->Update();

	//残像の更新を行う
	UpdateAfterImage();

	//アニメーションの更新
	PlayAnim();

}

void Character::UpdateKnockOut()
{
	//移動ベクトルが設定されていなければ
	if (m_knockOutVelo.SqLength() < 0.01f)
	{
		//移動ベクトルを設定する
		m_knockOutVelo = m_rigidbody.GetVelo();
	}

	//移動を遅くする
	m_rigidbody.SetVelo(m_knockOutVelo * 0.1f);
	//アニメーションもゆっくり再生する
	SetAnimPlaySpeed(0.1f);

	//残像の更新を行う
	UpdateAfterImage();

	//アニメーションの更新
	PlayAnim();
}

void Character::UpdateResult()
{
	//移動をしないようにする
	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));

	//アニメーションの再生速度をリセットする
	SetAnimPlaySpeed();

	//残像の更新を行う
	UpdateAfterImage();

	//アニメーションを変更していなければ変更する
	if (m_nowHp > 0)
	{
		if (m_playAnimKind != AnimKind::kWinPose)
		{
			ChangeAnim(AnimKind::kWinPose, true);
			//アニメーションを変更するタイミングで正面方向も変更する
			auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pBattleManager->GetTargetPos(thisPointer);

			frontPos.y = m_rigidbody.GetPos().y;

			SetFrontPos(frontPos);
		}
	}
	else
	{
		if (m_playAnimKind != AnimKind::kLosePose)
		{
			ChangeAnim(AnimKind::kLosePose, true);
			//アニメーションを変更するタイミングで正面方向も変更する
			auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pBattleManager->GetTargetPos(thisPointer);

			frontPos.y = m_rigidbody.GetPos().y;

			SetFrontPos(frontPos);
		}
	}

	PlayAnim();
}

void Character::UpdateIdle()
{
	//移動をしないようにする
	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));

	//アニメーションの再生速度をリセットする
	SetAnimPlaySpeed();

	//残像の更新を行う
	UpdateAfterImage();

	//アニメーションを変更していなければ変更する
	if (m_playAnimKind != AnimKind::kSkyIdle)
	{
		ChangeAnim(AnimKind::kSkyIdle, true);
	}

	//アニメーションの更新を行う
	PlayAnim();
}

void Character::UpdateNone()
{
	//移動をしないようにする
	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));
}
