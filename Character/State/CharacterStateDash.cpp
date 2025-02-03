#include "CharacterStateDash.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"
#include "SoundManager.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kDefaultMoveDir(0, 0, 1);

	constexpr float kDashSpeed = 4.0f;

	constexpr float kDodgeSpeed = 2.5f;

	constexpr int kDashTime = 15;

	constexpr int kJustGuardTime = 20;

	constexpr float kAnimationBlendSpeed = 0.12f;

	constexpr float kAnimPlaySpeed = 1.5f;
}

CharacterStateDash::CharacterStateDash(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveDir(kDefaultMoveDir),
	m_isDodge(false)
{
}

void CharacterStateDash::SetMoveDir(MyEngine::Vector3 moveDir)
{
	m_moveDir = moveDir;
}

void CharacterStateDash::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = GetKind();
	if (m_pCharacter->IsGround())
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kGroundDash, true, kAnimationBlendSpeed);
	}
	else
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kSkyDash, true, kAnimationBlendSpeed);
	}

	//敵の距離が近ければ
	if ((m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length() < GameSceneConstant::kNearLange)
	{
		//回避行動をする
		m_isDodge = true;

		//回避状態にしておく
		m_guardKind = CharacterGuardKind::kDodge;
	}

	//回避
	if (m_isDodge)
	{
		//向きによってアニメーションを変える
		//回避の場合は移動方向を前後左右の四通りに限定する(斜めをなくす)
		if (m_moveDir.z >= 0.5f)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeFront, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(0.0f, 0.0f, 1.0f);
		}
		else if (m_moveDir.z < -0.5f)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeBack, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(0.0f, 0.0f, -1.0f);
		}
		else if (m_moveDir.x >= 0.5f)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeRight, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeLeft, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(-1.0f, 0.0f, 0.0f);
		}

		//回避の音声再生
		SoundManager::GetInstance().PlayOnceSound("Step");
	}
	//ダッシュの場合
	else
	{
		//ダッシュの音声再生
		SoundManager::GetInstance().PlayOnceSound("Dash");
	}

	m_pCharacter->SetAnimPlaySpeed(kAnimPlaySpeed);

}

void CharacterStateDash::Update()
{
	m_time++;

	//エネミーの方向に移動方向を回転させる
	float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
	float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);

	//空中にいて前入力されていたら
	if (m_moveDir.z > 0 && !m_pCharacter->IsGround())
	{
		float frontRate = m_moveDir.z;

		MyEngine::LocalPos enemyLocal;

		enemyLocal.SetCenterPos(m_pManager->GetTargetPos(m_pCharacter));

		enemyLocal.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, GameSceneConstant::kCharacterRadius));

		enemyLocal.SetFrontPos(m_pCharacter->GetPos());

		MyEngine::Vector3 targetPos = m_pManager->GetTargetPos(m_pCharacter) + enemyLocal.GetWorldPos();

		MyEngine::Vector3 toTarget = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

		dir = (dir * (1.0f - frontRate)) + toTarget * frontRate;
	}

	MyEngine::Vector3 velo;

	if (m_isDodge)
	{
		velo = dir * kDodgeSpeed;
	}
	else
	{
		velo = dir * kDashSpeed;
	}

	//移動方向にスピードをかける
	SetCharacterVelo(velo);

	if (m_isDodge)
	{
		//回避なら敵の方を向く
		m_pCharacter->LookTarget();
	}
	else
	{
		//ダッシュなら移動方向を向く
		m_pCharacter->SetFrontPos(m_pCharacter->GetPos() + velo);
	}

	//回避の場合
	if (m_isDodge)
	{
		//残像を作成するタイミングになったら
		if (static_cast<int>(m_time) % GameSceneConstant::kAfterImageCreateTime == 0)
		{
			m_pCharacter->CreateAfterImage();
		}

		//一定時間移動したらアイドルStateに戻る
		if (m_time > kJustGuardTime)
		{
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);
			return;
		}
	}
	//ダッシュの場合
	else
	{
		//一定時間移動したらアイドルStateに戻る
		if (m_time > kDashTime)
		{
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);
			return;
		}
	}
}

void CharacterStateDash::Exit()
{
	//回避の場合
	if (m_isDodge)
	{
		//敵の方向を向く
		m_pCharacter->LookTarget();
		m_pCharacter->SetFrontPos(m_pManager->GetTargetPos(m_pCharacter));

		//ステップチュートリアルクリア判定をtrueにする
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kStep));
	}
	//ダッシュの場合
	else
	{
		//ダッシュチュートリアルクリア判定をtrueにする
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDash));
	}


	m_pCharacter->SetAnimPlaySpeed();
	m_guardKind = CharacterGuardKind::kNone;
}
