#include "CharacterBase.h"
#include "SphereColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include "Input.h"
#include "EnemyInput.h"
#include <cassert>
#include <cmath>
#include "GameManager.h"
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
	const MyEngine::Vector3 kOnePlayerInitPos(-50, 0, -50);
	//2P側のキャラクターの初期座標
	const MyEngine::Vector3 kTwoPlayerInitPos(50, 0, 50);

	//攻撃の種類を外部ファイルの文字列から内部のAttackHitKindに変換する際に使用する
	const std::map<std::string, CharacterBase::AttackHitKind> kAttackHitKindMap =
	{
		{"弱",CharacterBase::AttackHitKind::kLow},
		{"中",CharacterBase::AttackHitKind::kMiddle},
		{"奥吹っ飛び",CharacterBase::AttackHitKind::kFarBurst},
		{"上吹っ飛び",CharacterBase::AttackHitKind::kUpBurst},
		{"下吹っ飛び",CharacterBase::AttackHitKind::kDownBurst},
		{"中スタン",CharacterBase::AttackHitKind::kMiddleStan},
		{"下スタン",CharacterBase::AttackHitKind::kBottomStan}
	};

	//必殺技の種類を外部ファイルの文字列から内部のAttackKindに変換する際に使用する
	const std::map<std::string, CharacterBase::AttackKind> kSpecialAttackKindMap =
	{
		{"ビーム",CharacterBase::AttackKind::kBeam},
		{"格闘",CharacterBase::AttackKind::kRush},
		{"気弾",CharacterBase::AttackKind::kEnergy},
		{"投げ",CharacterBase::AttackKind::kThrow},
		{"体当たり",CharacterBase::AttackKind::kAssault}
	};


	//攻撃のアニメーションを外部ファイルの文字列から内部のAnimKindに変換する際に使用する
	const std::map<std::string, CharacterBase::AnimKind> kAttackAnimKindMap =
	{
		{"LowAttack1",CharacterBase::AnimKind::kLowAttack1},
		{"LowAttack2",CharacterBase::AnimKind::kLowAttack2},
		{"LowAttack3",CharacterBase::AnimKind::kLowAttack3},
		{"LowAttack4",CharacterBase::AnimKind::kLowAttack4},
		{"LowAttack5",CharacterBase::AnimKind::kLowAttack5},
		{"LowAttack6",CharacterBase::AnimKind::kLowAttack6},
		{"LowAttack7",CharacterBase::AnimKind::kLowAttack7},
		{"LowAttack8",CharacterBase::AnimKind::kLowAttack8},
		{"EnergyAttackRight",CharacterBase::AnimKind::kEnergyAttackRight},
		{"EnergyAttackLeft",CharacterBase::AnimKind::kEnergyAttackLeft},
		{"UpperAttack",CharacterBase::AnimKind::kUpperAttack},
		{"StanAttack",CharacterBase::AnimKind::kStanAttack},
		{"LegSweepAttack",CharacterBase::AnimKind::kLegSweepAttack},
		{"UpChargeAttack",CharacterBase::AnimKind::kUpChargeAttack},
		{"MiddleChargeAttack",CharacterBase::AnimKind::kMiddleChargeAttack},
		{"DownChargeAttack",CharacterBase::AnimKind::kDownChargeAttack},
		{"EnergyChargeAttack",CharacterBase::AnimKind::kEnergyChargeAttack},
		{"DashAttack",CharacterBase::AnimKind::kDashAttack}
	};
}

CharacterBase::CharacterBase(ObjectTag tag, CharacterKind kind) :
	Actor(tag, ColliderData::Kind::kSphere),
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
	m_isEndAnim(false)
{
	auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);

	sphereData->m_radius = GameSceneConstant::kCharacterRadius;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	SetNormalAttackData(data);

	m_playAnimKind = AnimKind::kSkyIdle;
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::Init()
{

	//入力情報の設定
	if (m_playerNumber == CharacterBase::PlayerNumber::kOnePlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(0);
	}
	else if (m_playerNumber == CharacterBase::PlayerNumber::kTwoPlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(1);

		m_pEnemyInput = std::make_shared<EnemyInput>(m_input);

		m_pEnemyInput->SetGameManager(m_pGameManager);
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
	else if (m_characterKind == CharacterKind::kBigBlue)
	{
		path += "BigB";
	}

	path = "data/model/" + path + ".mv1";

	m_modelHandle = MV1LoadModel(path.c_str());

	MV1SetScale(m_modelHandle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));

	Collidable::Init();

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

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

	//初期ステートの設定
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();

	//敵側の処理
	if (m_playerNumber == CharacterBase::PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->SetState(m_pState);
	}

}

