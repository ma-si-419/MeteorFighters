#include "CharacterStateDown.h"
#include "Character.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "TutorialManager.h"
#include "GameManagerBase.h"


namespace
{
	//ダウンする時間
	constexpr int kDownTime = 90;

	//動けるようになる時間
	constexpr int kMoveTime = 25;
}

CharacterStateDown::CharacterStateDown(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_isReturn(false)
{
}

void CharacterStateDown::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kDown;


}

void CharacterStateDown::Update()
{
	//時間計測
	m_time++;

	//ダウンする時間が経ったら
	if (m_time > kDownTime)
	{
		//次の状態をアイドル状態に設定する
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
		//アイドル状態に遷移する
		ChangeState(next);
	}

	//動けるようになる時間が経ったら
	if (m_time > kMoveTime)
	{
		//復帰フラグを立てる
		m_isReturn = true;

	}
	//入力データを取得
	auto input = m_pCharacter->GetInputData();

	//復帰できる状況で復帰入力があったら
	if (input->IsTrigger("B") && m_isReturn)
	{
		//次の状態をダッシュ状態に設定する
		std::shared_ptr<CharacterStateDash> next = std::make_shared<CharacterStateDash>(m_pCharacter);
		//回避になるように設定
		next->SetDodge();
		//移動方向を設定
		next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, -1.0f));
		//ダッシュ状態に遷移
		ChangeState(next);
		//復帰チュートリアルをクリア
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kReturn));
		return;
	}

	//移動ベクトルを0にする
	SetCharacterVelo(MyEngine::Vector3());
}

void CharacterStateDown::Exit()
{
	//前方向を敵の座標のY成分を抜いたものにする
	m_pCharacter->SetFrontPos(MyEngine::Vector3(m_pManager->GetTargetPos(m_pCharacter).x, m_pCharacter->GetPos().y, m_pManager->GetTargetPos(m_pCharacter).z));
}

void CharacterStateDown::SetFrontHit(bool front)
{
	//前方から攻撃を受けたかどうかを設定
	if (front)
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kFrontDown, false);
	}
	else
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kBackDown, false);
	}
}
