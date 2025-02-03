#include "CharacterStateCharge.h"
#include "CharacterStateIdle.h"
#include "CharacterStateRush.h"
#include "CharacterStateSpecialAttack.h"
#include "Character.h"
#include "Input.h"
#include "DxLib.h"
#include "Effect.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"

namespace
{
	//チャージ終了アニメーションの時間
	constexpr int kEndChargeTime = 10;

	//チャージ始めのアニメーションの再生速度
	constexpr float kStartAnimSpeed = 2.0f;

	//チャージ初めのカメラを揺らす時間
	constexpr int kCameraShakeTime = 5;

	//エフェクトのループスタートフレーム
	constexpr int kEffectStartFrame = 30;

	//エフェクトのループエンドフレーム
	constexpr int kEffectEndFrame = 33;
}

CharacterStateCharge::CharacterStateCharge(std::shared_ptr<Character> character) :
	CharacterStateBase(character)
{
}

void CharacterStateCharge::Enter()
{
	//次のStateのポインタを自身に設定しておく
	m_pNextState = shared_from_this();
	//StateKindを設定
	m_kind = CharacterStateKind::kCharge;
	//アニメーションを設定
	m_pCharacter->ChangeAnim(Character::AnimKind::kStartCharge, false);
	//アニメーションの再生速度を設定
	m_pCharacter->SetAnimPlaySpeed(kStartAnimSpeed);
	//エフェクトのポインタ作成
	m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kCharge);
	//エフェクトの座標設定
	m_pEffect->SetPos(m_pCharacter->GetPos());
	//エフェクトのループ設定
	m_pEffect->SetLoop(kEffectStartFrame, kEffectEndFrame);
	//エフェクトをマネージャーに登録
	m_pManager->EntryEffect(m_pEffect);
}

void CharacterStateCharge::Update()
{

	auto input = GetCharacterInput();

	m_pCharacter->ChargeMp();

	//チャージ開始を再生していて
	if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kStartCharge)
	{
		//アニメーションが終わっていたら
		if (m_pCharacter->IsEndAnim())
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kInCharge, true);
			m_pCharacter->SetAnimPlaySpeed();
			m_pManager->ShakeCamera(kCameraShakeTime);
		}
	}

	//レフトショルダーが押されなくなったらアイドルStateに戻る
	if (!input->IsPushTrigger(false))
	{
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		next->SetEndAnim(static_cast<int>(Character::AnimKind::kEndCharge), kEndChargeTime);

		ChangeState(next);

		return;
	}

	//Aボタンが押されたら
	if (input->IsTrigger("A"))
	{
		MyEngine::Vector3 moveDir(input->GetStickInfo().leftStickX, 0, -input->GetStickInfo().leftStickY);

		//突撃Stateに移行する
		auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

		if (moveDir.SqLength() > 0.01f)
		{
			moveDir = moveDir.Normalize();
			next->SetMoveDir(moveDir);

			ChangeState(next);

			return;
		}

		moveDir = MyEngine::Vector3(0, 0, 1);

		next->SetMoveDir(moveDir);

		ChangeState(next);

		return;
	}

	//Yボタンが押されたら
	if (input->IsTrigger("Y"))
	{
		float cost = static_cast<float>(m_pCharacter->GetSpecialAttackData(1).cost);

		//気力が足りなかったら実行しない
		if (m_pCharacter->SubMp(cost))
		{
			auto next = std::make_shared<CharacterStateSpecialAttack>(m_pCharacter);

			next->SetSpecialAttackNumber(1);

			ChangeState(next);
		}
	}

	//1P側が行っていたらカメラを少し揺らす
	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		m_pManager->SwayCamera();
	}

	//エフェクトの座標を設定する
	m_pEffect->SetPos(m_pCharacter->GetPos());

	SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

	//気力がマックスになっていたら
	if (m_pCharacter->GetMp() >= GameSceneConstant::kMaxMp)
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kEnergyCharge));
	}

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Charge", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateCharge::Exit()
{
	//エフェクトを削除する
	m_pManager->ExitEffect(m_pEffect);
}