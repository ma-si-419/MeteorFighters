#include "CharacterStateBase.h"
#include "CharacterStateHitAttack.h"
#include "CharacterStateButtonBashing.h"
#include "Character.h"
#include "GameManagerBase.h"
#include "TutorialManager.h"
#include "Attack.h"
#include "Input.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cmath>

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

	//�U�����󂯂����ɏo���G�t�F�N�g
	const std::map<Character::AttackHitKind, Effect::EffectKind> kPhysicalAttackHitEffectMap =
	{
		{Character::AttackHitKind::kLow,Effect::EffectKind::kLowHit},
		{Character::AttackHitKind::kMiddle,Effect::EffectKind::kMiddleHit},
		{Character::AttackHitKind::kWeakUpBurst,Effect::EffectKind::kMiddleHit},
		{Character::AttackHitKind::kUpBurst,Effect::EffectKind::kHighHit},
		{Character::AttackHitKind::kDownBurst,Effect::EffectKind::kHighHit},
		{Character::AttackHitKind::kFarBurst,Effect::EffectKind::kHighHit},
		{Character::AttackHitKind::kBottomStan,Effect::EffectKind::kMiddleHit},
		{Character::AttackHitKind::kMiddleStan,Effect::EffectKind::kMiddleHit}
	};

	//�q�b�g�G�t�F�N�g���c������
	constexpr int kHitEffectLifeTime = 30;

	//�K�[�h�ł���\����������
	const std::vector<Character::HitReactionKind> kCanGuardHitReactionKind =
	{
		Character::HitReactionKind::kGuard,
		Character::HitReactionKind::kLow,
		Character::HitReactionKind::kMiddle,
		Character::HitReactionKind::kNone
	};
}

CharacterStateBase::CharacterStateBase(std::shared_ptr<Character> character)
{
	m_pCharacter = character;
	m_pManager = m_pCharacter->m_pBattleManager;
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
	std::shared_ptr<CharacterStateHitAttack> nextState = std::make_shared<CharacterStateHitAttack>(m_pCharacter);

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
		if (status.attackKind == Character::AttackKind::kBeam ||
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
		if (status.attackKind == Character::AttackKind::kBeam)
		{
			//�K�[�h�Ɋ֌W�Ȃ��G�t�F�N�g���Đ�����
			effectKind = Effect::EffectKind::kHighHit;
		}
	}
	//�K�[�h���Ă��Ȃ��ꍇ
	else
	{
		//�W���X�g�K�[�h�ł��Ă����ꍇ
		if (m_guardKind == CharacterGuardKind::kJustGuard)
		{
			//�u�Ԉړ��̃G�t�F�N�g���Đ�����
			//effectKind = Effect::EffectKind::kTeleportaion;
		}
		//�����Ԃ̏ꍇ
		else if (m_guardKind == CharacterGuardKind::kDodge)
		{
			//���ɉ����G�t�F�N�g���Đ����Ȃ�
		}
		else
		{
			//�U���ɍ��킹���G�t�F�N�g���Đ�����

			//�i���U���̏ꍇ
			if (status.attackKind == Character::AttackKind::kPhysical)
			{
				effectKind = kPhysicalAttackHitEffectMap.at(status.attackHitKind);
			}
			//�C�e�U���̏ꍇ
			else if (status.attackKind == Character::AttackKind::kEnergy)
			{
				effectKind = Effect::EffectKind::kLowHit;
			}
			//���[�U�[�U���̏ꍇ
			else if (status.attackKind == Character::AttackKind::kBeam)
			{
				effectKind = Effect::EffectKind::kHighHit;
			}
		}
	}

	//�K�[�h�`���[�g���A�����N���A������
//	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kGuard));

	//�G�t�F�N�g���ݒ肳��Ă����
	if (effectKind != Effect::EffectKind::kNone)
	{
		std::shared_ptr<Effect> hitEffect = std::make_shared<Effect>(effectKind);
		hitEffect->SetLifeTime(kHitEffectLifeTime);
		m_pCharacter->m_pBattleManager->GetEffectManagerPointer()->Entry(hitEffect, m_pCharacter->GetPos());

		MyEngine::Vector3 rotation;

		float vX = attack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = attack->GetPos().z - m_pCharacter->GetPos().z;

		rotation.y = std::atan2f(vX, vZ);

		hitEffect->SetRotation(rotation);
	}

	nextState->HitAttack(static_cast<int>(hitReaction));

	//�̗͂����炷
	m_pCharacter->SubHp(damage);

	ChangeState(nextState);
}

void CharacterStateBase::SuccessTutorial(int tutorialNumber)
{
	//�Q�[�����[�h���`���[�g���A���łȂ���Ζ�������
	if (m_pCharacter->m_pBattleManager->GetGameKind() != GameManagerBase::GameKind::kTutorial) return;

	auto manager = std::dynamic_pointer_cast<TutorialManager>(m_pCharacter->m_pBattleManager);

	auto clearKind = static_cast<TutorialManager::TutorialSuccessKind>(tutorialNumber);

	manager->SuccessTutorial(clearKind);
}

int CharacterStateBase::GetNextHitReactionKind(std::shared_ptr<Attack> attack)
{
	auto status = attack->GetStatus();


	//���������K�[�h�ł����Ԃɂ���̂��𒲂ׂ�
	bool isGuard = false;

	for (auto& hitReaction : kCanGuardHitReactionKind)
	{
		if (m_pCharacter->m_nowHitReaction == hitReaction)
		{
			isGuard = true;
		}
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
		status.attackKind == Character::AttackKind::kBeam)
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
}
