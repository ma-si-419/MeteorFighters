#include "CharacterStateBase.h"
#include "CharacterStateHitAttack.h"
#include "CharacterStateGuard.h"
#include "CharacterStateButtonBashing.h"
#include "CharacterStateTeleportation.h"
#include "Character.h"
#include "GameManagerBase.h"
#include "TutorialManager.h"
#include "Attack.h"
#include "Input.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cmath>
#include "GameSceneConstant.h"
#include "LocalPos.h"
#include "SoundManager.h"

namespace
{
	//�G�l���M�[�U�����K�[�h�������̃_���[�W�J�b�g��
	constexpr float kDamageCutRate = 0.2f;

	//�U�����󂯂����ɑJ�ڂ�����
	const std::map<Character::AttackHitKind, Character::HitReactionKind> kHitKindMap =
	{
		{Character::AttackHitKind::kLow,Character::HitReactionKind::kLow},
		{Character::AttackHitKind::kMiddle,Character::HitReactionKind::kMiddle},
		{Character::AttackHitKind::kWeakUpBurst,Character::HitReactionKind::kWeakUpBurst},
		{Character::AttackHitKind::kUpBurst,Character::HitReactionKind::kUpBurst},
		{Character::AttackHitKind::kDownBurst,Character::HitReactionKind::kDownBurst},
		{Character::AttackHitKind::kFarBurst,Character::HitReactionKind::kFarBurst},
		{Character::AttackHitKind::kBottomStan,Character::HitReactionKind::kBottomStan},
		{Character::AttackHitKind::kMiddleStan,Character::HitReactionKind::kMiddleStan}
	};

	//�q�b�g�G�t�F�N�g���c������
	constexpr int kHitEffectLifeTime = 180;

	//�K�[�h�ł���\����������
	const std::vector<Character::HitReactionKind> kCanGuardHitReactionKind =
	{
		Character::HitReactionKind::kGuard,
		Character::HitReactionKind::kLow,
		Character::HitReactionKind::kMiddle,
		Character::HitReactionKind::kNone
	};

	//�u�Ԉړ��ɂ����鎞��
	constexpr int kTeleportationTime = 12;

	//�G�̔w��ɏu�Ԉړ�����Ƃ��̓G�Ƃ̋���
	constexpr float kTeleportationDistance = GameSceneConstant::kCharacterRadius + 5.0f;

	//�U�����󂯂����̍d������
	const std::map<Character::HitReactionKind, int> kDownTimeMap =
	{
		{Character::HitReactionKind::kGuard,10},
		{Character::HitReactionKind::kLow,40},
		{Character::HitReactionKind::kMiddle,40},
		{Character::HitReactionKind::kWeakUpBurst,30},
		{Character::HitReactionKind::kUpBurst,110},
		{Character::HitReactionKind::kFarBurst,110},
		{Character::HitReactionKind::kDownBurst,110},
		{Character::HitReactionKind::kWeakBurst,30},
		{Character::HitReactionKind::kMiddleStan,60},
		{Character::HitReactionKind::kBottomStan,60}
	};

	//������΂��U���𓖂Ă����̃J������h�炷����
	constexpr int kHighShakeCameraTime = 10;

	//������΂��U���𓖂Ă����̃J������h�炷�傫��
	constexpr int kHighShakeCameraPower = 10;

	//�����炢�̍U���𓖂Ă����̃J������h�炷����
	constexpr int kMiddleShakeCameraTime = 5;

	//�����炢�̍U���𓖂Ă����̃J������h�炷�傫��
	constexpr int kMiddleShakeCameraPower = 5;
}

CharacterStateBase::CharacterStateBase(std::shared_ptr<Character> character) :
	m_pCharacter(character),
	m_pManager(m_pCharacter->m_pBattleManager),
	m_pNextState(nullptr),
	m_kind(CharacterStateKind::kIdle),
	m_time(0.0f),
	m_guardKind(CharacterGuardKind::kNone),
	m_stopTime(0)
{
}

void CharacterStateBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	//1P���̏���
	if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
	{
		//2P�̍U���ɓ���������
		if (collider->GetTag() == ObjectTag::kTwoPlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack);
		}
	}
	//2P���̏���
	else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
	{
		//2P�̍U���ɓ���������
		if (collider->GetTag() == ObjectTag::kOnePlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack);
		}
	}
}

void CharacterStateBase::UpdateCommon()
{
	if (m_kind != CharacterStateKind::kButtonBashing)
	{
		//�����State���{�^���A�ł�������ButtonBashingState�ɕύX����
		if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) == CharacterStateKind::kButtonBashing)
		{
			auto next = std::make_shared<CharacterStateButtonBashing>(m_pCharacter);

			ChangeState(next);
		}
	}
}

void CharacterStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//�I������
	Exit();

	//����State�̐ݒ�
	m_pNextState = nextState;

	//����State�̊J�n����
	m_pNextState->Enter();
}

std::shared_ptr<MyEngine::InputData> CharacterStateBase::GetCharacterInput()
{
	return m_pCharacter->m_input;
}

void CharacterStateBase::SetCharacterVelo(MyEngine::Vector3 velo)
{
	m_pCharacter->m_rigidbody.SetVelo(velo);
}

void CharacterStateBase::SetCharacterPos(MyEngine::Vector3 pos)
{
	m_pCharacter->m_rigidbody.SetPos(pos);
}

