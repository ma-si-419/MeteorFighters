#include "CharacterStateHitAttack.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDown.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Character.h"
#include "Attack.h"
#include "GameManagerBase.h"
#include "Effect.h"
#include "Rock.h"
#include "TutorialManager.h"
#include "SoundManager.h"

namespace
{
	const std::map<Character::HitReactionKind, float> kMoveSpeedMap =
	{
		{Character::HitReactionKind::kGuard,0.2f},
		{Character::HitReactionKind::kLow,0.2f},
		{Character::HitReactionKind::kMiddle,0.8f},
		{Character::HitReactionKind::kWeakUpBurst,1.2f},
		{Character::HitReactionKind::kUpBurst,2.0f},
		{Character::HitReactionKind::kFarBurst,2.0f},
		{Character::HitReactionKind::kDownBurst,2.0f},
		{Character::HitReactionKind::kWeakBurst,1.8f},
		{Character::HitReactionKind::kMiddleStan,0.1f},
		{Character::HitReactionKind::kBottomStan,0.1f}
	};

	//�O����U�����󂯂����̔���
	const std::map<Character::HitReactionKind, Character::AnimKind> kFrontHitReactionMap =
	{
		{Character::HitReactionKind::kGuard,Character::AnimKind::kGuardMiddle},
		{Character::HitReactionKind::kLow,Character::AnimKind::kLowHit1},
		{Character::HitReactionKind::kMiddle,Character::AnimKind::kMiddleHit},
		{Character::HitReactionKind::kWeakUpBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kUpBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kDownBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kFarBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kWeakBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kBottomStan,Character::AnimKind::kBottomStan},
		{Character::HitReactionKind::kMiddleStan,Character::AnimKind::kFrontMiddleStan}
	};

	//��납��U�����󂯂����̔���
	const std::map<Character::HitReactionKind, Character::AnimKind> kBackHitReactionMap =
	{
		{Character::HitReactionKind::kGuard,Character::AnimKind::kBackLowHit1},
		{Character::HitReactionKind::kLow,Character::AnimKind::kBackLowHit1},
		{Character::HitReactionKind::kMiddle,Character::AnimKind::kBackMiddleHit},
		{Character::HitReactionKind::kWeakUpBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kUpBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kDownBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kFarBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kWeakBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kBottomStan,Character::AnimKind::kBottomStan},
		{Character::HitReactionKind::kMiddleStan,Character::AnimKind::kBackMiddleStan}
	};

	//�X�^�����̃A�j���[�V�������������Đ����鎞�Ԃ̊���
	constexpr float kSlowAnimTimeRate = 0.4f;
	//�X�^�����̃A�j���[�V�������������Đ�����Ƃ��̍Đ����x
	constexpr float kSlowAnimPlaySpeed = 0.3f;

	//�ړ����x�����������Ă����^�C�~���O
	constexpr float kMoveSpeedDecelerationTime = 0.6f;

	//�󂯐g���Ƃ��悤�ɂȂ鎞��(�����Ȃ����Ԃ̊���)
	constexpr float kStopTimeRate = 0.7f;

	//������ђ��ɂ�����d��
	constexpr float kBurstGravity = 0.1f;

	//�n�ʂƂԂ���Ɣ��肷��ړ��x�N�g����y�̒l
	constexpr float kGroundHitY = 1.8f;

	//�ǂƂԂ���Ɣ��肷��ړ��x�N�g���̒���
	constexpr float kWallHitLength = 1.8f;

	//�Ԃ��������̃G�t�F�N�g�̍Đ�����
	constexpr int kBumpEffectLifeTime = 90;

	//�X�e�[�W�ƂԂ��������ɃG�t�F�N�g�������悤�ɂ��鋗��(��苗���ړ����Ȃ��ƃG�t�F�N�g�������Ȃ��悤�ɂ���)
	constexpr float kStageBumpEffectPopLength = 20.0f;

	//�΂̎�ސ�
	constexpr int kRockKindNum = 2;

	//�Ԃ������Ƃ��ɏo���΂̐�
	constexpr int kRockNum = 15;