void CharacterBase::Update()
{

	//エネミーの入力情報を更新する
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->Update();
	}

	//Stateが変更されていたらStateを入れ替える
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;
		
		//敵側の処理
		if (m_playerNumber == CharacterBase::PlayerNumber::kTwoPlayer)
		{
			m_pEnemyInput->SetState(m_pState);
		}
	}

	//状況によって変化するアップデート
	(this->*m_updateSituationFunc)();

	//ローカル座標の中心座標の更新
	m_lookPos.SetCenterPos(m_rigidbody.GetPos());


	//描画座標の設定
	SetDrawPos(m_rigidbody.GetPos());
}

void CharacterBase::Draw()
{
	//リザルト時に2P側を消す
	if (m_pGameManager->GetNowSituation() == GameManager::Situation::kResult ||
		m_pGameManager->GetNowSituation() == GameManager::Situation::kRetry)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer) return;
	}

	//残像の描画
	for (auto item : m_afterImageList)
	{
		MV1DrawModel(item.handle);
	}

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void CharacterBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_pState->OnCollide(collider);

#ifdef _DEBUG

	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		DrawString(0, 64, "1Pがなにかとぶつかった", GetColor(255, 255, 255));
	}
	else
	{
		DrawString(0, 80, "2Pがなにかとぶつかった", GetColor(255, 255, 255));
	}

#endif // _DEBUG
}

void CharacterBase::Final()
{
	Collidable::Final();
	MV1DeleteModel(m_modelHandle);
	for (auto& item : m_afterImageList)
	{
		MV1DeleteModel(item.handle);
	}
}

void CharacterBase::RetryInit()
{
	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	m_knockOutVelo = MyEngine::Vector3();

	auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

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

	//初期ステートの設定
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();
}

void CharacterBase::UpdateAfterImage()
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

void CharacterBase::SetGameManager(std::shared_ptr<GameManager> manager)
{
	m_pGameManager = manager;
}

MyEngine::Vector3 CharacterBase::GetPos()
{
	return m_rigidbody.GetPos();
}

MyEngine::Vector3 CharacterBase::GetVelo()
{
	return m_rigidbody.GetVelo();
}

bool CharacterBase::SubMp(float subMp)
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

void CharacterBase::ChargeMp()
{
	m_nowMp += m_status.chargeSpd;

	//最大を超えないようにクランプ
	m_nowMp = std::fmin(m_nowMp, GameSceneConstant::kMaxMp);
}

void CharacterBase::ChangeAnim(AnimKind animKind, bool loop)
{
	ChangeAnim(animKind, loop, kAnimBlendSpeed);
}

void CharacterBase::ChangeAnim(AnimKind animKind, bool loop, float blendSpeed)
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

void CharacterBase::PlayAnim()
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

void CharacterBase::SetAnimPlaySpeed(float speed)
{
	m_animPlaySpeed = speed;
}

void CharacterBase::SetAnimPlaySpeed()
{
	m_animPlaySpeed = kAnimPlaySpeed;
}

bool CharacterBase::IsAnimEnd()
{
	return false;
}

CharacterBase::AttackKind CharacterBase::GetSpecialAttackKind(std::string kind)
{
	return kSpecialAttackKindMap.at(kind);
}

void CharacterBase::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto& item : normalAttackData)
	{
		NormalAttackData pushData;

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

		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

std::shared_ptr<Attack> CharacterBase::CreateAttack(AttackData attackData)
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
		auto player1 = m_pGameManager->GetOnePlayerPointer();

		targetPos = m_pGameManager->GetTargetPos(player1);
		toTarget = targetPos - m_rigidbody.GetPos();
	}
	else
	{
		auto player2 = m_pGameManager->GetTwoPlayerPointer();

		targetPos = m_pGameManager->GetTargetPos(player2);
		toTarget = targetPos - m_rigidbody.GetPos();
	}

	localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget.Normalize());

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

	ans->Init(status, m_pGameManager->GetEffectManagerPointer());

	m_pGameManager->AddAttack(ans);

	return ans;
}

CharacterBase::NormalAttackData CharacterBase::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}

CharacterBase::SpecialAttackData CharacterBase::GetSpecialAttackData(int specialNumber)
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

void CharacterBase::SetFrontPos(MyEngine::Vector3 frontPos)
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

MyEngine::Vector3 CharacterBase::GetFrontPos()
{
	return m_lookPos.GetWorldPos();
}

bool CharacterBase::IsFrontTarget()
{

	MyEngine::Vector3 toTargetDir;

	//プレイヤーなら
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		auto player1 = m_pGameManager->GetOnePlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetTargetPos(player1));
	}
	//2P側なら(基本CPU)
	else
	{
		auto player2 = m_pGameManager->GetTwoPlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetTargetPos(player2));
	}

	toTargetDir = toTargetDir.Normalize();

	if (toTargetDir.z > 0)
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