void CharacterStateBase::HitAttack(std::shared_ptr<Attack> attack)
{
	//�U���̃X�e�[�^�X
	auto status = attack->GetStatus();

	int damage = status.damage;

	//���̏�Ԃ��擾����
	auto hitReaction = static_cast<Character::HitReactionKind>(GetNextHitReactionKind(attack));

	//�Đ�����G�t�F�N�g
	Effect::EffectKind effectKind = Effect::EffectKind::kNone;

	//�������̏�Ԃ��K�[�h�ł����
	if (hitReaction == Character::HitReactionKind::kGuard)
	{
		//�C�e�U���n���_���[�W�J�b�g����
		if (status.attackKind == Character::AttackKind::kLaser ||
			status.attackKind == Character::AttackKind::kEnergy)
		{
			damage = static_cast<int>(damage * kDamageCutRate);
		}
		//�Ō��n�͂��ׂ�0�_���[�W�ɂ���(�����͖�����)
		else
		{
			damage = 0;
		}

		//��{�I�ɃK�[�h�G�t�F�N�g
		effectKind = Effect::EffectKind::kGuardHit;

		//�r�[���U���̏ꍇ
		if (status.attackKind == Character::AttackKind::kLaser)
		{
			//�K�[�h�Ɋ֌W�Ȃ��G�t�F�N�g���Đ�����
			effectKind = Effect::EffectKind::kLaserHit;
		}
	}
	//�K�[�h���Ă��Ȃ��ꍇ
	else
	{
		//�W���X�g�K�[�h�ł��Ă����ꍇ
		if (m_guardKind == CharacterGuardKind::kJustGuard)
		{
			//�u�Ԉړ��̃G�t�F�N�g���Đ�����
			effectKind = Effect::EffectKind::kTeleportaion;
		}
		//�����Ԃ̏ꍇ
		else if (m_guardKind == CharacterGuardKind::kDodge)
		{
			//���ɉ����G�t�F�N�g���Đ����Ȃ�			
		}
		else
		{
			//�U���ɍ��킹���G�t�F�N�g���Đ�����
			effectKind = static_cast<Effect::EffectKind>(static_cast<Effect::EffectKind>(attack->GetHitEffeckKind()));
		}
	}

	//�G�t�F�N�g���ݒ肳��Ă����
	if (effectKind != Effect::EffectKind::kNone)
	{
		std::shared_ptr<Effect> hitEffect = std::make_shared<Effect>(effectKind);
		hitEffect->SetLifeTime(kHitEffectLifeTime);
		hitEffect->SetPos(m_pCharacter->GetPos());
		m_pCharacter->m_pBattleManager->GetEffectManagerPointer()->Entry(hitEffect, m_pCharacter->GetPos());

		MyEngine::Vector3 rotation;

		float vX = attack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = attack->GetPos().z - m_pCharacter->GetPos().z;

		rotation.y = std::atan2f(vX, vZ);

		hitEffect->SetRotation(rotation);
	}

	//�����Ԃł����
	if (m_guardKind == CharacterGuardKind::kDodge)
	{
		//�_���[�W���󂯂Ȃ�
		return;
	}

	//�W���X�g�K�[�h���ł����
	if (m_guardKind == CharacterGuardKind::kJustGuard)
	{
		MyEngine::Vector3 moveTargetPos;

		auto nextState = std::make_shared<CharacterStateTeleportation>(m_pCharacter);

		//�C�e�n�̍U���ł����
		if (attack->GetStatus().attackKind == Character::AttackKind::kEnergy ||
			attack->GetStatus().attackKind == Character::AttackKind::kLaser)
		{
			//�ړ��ڕW���U���̉��̍��W�ɂ���
			MyEngine::LocalPos local;

			local.SetCenterPos(attack->GetPos());

			MyEngine::Vector3 frontPos = (attack->GetStatus().targetPos - attack->GetLastPos()).Normalize() + attack->GetPos();

			local.SetFrontPos(frontPos);

			//�����_���ō��E�ǂ���Ɉړ����邩���߂�
			if (GetRand(1))
			{
				local.SetLocalPos(MyEngine::Vector3(attack->GetStatus().radius + kTeleportationDistance, 0.0f, 0.0f));
			}
			else
			{
				local.SetLocalPos(MyEngine::Vector3(-attack->GetStatus().radius - kTeleportationDistance, 0.0f, 0.0f));
			}

			moveTargetPos = local.GetWorldPos();

			//�ړ���̍��W��ݒ肷��
			nextState->Init(moveTargetPos, kTeleportationTime);
		}
		//�Ō��n�̍U���ł����
		else
		{
			//�G�̌�둤�Ɉړ�����
			nextState->Init(m_pManager->GetTargetBackPos(kTeleportationDistance, m_pCharacter), kTeleportationTime);
		}

		//�W���X�g�K�[�h�`���[�g���A�����N���A������
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kJustGuard));

		//�u�Ԉړ���ԂɑJ�ڂ���
		ChangeState(nextState);

		return;
	}

	//�U�����폜����
	attack->DeleteAttack();

	//�_���[�W���󂯂�
	m_pCharacter->SubHp(damage);

	//�_���[�W���󂯂�ۂɃq�b�g�T�E���h��炷
	SoundManager::GetInstance().PlayOnceSound(status.hitSoundName);

	//�G�𐁂���΂��U���ł����
	if (hitReaction == Character::HitReactionKind::kUpBurst ||
		hitReaction == Character::HitReactionKind::kDownBurst ||
		hitReaction == Character::HitReactionKind::kFarBurst)
	{
		//�J������h�炷
		m_pManager->ShakeCamera(kHighShakeCameraTime, kHighShakeCameraPower);
	}
	//�����炢�̍U���ł����
	else if (hitReaction == Character::HitReactionKind::kMiddle ||
		hitReaction == Character::HitReactionKind::kWeakBurst ||
		hitReaction == Character::HitReactionKind::kWeakUpBurst)
	{
		//�J������h�炷
		m_pManager->ShakeCamera(kMiddleShakeCameraTime, kMiddleShakeCameraPower);
	}

	//�X�[�p�[�A�[�}�[��ԂȂ��
	if (m_guardKind == CharacterGuardKind::kSuperArmor)
	{
		//���ꂪ��ƒ��ł����
		if (hitReaction == Character::HitReactionKind::kLow ||
			hitReaction == Character::HitReactionKind::kMiddle)
		{
			//�����ԂɂȂ�Ȃ�
			return;
		}
		//����ȊO�̏ꍇ�͉���ʂ�(���ʂɃ_���[�W���o�Ɉڍs����)
	}

	//�K�[�h���
	if (hitReaction == Character::HitReactionKind::kGuard)
	{
		//�����Ȃ����Ԃ�ݒ肷��
		SetStopTime(kDownTimeMap.at(hitReaction));

		//�G�̕���������
		m_pCharacter->LookTarget();

		//�K�[�h�`���[�g���A�����N���A������
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kGuard));

		//���݂̏�Ԃ��K�[�h��Ԃł���΃K�[�h��Ԃ��p������
		if (m_kind == CharacterStateKind::kGuard) return;

		//���݂̏�Ԃ��K�[�h��ԂłȂ���΃K�[�h��ԂɑJ�ڂ���
		auto nextState = std::make_shared<CharacterStateGuard>(m_pCharacter);

		//����State�ɓ����Ȃ����Ԃ�ݒ肷��
		nextState->SetStopTime(kDownTimeMap.at(hitReaction));

		ChangeState(nextState);
		return;
	}

	//���̏�Ԃ��U�����󂯂���ԂłȂ��Ȃ��
	if (m_kind != CharacterStateKind::kHitAttack)
	{
		//�����܂ŗ�����q�b�g�A�^�b�N��ԂɑJ�ڂ���
		std::shared_ptr<CharacterStateHitAttack> nextState = std::make_shared<CharacterStateHitAttack>(m_pCharacter);

		//�����Ȃ����Ԃ�ݒ肷��
		nextState->SetStopTime(kDownTimeMap.at(hitReaction));

		//�U�����󂯂����̏�Ԃ�ݒ肷��
		nextState->HitAttack(static_cast<int>(hitReaction));

		ChangeState(nextState);
	}
	else
	{
		auto nowState = std::dynamic_pointer_cast<CharacterStateHitAttack>(m_pCharacter->m_pState);

		//�����Ȃ����Ԃ�ݒ肷��
		nowState->SetStopTime(kDownTimeMap.at(hitReaction));

		nowState->HitAttack(static_cast<int>(hitReaction));
	}

	//�����܂ŗ�����U�����󂯂Ă���̂�

	//����̍U�����s���Ă�����`���[�g���A�����N���A������
	if (status.attackName == "Low1")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack1));
	}
	else if (status.attackName == "Low2")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack2));
	}
	else if (status.attackName == "Low3")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack3));
	}
	else if (status.attackName == "Low4")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack4));
	}
	else if (status.attackName == "Low5")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack5));
	}
	else if (status.attackName == "Low6")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack6));
	}
	else if (status.attackName == "Low7")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack7));
	}
	else if (status.attackName == "Low8")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack8));
	}
	//�`���[�W�i���U�������Ă�����`���[�W�i���U���`���[�g���A�����N���A������
	else if (status.attackName == "UpCharge" ||
		status.attackName == "MiddleCharge" ||
		status.attackName == "DownCharge")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kChargePhysicalAttack));
	}
	else if (status.attackName == "Upper")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kUpperAttack));
	}
	else if (status.attackName == "Stan")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDekaKick));
	}
	else if (status.attackName == "LegSweep")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kCycloneKick));
	}
	else if (status.attackName == "Energy1")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kEnergyAttack));
	}
	else if (status.attackName == "EnergyCharge")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kChargeEnergyAttack));
	}
	else if (status.attackName == "Teleportation")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kChaseAttack));
	}
	//�K�E�Z���󂯂Ă�����K�E�Z�`���[�g���A�����N���A������
	else if (status.attackKind == Character::AttackKind::kLaser)
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kSpecialAttack));
	}
}