	//�󂯐g�����Ƃ��̃A�j���[�V�����u�����h�̑���
	constexpr float kFallsBlendSpeed = 0.2f;

	//�󂯐g�����Ƃ��̃A�j���[�V�����̍Đ����x
	constexpr float kFallsPlaySpeed = 2.5f;

	//�󂯐g�ɂ����鎞��
	constexpr int kFallsStopTime = 10;

	//�󂯐g���Ƃ������̃G�t�F�N�g�̍Đ�����
	constexpr int kFallsEffectLifeTime = 30;

	//�X�e�[�W�ɂԂ��������̃J������h�炷����
	constexpr int kStageBumpShakeCameraTime = 13;

	//�X�e�[�W�ɂԂ��������̃J������h�炷�傫��
	constexpr int kStageBumpShakeCameraPower = 10;

	//�X�e�[�W�ɂԂ��������󂯂�_���[�W
	constexpr int kStageBumpDamage = 2000;
}

CharacterStateHitAttack::CharacterStateHitAttack(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveTime(0),
	m_isFrontHit(false),
	m_moveLength(0.0f),
	m_isStageBump(false),
	m_isFalls(false)
{
}

void CharacterStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void CharacterStateHitAttack::Update()
{
	//���Ԍv��
	m_time++;

	//���̓}�l�[�W���[
	auto input = GetCharacterInput();

	//���󂯂Ă���U�����X�^���U����������
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kBottomStan ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_stopTime * kSlowAnimTimeRate);

		if (m_time < slowAnimTime)
		{
			//�Đ����x���������ɂ���
			m_pCharacter->SetAnimPlaySpeed(kSlowAnimPlaySpeed);
		}
		else
		{
			//�Đ����x�������l�ɖ߂�
			m_pCharacter->SetAnimPlaySpeed();
		}
	}

	//���������Ă������ԂɂȂ��Ă�����
	if (m_time > m_stopTime * kMoveSpeedDecelerationTime)
	{
		//�����x�N�g�����ݒ肳��Ă��Ȃ����
		if (m_decelerationVec.SqLength() < 0.01f)
		{
			//�����x�N�g�����v�Z����
			m_decelerationVec = m_moveVec / static_cast<float>(m_stopTime * kMoveSpeedDecelerationTime);
		}

		//�����x�N�g�������ړ��x�N�g���̒������傫�����
		if (m_moveVec.SqLength() > m_decelerationVec.SqLength())
		{
			//�����x�N�g�����ړ��x�N�g���ɉ��Z����
			m_moveVec += m_decelerationVec;
		}
		//�����x�N�g�������ړ��x�N�g���̒��������������
		else
		{
			//�����x�N�g����0�ɂ���
			m_decelerationVec = MyEngine::Vector3();
		}

		//������ђ��͏d�͂�������
		if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakUpBurst)
		{
			m_moveVec.y -= kBurstGravity;
		}
	}


	//��������т̏�Ԃł���Ί�{�I�ɃG�t�F�N�g���Đ�����
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst)
	{
		m_isStageBump = true;
	}

	//���ȏ�ړ����Ă�����
	if (m_moveLength > kStageBumpEffectPopLength)
	{
		//�n�ʂɂԂ��������G�t�F�N�g���Đ�����t���O�𗧂Ă�
		m_isStageBump = true;
	}
	//����ȊO�ł����
	else
	{
		//�n�ʂɂԂ��������G�t�F�N�g���Đ�����t���O��������
		m_isStageBump = false;
	}

	//�������Ă���ꍇ
	if (m_time > m_stopTime * kMoveSpeedDecelerationTime)
	{
		//��������шȊO�̓G�t�F�N�g���Đ����Ȃ�
		if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst)
		{
			m_isStageBump = false;
		}
	}

	//�ݒ肵�����Ԃ�������
	if (m_time >= static_cast<float>(m_stopTime * kStopTimeRate))
	{
		//�󂯐g���Ƃ��悤�ɂ���
		m_isFalls = true;
	}

	//������я�Ԃł����
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst)
	{
		//�ړ��x�N�g�������ȏ㉺�������Ă��Ēn�ʂƂԂ����Ă�����
		if (m_pCharacter->IsGround() &&
			m_moveVec.y < -kGroundHitY)
		{
			//���̏�Ԃ��_�E����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateDown> next = std::make_shared<CharacterStateDown>(m_pCharacter);
			//�_�E���̕�����ݒ肷��
			next->SetFrontHit(m_isFrontHit);
			//�����Ă����������ł�������Ƌt�����ɂ���
			m_pCharacter->SetFrontPos(m_pCharacter->GetPos() - m_moveVec);
			//�_�E����ԂɑJ�ڂ���
			ChangeState(next);

			//�X�e�[�W�ƂԂ���t���O�������Ă�����
			if (m_isStageBump)
			{
				//�n�ʂɂԂ������G�t�F�N�g���Đ�����
				auto effect = std::make_shared<Effect>(Effect::EffectKind::kStageHit);
				//�G�t�F�N�g�̈ʒu��ݒ肷��
				effect->SetPos(m_pCharacter->GetPos() + m_moveVec);
				effect->SetLifeTime(kBumpEffectLifeTime);
				m_pManager->EntryEffect(effect);

				//�΂̐��������[�v
				for (int i = 0; i < kRockNum; i++)
				{
					//�΂̃I�u�W�F�N�g�쐬����
					auto rock = std::make_shared<Rock>();
					//�΂̃��f���n���h����ݒ肷��
					rock->SetHandle(m_pManager->GetRockModelHandle(GetRand(kRockKindNum)));
					//�΂̈ړ��x�N�g����ݒ肷��
					rock->SetMoveVec(m_pCharacter->GetPos() + m_moveVec, m_pCharacter->GetPos());
					//�΂̏�����
					rock->Init();
					//�΂�o�^����
					m_pManager->EntryObject(rock);
				}
				//�Ԃ��������̃T�E���h���Đ�
				SoundManager::GetInstance().PlayOnceSound("StageHit");

				//�X�e�[�W�ɂԂ��������ɃJ������h�炷
				m_pManager->ShakeCamera(kStageBumpShakeCameraTime, kStageBumpShakeCameraPower);

				//�X�e�[�W�ɂԂ��������Ƀ_���[�W���󂯂�
				m_pCharacter->SubHp(kStageBumpDamage);
			}


			return;
		}

		//�ړ��x�N�g����XZ�����̒���
		float moveXZLength = MyEngine::Vector3(m_moveVec.x, 0.0f, m_moveVec.z).Length();

		//�ǂƂԂ����Ă�����
		if (m_pCharacter->IsWall() &&
			moveXZLength > kWallHitLength)
		{
			//���̏�Ԃ��_�E����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateDown> next = std::make_shared<CharacterStateDown>(m_pCharacter);
			//�_�E���̕�����ݒ肷��
			next->SetFrontHit(m_isFrontHit);
			//�����Ă����������ł�������Ƌt�����ɂ���
			m_pCharacter->SetFrontPos(m_pCharacter->GetPos() - m_moveVec);
			//�_�E����ԂɑJ�ڂ���
			ChangeState(next);

			//�X�e�[�W�ƂԂ���t���O�������Ă�����
			if (m_isStageBump)
			{
				//�ǂɂԂ������G�t�F�N�g���Đ�����
				auto effect = std::make_shared<Effect>(Effect::EffectKind::kStageHit);
				//�G�t�F�N�g�̈ʒu��ݒ肷��
				effect->SetPos(m_pCharacter->GetPos());
				effect->SetLifeTime(kBumpEffectLifeTime);
				m_pManager->EntryEffect(effect);

				//�΂̐��������[�v
				for (int i = 0; i < kRockNum; i++)
				{
					//�΂̃I�u�W�F�N�g�쐬����
					auto rock = std::make_shared<Rock>();
					//�΂̃��f���n���h����ݒ肷��
					rock->SetHandle(m_pManager->GetRockModelHandle(GetRand(kRockKindNum)));
					//�΂̈ړ��x�N�g����ݒ肷��
					rock->SetMoveVec(m_pCharacter->GetPos() + m_moveVec, m_pCharacter->GetPos());
					//�΂̏�����
					rock->Init();
					//�΂�o�^����
					m_pManager->EntryObject(rock);
				}

				//�Ԃ��������̃T�E���h���Đ�
				SoundManager::GetInstance().PlayOnceSound("StageHit");

				//�X�e�[�W�ɂԂ��������ɃJ������h�炷
				m_pManager->ShakeCamera(kStageBumpShakeCameraTime, kStageBumpShakeCameraPower);

				//�X�e�[�W�ɂԂ��������Ƀ_���[�W���󂯂�
				m_pCharacter->SubHp(kStageBumpDamage);
			}

			return;
		}

		//�󂯐g���Ƃ���ԂŎ󂯐g�{�^���������ꂽ��
		if (input->IsTrigger("B") && m_isFalls)
		{
			//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			//�󂯐g�A�j�����Đ�����
			next->SetEndAnim(static_cast<int>(Character::AnimKind::kBottomStan), kFallsStopTime, kFallsBlendSpeed);

			//�󂯐g�G�t�F�N�g���Đ�����
			auto effect = std::make_shared<Effect>(Effect::EffectKind::kFalls);

			//�G�t�F�N�g�̈ʒu��ݒ肷��
			effect->SetPos(m_pCharacter->GetPos() + m_moveVec * kFallsStopTime);

			//�G�t�F�N�g�̎�����ݒ肷��
			effect->SetLifeTime(kFallsEffectLifeTime);

			//�G�t�F�N�g��o�^����
			m_pManager->EntryEffect(effect);

			//�󂯐g�`���[�g���A�����N���A������
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kFalls));

			m_pCharacter->SetAnimPlaySpeed(kFallsPlaySpeed);

			//�A�C�h����ԂɑJ�ڂ���
			ChangeState(next);

			return;
		}
	}
	//�㐁����тł����
	else if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakUpBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakBurst)
	{

		//�󂯐g���Ƃ���ԂŎ󂯐g�{�^���������ꂽ��
		if (input->IsTrigger("B") && m_isFalls)
		{
			//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//�󂯐g�A�j�����Đ�����
			next->SetEndAnim(static_cast<int>(Character::AnimKind::kFrontBurst), kFallsStopTime, kFallsBlendSpeed);
			m_pCharacter->SetAnimPlaySpeed(kFallsPlaySpeed);
			//�󂯐g�`���[�g���A�����N���A������
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kFalls));
			//�A�C�h����ԂɑJ�ڂ���
			ChangeState(next);
			return;
		}

		//�ݒ肵�����Ԃ�������
		if (m_time >= m_stopTime)
		{
			//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//�A�C�h����ԂɑJ�ڂ���
			ChangeState(next);

			return;
		}
	}
	//����ȊO�̏�Ԃł����
	else
	{
		//�ݒ肵�����Ԃ�������
		if (m_time >= m_stopTime)
		{
			//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//�A�C�h����ԂɑJ�ڂ���
			ChangeState(next);

			return;
		}
	}

	//�ړ�����������ۑ�����
	m_moveLength += m_moveVec.Length();

	//�L�����N�^�[�̈ړ��x�N�g����ݒ肷��
	SetCharacterVelo(m_moveVec);


}

