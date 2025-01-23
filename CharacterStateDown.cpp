#include "CharacterStateDown.h"
#include "Character.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"

namespace
{
	//ダウンする時間
	constexpr int kDownTime = 120;

	//動けるようになる時間
	constexpr int kMoveTime = 45;
}

CharacterStateDown::CharacterStateDown(std::shared_ptr<Character> character) :
	CharacterStateBase(character)
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
		//入力データを取得
		auto input = m_pCharacter->GetInputData();

		//回避入力があったら
		if (input->IsTrigger("A"))
		{
			//次の状態をダッシュ状態に設定する
			std::shared_ptr<CharacterStateDash> next = std::make_shared<CharacterStateDash>(m_pCharacter);
			//ダッシュ状態に遷移
			ChangeState(next);
			//回避になるように設定
			next->SetDodge();
		}
	}

	//移動ベクトルを0にする
	SetCharacterVelo(MyEngine::Vector3());
}

void CharacterStateDown::Exit()
{
}

void CharacterStateDown::SetFrontHit(bool front)
{
	//前方から攻撃を受けたかどうかを設定
	if (front)
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kFrontDown,false);
	}
	else
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kBackDown,false);
	}
}