void CharacterBase::SetDrawPos(MyEngine::Vector3 pos)
{
	pos.y -= kCharacterHeight;

	MV1SetPosition(m_modelHandle, pos.CastVECTOR());
}

CharacterBase::AnimKind CharacterBase::GetAttackAnimKind(std::string animName)
{
	return kAttackAnimKindMap.at(animName);
}

void CharacterBase::LookTarget()
{
	auto pointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetTargetPos(pointer)).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	m_lookPos.SetLocalPos(m_lookPos.ChangeWorldToLocal(m_pGameManager->GetTargetPos(pointer)));
}

void CharacterBase::CreateAfterImage()
{
	AfterImage data;

	data.DeleteSpeed = GameSceneConstant::kAfterImageDeleteSpeed;

	data.nowOpacityRate = GameSceneConstant::kAfterImageInitOpacityRate;

	data.maxOpacityRate = GameSceneConstant::kAfterImageMaxOpacityRate;

	CreateAfterImage(data);
}

void CharacterBase::CreateAfterImage(AfterImage afterImageInfo)
{
	CharacterBase::AfterImage ans;

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

void CharacterBase::StartFastCameraMove()
{
	m_pGameManager->StartFastCameraMove();
}

MyEngine::Vector3 CharacterBase::GetBackPos(float distance)
{
	MyEngine::Vector3 toBackPos = m_rigidbody.GetPos() - m_lookPos.GetWorldPos();

	toBackPos = toBackPos.Normalize();

	MyEngine::Vector3 ans = m_rigidbody.GetPos() + (toBackPos * (distance + GameSceneConstant::kCharacterRadius));

	MyEngine::LocalPos local;

	local.SetCenterPos(m_rigidbody.GetPos());
	MyEngine::Vector3 pos = m_rigidbody.GetPos();

	return ans;
}

void CharacterBase::SetIsTrigger(bool flag)
{
	m_pColData->SetIsTrigger(flag);
}

void CharacterBase::ChangeSituationUpdate(int situation)
{
	auto sit = static_cast<GameManager::Situation>(situation);

	//リトライが押されたとき
	if (sit == GameManager::Situation::kRetry)
	{
		//アニメーションを止める
		m_updateSituationFunc = &CharacterBase::UpdateNone;
	}
	//1Pの開始演出をしているとき
	else if (sit == GameManager::Situation::kStart1P)
	{

		if (m_playerNumber == PlayerNumber::kOnePlayer)
		{
			//1P側はスタート処理を行う
			m_updateSituationFunc = &CharacterBase::UpdateStart;
		}
		else
		{
			//2P側は何もしないように設定する
			m_updateSituationFunc = &CharacterBase::UpdateNone;
		}
	}
	//2Pの開始演出をしているとき
	else if (sit == GameManager::Situation::kStart2P)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer)
		{
			//2P側はスタート処理を行う
			m_updateSituationFunc = &CharacterBase::UpdateStart;
		}
		else
		{
			//1P側は何もしないように設定する
			m_updateSituationFunc = &CharacterBase::UpdateNone;
		}
	}
	//バトル時
	else if (sit == GameManager::Situation::kBattle)
	{
		m_updateSituationFunc = &CharacterBase::UpdateBattle;
	}
	//ノックアウトの演出時
	else if (sit == GameManager::Situation::kKnockOut)
	{
		m_updateSituationFunc = &CharacterBase::UpdateKnockOut;
	}
	//リザルト時
	else if (sit == GameManager::Situation::kResult)
	{
		m_updateSituationFunc = &CharacterBase::UpdateResult;
	}
}

void CharacterBase::UpdateStart()
{
	//アニメーションを変更していなければ変更する
	if (m_playAnimKind != AnimKind::kStartPose)
	{
		ChangeAnim(AnimKind::kStartPose, false);
	}
	PlayAnim();
}

void CharacterBase::UpdateBattle()
{
	//Stateの更新
	m_pState->Update();

	//残像の更新を行う
	UpdateAfterImage();

	//アニメーションの更新
	PlayAnim();

}

void CharacterBase::UpdateKnockOut()
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

void CharacterBase::UpdateResult()
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
			auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pGameManager->GetTargetPos(thisPointer);

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
			auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pGameManager->GetTargetPos(thisPointer);

			frontPos.y = m_rigidbody.GetPos().y;

			SetFrontPos(frontPos);
		}
	}

	PlayAnim();
}

void CharacterBase::UpdateNone()
{
	//何も更新しない
}