void CharacterStateHitAttack::Exit()
{
}

void CharacterStateHitAttack::HitAttack(int kind)
{
	//���Ԃ�����������
	m_time = 0;

	Character::HitReactionKind reaction = static_cast<Character::HitReactionKind>(kind);

	//�R���{���ɉ��̍U�����󂯂�����ۑ����Ă���
	int hitAttackKinds[static_cast<int>(Character::HitReactionKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[item]++;
	}

	//����������ݒ肷��
	MyEngine::Vector3 moveDir;
	//�U�����ꂽ�L�����N�^�[�̍��W�𒆐S�Ƃ������[�J�����W���쐬����
	MyEngine::LocalPos local;
	local.SetCenterPos(m_pCharacter->GetPos());
	//���[�J�����W�̑O�������U���������̂���U�����ꂽ���̂Ɍ�����
	MyEngine::Vector3 centerFrontPos = (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)).Normalize() + m_pCharacter->GetPos();
	local.SetFrontPos(centerFrontPos);

	//�R���{�Ƃ����Ԃ̊m�F

	//���i�X�^���U��
	if (reaction == Character::HitReactionKind::kBottomStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������
			reaction = Character::HitReactionKind::kWeakBurst;
		}
	}
	//���i�X�^���U��
	else if (reaction == Character::HitReactionKind::kMiddleStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������
			reaction = Character::HitReactionKind::kWeakBurst;
		}
	}


	//������ɐ�����΂�����
	if (reaction == Character::HitReactionKind::kUpBurst)
	{
		//�΂ߏ�ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//�������ɐ�����΂�����
	else if (reaction == Character::HitReactionKind::kDownBurst)
	{
		//�΂߉��ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, -1, 1));
	}
	//����ȊO�̂���
	else
	{
		//��{�I�ɉ������ɓ�����
		local.SetLocalPos(MyEngine::Vector3(0, 0, 1));
	}

	moveDir = (local.GetWorldPos() - m_pCharacter->GetPos());

	//�����Ԃɂ���Ĉړ����x��ύX����
	m_moveVec = moveDir * kMoveSpeedMap.at(reaction);

	//�O�����牣��ꂽ���ǂ������擾����
	m_isFrontHit = m_pCharacter->IsFrontTarget();

	//�O�����牣���Ă�����v���C���[�̕���������
	if (m_isFrontHit)
	{
		m_pCharacter->LookTarget();
	}
	//������牣���Ă�����v���C���[�Ƃ͋t�̕���������
	else
	{
		MyEngine::Vector3 frontPos;

		frontPos = (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)) + m_pCharacter->GetPos();

		m_pCharacter->SetFrontPos(frontPos);
	}

	//�A�j���[�V�����̕ύX
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(GetNextAnimKind(kind)), false);

	//�O����̐�����уA�j���[�V�����̏ꍇ
	if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kFrontBurst)
	{
		//�����Ă�������Ƌt�̕���������
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() - m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}
	//��납��̏ꍇ
	else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kBackBurst)
	{
		//�����Ă������������
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() + m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}

	//���݂̂����Ԃ�ݒ肷��
	m_pCharacter->SetHitReaction(reaction);

	//���܂Ŏ󂯂��U����ۑ����Ă���
	m_hitReactions.push_back(kind);
}