void CharacterStateBase::SuccessTutorial(int tutorialNumber)
{
	//�Q�[�����[�h���`���[�g���A���łȂ���Ζ�������
	if (m_pCharacter->m_pBattleManager->GetGameKind() != GameManagerBase::GameKind::kTutorial) return;

	auto manager = std::dynamic_pointer_cast<TutorialManager>(m_pCharacter->m_pBattleManager);

	auto clearKind = static_cast<TutorialManager::TutorialSuccessKind>(tutorialNumber);

	manager->SuccessTutorial(clearKind);
}

void CharacterStateBase::SetDrawFlag(bool flag)
{
	m_pCharacter->m_isDrawCharacter = flag;
}

int CharacterStateBase::GetNextHitReactionKind(std::shared_ptr<Attack> attack)
{
	auto status = attack->GetStatus();

	//���������K�[�h�ł����Ԃɂ���̂��𒲂ׂ�
	bool isGuard = false;

	//�K�[�h�ł����Ԃ��𒲂ׂ�
	for (auto& hitReaction : kCanGuardHitReactionKind)
	{
		if (m_pCharacter->m_nowHitReaction == hitReaction)
		{
			isGuard = true;
		}
	}

	//�G����둤�ɂ���Ȃ��
	if (!m_pCharacter->IsFrontTarget())
	{
		//��΂ɃK�[�h�ł��Ȃ�
		isGuard = false;
	}

	//�K�[�h�ł����Ԃɂ��Ȃ����
	if (!isGuard)
	{
		//�K�[�h���s
		return static_cast<int>(kHitKindMap.at(status.attackHitKind));
	}

	//�Ō��U���̏ꍇ
	if (status.attackKind == Character::AttackKind::kPhysical)
	{
		//��K�[�h�����Ă���ꍇ
		if (m_guardKind == CharacterGuardKind::kUpGuard)
		{
			//���ɐ�����΂��U���Ȃ�
			if (status.attackHitKind == Character::AttackHitKind::kDownBurst)
			{
				//��K�[�h�̃`���[�g���A�����N���A������
				SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kUpGuard));

				//�K�[�h����
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//��U�������U���Ȃ�
			else if (status.attackHitKind == Character::AttackHitKind::kLow ||
				status.attackHitKind == Character::AttackHitKind::kMiddle)
			{
				//�U�����󂯂Ă����ԂȂ��
				if (m_kind == CharacterStateKind::kHitAttack)
				{
					//�K�[�h���s
					return static_cast<int>(kHitKindMap.at(status.attackHitKind));
				}
				//�U�����󂯂Ă��Ȃ���ԂȂ�
				else
				{
					//��{�I�ɃK�[�h����������
					return static_cast<int>(Character::HitReactionKind::kGuard);
				}
			}
			//�ق��̍U���͊�{�I�ɃK�[�h���s
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
		//���i�K�[�h�����Ă���ꍇ
		else if (m_guardKind == CharacterGuardKind::kMiddleGuard)
		{
			//���ɐ�����΂��U�������i�X�^���Ȃ��
			if (status.attackHitKind == Character::AttackHitKind::kFarBurst ||
				status.attackHitKind == Character::AttackHitKind::kMiddleStan)
			{
				//�K�[�h����
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//��U�������U���Ȃ��
			else if (status.attackHitKind == Character::AttackHitKind::kLow ||
				status.attackHitKind == Character::AttackHitKind::kMiddle)
			{
				//�U�����󂯂Ă����ԂȂ��
				if (m_kind == CharacterStateKind::kHitAttack)
				{
					//�K�[�h���s
					return static_cast<int>(kHitKindMap.at(status.attackHitKind));
				}
				//�U�����󂯂Ă��Ȃ���ԂȂ�
				else
				{
					//��{�I�ɃK�[�h����������
					return static_cast<int>(Character::HitReactionKind::kGuard);
				}
			}
			//�ق��̍U���̓K�[�h���s
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
		//���i�K�[�h�����Ă���ꍇ
		else if (m_guardKind == CharacterGuardKind::kDownGuard)
		{
			//��ɐ�����΂��U�������i�X�^���Ȃ��
			if (status.attackHitKind == Character::AttackHitKind::kUpBurst ||
				status.attackHitKind == Character::AttackHitKind::kWeakUpBurst ||
				status.attackHitKind == Character::AttackHitKind::kBottomStan)
			{
				//���K�[�h�̃`���[�g���A�����N���A������
				SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDownGuard));

				//�K�[�h����
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//��U�������U���Ȃ��
			else if (status.attackHitKind == Character::AttackHitKind::kLow ||
				status.attackHitKind == Character::AttackHitKind::kMiddle)
			{
				//�U�����󂯂Ă����ԂȂ��
				if (m_kind == CharacterStateKind::kHitAttack)
				{
					//�K�[�h���s
					return static_cast<int>(kHitKindMap.at(status.attackHitKind));
				}
				//�U�����󂯂Ă��Ȃ���ԂȂ�
				else
				{
					//��{�I�ɃK�[�h����������
					return static_cast<int>(Character::HitReactionKind::kGuard);
				}
			}
			//�ق��̍U���̓K�[�h���s
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
		//�K�[�h���Ă��Ȃ��ꍇ
		else
		{
			return static_cast<int>(kHitKindMap.at(status.attackHitKind));
		}
	}
	//�C�e�n�̍U���̏ꍇ
	else if (status.attackKind == Character::AttackKind::kEnergy ||
		status.attackKind == Character::AttackKind::kLaser)
	{
		//�U�����󂯂Ă����ԂȂ��
		if (m_kind == CharacterStateKind::kHitAttack)
		{
			//�K�[�h���s
			return static_cast<int>(kHitKindMap.at(status.attackHitKind));
		}
		//�U�����󂯂Ă��Ȃ���ԂȂ�
		else
		{
			//�K�[�h���Ă�����
			if (m_guardKind == CharacterGuardKind::kUpGuard ||
				m_guardKind == CharacterGuardKind::kMiddleGuard ||
				m_guardKind == CharacterGuardKind::kDownGuard)
			{

				//��{�I�ɃK�[�h����������
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//�K�[�h���Ă��Ȃ�������
			else
			{
				//�K�[�h���s
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
	}
	//�Ԃ���Ɖ��o�ɐ؂�ւ��U����ˌ��U���̏ꍇ(������)
	else if (status.attackKind == Character::AttackKind::kRush ||
		status.attackKind == Character::AttackKind::kAssault)
	{
		//�U�����󂯂Ă����ԂȂ��
		if (m_kind == CharacterStateKind::kHitAttack)
		{
			//�K�[�h���s
			return static_cast<int>(kHitKindMap.at(status.attackHitKind));
		}
		//�U�����󂯂Ă��Ȃ���ԂȂ�
		else
		{
			//�K�[�h���Ă�����
			if (m_guardKind == CharacterGuardKind::kUpGuard ||
				m_guardKind == CharacterGuardKind::kMiddleGuard ||
				m_guardKind == CharacterGuardKind::kDownGuard)
			{
				//�K�[�h�u���C�N��Ԃɂ���
				return static_cast<int>(Character::HitReactionKind::kGuardBreak);
			}
			//�K�[�h���Ă��Ȃ��ꍇ
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
	}
	//�����U��(������)
	else if (status.attackKind == Character::AttackKind::kThrow)
	{
		//��{�I�ɂǂ̏�Ԃł��󂯂�
		return static_cast<int>(kHitKindMap.at(status.attackHitKind));
	}

	//���Ԃ񂱂��܂ŗ��Ȃ��͂������ǈꉞ
	//�x�������̂��߂ɕԂ�l��ݒ肵�Ă���
	return static_cast<int>(kHitKindMap.at(status.attackHitKind));

}
