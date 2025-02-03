#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "DxLib.h"
#include "GameSceneConstant.h"
#include "GameManagerBase.h"
#include "Input.h"
#include "Attack.h"
#include "Effect.h"
#include "TutorialManager.h"
#include "SoundManager.h"


namespace
{
	const MyEngine::Vector3 kPlayerInitPos(-3, 200, -3);
	const MyEngine::Vector3 kEnemyInitPos(3, 200, 3);

	//���S���W
	const MyEngine::Vector3 kTargetPos(0, 200, 0);

	//�Ԃ��������̈ړ����x
	constexpr float kBumpSpeed = -10.0f;

	//�Ԃ����Ă���~�܂�͂��߂�܂ł̎���
	constexpr int kStopStartTime = 3;

	//�~�܂��
	constexpr float kStopPower = -(kBumpSpeed / 10.0f);

	//�~�܂������ɂǂ̂��炢�Ƃǂ܂邩
	constexpr int kHitBackStayTime = 15;

	//�ő呬�x
	constexpr float kMaxMoveSpeed = 3.0f;

	//�����x
	constexpr float kAcceleration = 0.2f;

	//����
	constexpr float kDashInitSpeed = 1.0f;

	//�u�����h�X�s�[�h
	constexpr float kBlendSpeed = 0.03f;

	//�I�����ɔ�������U���̖��O
	const std::string kAttackName = "ButtonBashingAttack";

	//�U���̑傫��
	constexpr float kAttackRadius = 8.0f;

	//�U���̐�������
	constexpr int kAttackLifeTime = 2;

	//�G�t�F�N�g�̃��[�v�J�n����
	constexpr float kEffectLoopStartTime = 9.0f;
	constexpr float kEffectLoopEndTime = 11.0f;

	//�Ԃ��������̃G�t�F�N�g�̐�������
	constexpr float kBumpEffectLifeTime = 5.0f;

	//�Ԃ��������J������h�炷����
	constexpr int kBumpCameraShakeTime = 2;

	//�Ԃ��������J������h�炷�傫��
	constexpr int kBumpCameraShakePower = 3;

	//���ʉ����Đ�����Ԋu
	constexpr int kSoundPlayInterval = 7;
}

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveSpeed(kBumpSpeed),
	m_isBump(false),
	m_bumpTime(0),
	m_stayTime(0),
	m_isStay(false),
	m_soundPlayTime(0)
{
}

void CharacterStateButtonBashing::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kButtonBashing;

	//1P��2P���ōŏ��̍��W��ύX����
	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		SetCharacterPos(kPlayerInitPos);
	}
	else
	{
		SetCharacterPos(kEnemyInitPos);
	}

	//�ړ����x��������
	SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

	//�A�j���[�V�����ύX
	m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);

	//�{�^���A�ł��n�߂�ƃ}�l�[�W���[�ɓ`����
	m_pManager->StartButtonBashing();

	//�G�t�F�N�g���Đ�����
	auto effect = std::make_shared<Effect>(Effect::EffectKind::kHighHit);
	//���W�ݒ�
	effect->SetPos(kTargetPos);
	//�G�t�F�N�g�̍Đ����Ԃ�ݒ�
	effect->SetLifeTime(kBumpEffectLifeTime);
	//�G�t�F�N�g��o�^
	m_pManager->EntryEffect(effect);
	//�J������h�炷
	m_pManager->ShakeCamera(kBumpCameraShakeTime, kBumpCameraShakePower);
}