int CharacterStateHitAttack::GetNextAnimKind(int kind)
{

	Character::AnimKind ans = Character::AnimKind::kLowHit1;

	Character::AnimKind animKind = m_pCharacter->GetPlayAnimKind();

	Character::HitReactionKind reaction = static_cast<Character::HitReactionKind>(kind);

	//���ʂ���U�����󂯂Ă���ꍇ
	if (m_isFrontHit)
	{
		ans = kFrontHitReactionMap.at(reaction);
	}
	//��납��U�����󂯂Ă���ꍇ
	else
	{
		ans = kBackHitReactionMap.at(reaction);
	}

	//��U�����󂯂��ꍇ�A���ԂɃA�j���[�V������������
	if (ans == Character::AnimKind::kLowHit1)
	{
		//��U��1���󂯂Ă����Ԃ�������
		if (animKind == Character::AnimKind::kLowHit1)
		{
			ans = Character::AnimKind::kLowHit2;
		}
		//��U��2���󂯂Ă����Ԃ�������
		else if (animKind == Character::AnimKind::kLowHit2)
		{
			ans = Character::AnimKind::kLowHit3;
		}
	}
	else if (ans == Character::AnimKind::kBackLowHit1)
	{
		//��U��1���󂯂Ă����Ԃ�������
		if (animKind == Character::AnimKind::kLowHit1)
		{
			ans = Character::AnimKind::kLowHit2;
		}
		//��U��2���󂯂Ă����Ԃ�������
		else if (animKind == Character::AnimKind::kLowHit2)
		{
			ans = Character::AnimKind::kLowHit3;
		}
	}

	return static_cast<int>(ans);
}