void CharacterStateButtonBashing::Update()
{

	//���������State��ButtonBashing�o�Ȃ���Α������^�[��
	if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) != CharacterStateKind::kButtonBashing) return;

	//�Ԃ����Ă��牽�t���[������������ۑ�����
	m_bumpTime++;

	//�G�̕���������
	m_pCharacter->LookTarget();

	//���S�܂ł̃x�N�g��
	MyEngine::Vector3 toTarget;

	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		toTarget = kTargetPos - kPlayerInitPos;
	}
	else
	{
		toTarget = kTargetPos - kEnemyInitPos;
	}

	//���S�Ɍ������Ĉړ�����
	MyEngine::Vector3 moveDir = (toTarget).Normalize();

	//�~�܂�n�߂�^�C�~���O�ɂȂ�����
	if (m_bumpTime > kStopStartTime)
	{
		//�ړ����x���}�C�i�X�ł����
		if (m_moveSpeed < 0.0f)
		{
			//�������~�߂Ă���
			m_moveSpeed += kStopPower;

			//�ړ����x���v���X�ɂȂ��
			if (m_moveSpeed > 0.0f)
			{
				m_isStay = true;
			}
		}
	}

	//��莞�ԂƂǂ܂�
	if (m_isStay)
	{
		//�~�܂��Ă��鎞�Ԃ𑫂��Ă���
		m_moveSpeed = 0.0f;
		m_stayTime++;

		//��莞�ԂƂǂ܂�����
		if (m_stayTime > kHitBackStayTime)
		{
			//�Ƃǂ܂�̂���߂�
			m_isStay = false;
			m_stayTime = 0;
			m_moveSpeed = kDashInitSpeed;
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				//��ڂ�Situation�ɍs��
				m_pManager->SetBashingSituation(GameManagerBase::ButtonBashingSituation::kSecondHit);
			}
		}
	}

	//�Ƃǂ܂�̂��I�������
	if (m_bumpTime > kStopStartTime && !m_isStay)
	{
		//�������Ă���
		m_moveSpeed += kAcceleration;
	}

	//�ړ����x�̃N�����v
	m_moveSpeed = static_cast<float>(fmin(m_moveSpeed, kMaxMoveSpeed));

	//�G�Ƃ̋���
	float toEnemyLength = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length();

	//�ړ��x�N�g��
	MyEngine::Vector3 moveVec;

	//�ړ��x�N�g���ňړ����邩�ǂ���
	bool isMove = true;

	//�G�ƂԂ��鋗���ɂȂ�����
	if ((m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length() < (GameSceneConstant::kCharacterRadius * 2.0f) + 3.0f &&
		m_moveSpeed >= 0.0f)
	{
		//��x�Ԃ����Ă�����
		if (m_isBump)
		{
			//�ړ����Ȃ��悤�ɂ���
			isMove = false;
			SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

			//���ʉ��Đ����Ԃ��J�E���g����
			m_soundPlayTime++;

			//���ʉ����Đ�����
			if (m_soundPlayTime > kSoundPlayInterval)
			{
				SoundManager::GetInstance().PlayOnceSound("ButtonBashing");
				m_soundPlayTime = 0;
			}

			//��ڂ�Situation�ɍs��
			m_pManager->SetBashingSituation(GameManagerBase::ButtonBashingSituation::kFighting);

			//�G�t�F�N�g��ݒ肵�Ă��Ȃ����
			if (!m_pEffect)
			{
				//�G�t�F�N�g���Đ�����
				m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kLowHit);
				//���[�v�ݒ�
				m_pEffect->SetLoop(kEffectLoopStartTime, kEffectLoopEndTime);
				//���W�ݒ�
				m_pEffect->SetPos(kTargetPos);
				//�G�t�F�N�g��o�^
				m_pManager->EntryEffect(m_pEffect);
			}

			//���W��␳����
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				SetCharacterPos(kPlayerInitPos);
			}
			else
			{
				SetCharacterPos(kEnemyInitPos);
			}
		}
		//���߂ĂԂ���^�C�~���O�ł����
		else
		{
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				moveDir = (kPlayerInitPos - kTargetPos).Normalize();
			}
			else
			{
				moveDir = (kEnemyInitPos - kTargetPos).Normalize();
			}
			m_moveSpeed = kBumpSpeed;
			m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);

			//�G�t�F�N�g���Đ�����
			auto effect = std::make_shared<Effect>(Effect::EffectKind::kHighHit);
			//���W�ݒ�
			effect->SetPos(kTargetPos);
			//�G�t�F�N�g�̍Đ����Ԃ�ݒ�
			effect->SetLifeTime(kBumpEffectLifeTime);
			//�G�t�F�N�g��o�^
			m_pManager->EntryEffect(effect);
			//�J������h�炷
			m_pManager->ShakeCamera(kBumpCameraShakeTime, kBumpCameraShakePower);
			//�Ԃ������Ƃ��̃T�E���h���Đ�
			SoundManager::GetInstance().PlayOnceSound("HighHit");
		}
		m_bumpTime = 0;

		m_isBump = true;
	}

	//�ړ�����Ɛݒ肳��Ă�����
	if (isMove)
	{
		moveVec = moveDir * m_moveSpeed;
	}

	//�ړ��x�N�g����ݒ�
	SetCharacterVelo(moveVec);

	//�A�j���[�V�����̕ύX

	//�ړ����x���O�����ɂȂ��Ă����
	if (m_moveSpeed >= 0.0f && !m_isStay)
	{
		//��x�Ԃ����Ă�����
		if (m_isBump)
		{
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kOnButtonBashing)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kOnButtonBashing, true);
			}
		}
		//��x���G�ƂԂ����Ă��Ȃ����
		else
		{
			//�A�j���[�V������ύX����
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kButtonBashingAttack)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingAttack, false, kBlendSpeed);
			}
		}
	}

	//�A�őΌ�
	if (static_cast<GameManagerBase::ButtonBashingSituation>(m_pManager->GetBashingSituation()) == GameManagerBase::ButtonBashingSituation::kFighting)
	{
		auto input = MyEngine::Input::GetInstance().GetInputData(0);

		//�w�肳�ꂽ�{�^���������Ă�����
		if (input->IsTrigger(m_pManager->GetBashingButton()))
		{
			m_pManager->BashButton(m_pCharacter->GetPlayerNumber());
		}
	}

	//�{�^���A�ł���߂�^�C�~���O�ɂȂ�����
	if (!m_pManager->IsButtonBashing())
	{
		//�����Ă����ꍇ
		if (m_pManager->GetButtonBashWinner() == m_pCharacter->GetPlayerNumber())
		{
			//�������Ƃ��̉������Đ�����
			m_pCharacter->PlayVoice(Character::VoiceKind::kWinBashing);

			//�A�C�h����Ԃɖ߂�
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			next->SetEndAnim(static_cast<int>(Character::AnimKind::kOnButtonBashing), 30);

			ChangeState(next);
		}
		//�����Ă����ꍇ
		else
		{
			//�U�����󂯂�悤�ɂ���

			//�^�O�𑊎�̍U���ɂ���
			ObjectTag tag;

			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				tag = ObjectTag::kTwoPlayerAttack;
			}
			else
			{
				tag = ObjectTag::kOnePlayerAttack;
			}

			//�U���̏����擾
			auto attackData = m_pCharacter->GetNormalAttackData(kAttackName);

			//�U���ɐݒ肷��X�e�[�^�X
			Attack::AttackStatus status;
			status.damage = static_cast<int>(attackData.damageRate * m_pCharacter->GetPower());
			status.speed = attackData.attackMoveSpeed;
			status.radius = kAttackRadius;
			status.lifeTime = kAttackLifeTime;
			status.attackKind = attackData.attackKind;
			status.attackHitKind = attackData.attackHitKind;
			status.targetPos = m_pManager->GetTargetPos(m_pCharacter);

			//�󂯂�U��
			auto attack = std::make_shared<Attack>(tag, kTargetPos);

			attack->Init(status, m_pManager->GetEffectManagerPointer());

			m_pManager->AddAttack(attack);
		}
	}
}
void CharacterStateButtonBashing::Exit()
{
	m_pManager->ExitEffect(m_pEffect);

	//�{�^���A�őΌ��̃`���[�g���A�����N���A����
	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kButtonBashing));
}
