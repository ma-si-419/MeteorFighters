#include "CharacterStateNormalAttack.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"
#include <cmath>
#include "Effect.h"

namespace
{
	//���̍U���̓��͔�����͂��߂�t���[��
	constexpr float kNextAttackInputTime = 5.0f;
	//�i���U���̍U�������t���[��
	constexpr int kPhysicalAttackLifeTime = 2;
	//�i���U���̔���̑傫��
	constexpr float kPhysicalAttackRadius = 5.0f;
	//�i���U���ōŒ������鋗��
	constexpr float kPhysicalAttackNearLange = 8.0f;
	//�u�Ԉړ��U���ł��炷�傫��
	constexpr float kTeleportationShiftLange = 10.0f;
	//�C�e�U���̍U�����肪�c�鎞��
	constexpr int kEnergyAttackLifeTime = 240;
	//�C�e�U���̔���̑傫��
	constexpr float kEnergyAttackRadius = 3.0f;
	//�h���U���̃X�e�B�b�N�̌X��
	constexpr int kPhysicalAttackStickPower = 500;
	//���ߍU�����s���Ă���Ƃ��̌o�ߎ��Ԃ̌o�ߑ��x
	constexpr float kOnChargeStateTimeCountSpeed = 0.1f;
	//���ߍU�����s���Ă���Ƃ��̃A�j���Đ����x
	constexpr float kOnChargeAnimPlaySpeed = 0.1f;
	//���ߍU���̒��߂���}�b�N�X�̃t���[����
	constexpr float kChargeMaxTime = kOnChargeStateTimeCountSpeed * 40;
	//�u�Ԉړ��̃G�t�F�N�g��\�����鎞��
	constexpr int kTeleportationEffectTime = 20;
	//�u�Ԉړ����ɃJ�������~�߂鎞��
	constexpr int kTeleportationCameraStopTime = 18;
	//����͂̋��U��
	const std::string kUpperAttackName = "Upper";
	//�����͂̋��U��
	const std::string kStanAttackName = "Stan";
	//�����͂̋��U��
	const std::string kLegSweepAttackName = "LegSweep";
	//����͂̂��ߍU��
	const std::string kUpChargeAttack = "UpCharge";
	//�����͂̂��ߍU��
	const std::string kMiddleChargeAttack = "MiddleCharge";
	//�����͂̂��ߍU��
	const std::string kDownChargeAttack = "DownCharge";
	//�C�e�̂��ߍU��
	const std::string kEnergyChargeAttack = "EnergyCharge";
	//�u�Ԉړ�����U��
	const std::string kTeleportationAttack = "Teleportation";
}

CharacterStateNormalAttack::CharacterStateNormalAttack(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_nowAttackName("empty"),
	m_nextAttackName("empty"),
	m_isNextAttack(false),
	m_isCharge(false),
	m_attackKey("empty"),
	m_chargeTime(0.0f),
	m_isAttacked(false),
	m_isNextCharge(false),
	m_chaseAttackNum(0)
{
}
void CharacterStateNormalAttack::SetAttack(std::string key, std::string attackName)
{
	//�����ꂽ�L�[��ۑ����Ă���
	m_attackKey = key;

	m_nowAttackName = attackName;
}
void CharacterStateNormalAttack::SetAttackVelo(MyEngine::Vector3 velo)
{
	m_firstAttackMoveVec = velo;
}
void CharacterStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kNormalAttack;

	//�ݒ肳�ꂽ�U���̃A�j���[�V�����擾
	std::string animName = m_pCharacter->GetNormalAttackData(m_nowAttackName).animationName;

	CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(animName));

	//�A�j���[�V�����̕ύX
	m_pCharacter->ChangeAnim(anim, false);

	//�����������̐ݒ�
	MyEngine::Vector3 shiftVec = (GetTargetPos() - m_pCharacter->GetPos()).Normalize();

	shiftVec *= kPhysicalAttackNearLange;

	shiftVec.y = 0;

	m_moveTargetPos = GetTargetPos() + shiftVec;

	m_pCharacter->LookTarget();

	//�ŏ��̍U�����`���[�W�ł��邩�ǂ�����ݒ肷��
	if (m_nowAttackName == kUpChargeAttack ||
		m_nowAttackName == kMiddleChargeAttack ||
		m_nowAttackName == kDownChargeAttack)
	{
		m_isCharge = true;
	}

}

void CharacterStateNormalAttack::Update()
{
	//�`���[�W���̓t���[���𐔂��鑬�x��ς���
	if (m_isCharge)
	{
		m_time += kOnChargeStateTimeCountSpeed;
		//�ǂ̂��炢�`���[�W�������𐔂���
		m_chargeTime += kOnChargeStateTimeCountSpeed;
		//�}�b�N�X�܂Œ��߂���`���[�W����߂�
		if (m_time > kChargeMaxTime)
		{
			m_isCharge = false;
		}
		//�A�j���[�V�����̍Đ����x���ς���
		m_pCharacter->SetAnimPlaySpeed(kOnChargeAnimPlaySpeed);
	}
	//�`���[�W���Ă��Ȃ����
	else
	{
		//State�ɂ���t���[�����𐔂��Ă���
		m_time++;
		//�A�j���[�V�����̍Đ����x��ʏ�ɂ���
		m_pCharacter->SetAnimPlaySpeed();
	}

	//�{�^���������Ă���Ԃ̓`���[�W�𑱂���
	if (m_isCharge)
	{
		//�{�^���������ꂽ��`���[�W����߂�
		if (!MyEngine::Input::GetInstance().IsPress(m_attackKey))
		{
			m_isCharge = false;
		}
	}

	auto attackData = m_pCharacter->GetNormalAttackData(m_nowAttackName);

	MyEngine::Vector3 velo;

	//�U���̍��v�t���[���𒴂�����
	if (m_time >= attackData.totalFrame)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
		return;
	}

	//���̍U���Ɉڍs�ł���t���[���ł����
	if (m_time >= attackData.cancelFrame)
	{
		//���ɍU�����s���ƌ��肵�Ă����
		if (m_isNextAttack)
		{
			//���̍U�����Ȃ��U����������U�����s��Ȃ�
			if (m_nextAttackName == "None") return;

			//���̍U���̏��
			CharacterBase::NormalAttackData nextAttack = m_pCharacter->GetNormalAttackData(m_nextAttackName);

			//���̍U�����o�������ɓG�̏�Ԃ�����΍��̓G�̂����ԂƏƂ炵���킹��
			CharacterBase::HitReactionKind enemyHitReaction = static_cast<CharacterBase::HitReactionKind>(GetTargetHitReaction());

			if (nextAttack.targetHitReaction == "��")
			{
				//�����ꂶ��Ȃ����
				if (enemyHitReaction != CharacterBase::HitReactionKind::kMiddle)return;
			}
			else if (nextAttack.targetHitReaction == "�������")
			{
				//������я�ԂłȂ���΂���Ȃ����
				bool flag = false;

				//���̒��̂ǂꂩ��ʂ��OK
				if (enemyHitReaction != CharacterBase::HitReactionKind::kUpBurst)
				{
					flag = true;
				}
				else if (enemyHitReaction != CharacterBase::HitReactionKind::kFarBurst)
				{
					flag = true;
				}
				else if (enemyHitReaction != CharacterBase::HitReactionKind::kDownBurst)
				{
					flag = true;
				}

				//1���ʂ��Ă��Ȃ���΍U�����o���Ȃ�
				if (!flag) return;
			}

			//�u�Ԉړ�����U���ł���Ώu�Ԉړ�����
			if (nextAttack.isTeleportation)
			{
				//���̍U�����x�{�^���ɂ��ǌ���
				if (m_attackKey == "Y")
				{
					//����ȏ�ǌ��ł��Ȃ����
					if (m_chaseAttackNum >= m_pCharacter->GetChaseNum()) return;
					//�ǌ��ł���̂Ȃ�ǌ������񐔂𑝂₷
					m_chaseAttackNum++;
				}

				//�ړ��O�ɏu�Ԉړ��̃G�t�F�N�g��ݒ肷��
				std::shared_ptr<Effect> effect =  std::make_shared<Effect>(Effect::EffectKind::kTeleportaion);

				effect->SetPos(m_pCharacter->GetPos());

				effect->SetLifeTime(kTeleportationEffectTime);

				EntryEffect(effect);

				//�J���������������~�߂�
				StopCamera(kTeleportationCameraStopTime);

				//���̍U�������t���[�����ɓG������ꏊ���v�Z����
				MyEngine::Vector3 teleportationPos = GetTargetPos() + (GetTargetVelo() * (static_cast<float>(nextAttack.attackFrame)));
				//�u�Ԉړ���ɍU���̍U���͈͕���������𑫂�
				MyEngine::Vector3 attackShiftVec = GetTargetVelo();

				teleportationPos += attackShiftVec.Normalize() * (kTeleportationShiftLange);

				attackShiftVec.y = 0;

				teleportationPos += attackShiftVec.Normalize() * (kPhysicalAttackRadius);

				SetCharacterPos(teleportationPos);
			}

			//���Ԃ̃��Z�b�g
			m_time = 0;
			//�U�����s���Ă��Ȃ���Ԃɕω�
			m_isAttacked = false;
			//���ɍs���U���̐ݒ�
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";
			m_firstAttackMoveVec = MyEngine::Vector3(0, 0, 0);


			//���̍U�����`���[�W�ł��邩�ǂ���
			m_isCharge = m_isNextCharge;
			m_isNextCharge = false;

			//�U�����s��������ݒ肷��
			MyEngine::LocalPos attackPos;

			//���[�J�����W�̒��S��ݒ�
			attackPos.SetCenterPos(GetTargetPos());
			//���[�J�����W�̐��ʍ��W��ݒ�
			attackPos.SetFrontPos(GetTargetPos() + (m_pCharacter->GetPos() - GetTargetPos()).Normalize());
			//���[�J�����W��ݒ�
			attackPos.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, kPhysicalAttackRadius));

			MyEngine::Vector3 shiftVec = (attackPos.GetWorldPos() - m_pCharacter->GetPos()).Normalize();

			shiftVec *= kPhysicalAttackNearLange;

			shiftVec.y = 0;

			m_moveTargetPos = GetTargetPos() + shiftVec;
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(nextAttack.animationName));

			m_pCharacter->ChangeAnim(anim, false);

			m_isNextAttack = false;

			m_pCharacter->LookTarget();

			//�U�����̍X�V
			attackData = nextAttack;

		}
	}

	//�i���U���Ȃ��
	if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
	{
		//�U�����o���t���[���܂ł͈ړ�����
		if (m_time <= attackData.attackFrame)
		{

			MyEngine::Vector3 dir;

			dir = (m_moveTargetPos - m_pCharacter->GetPos()).Normalize();

			//�v���C���[�̈ړ����x
			float speed = attackData.moveSpeed;

			//�`���[�W���ł����
			if (m_isCharge)
			{
				//�ړ����x���A�j���[�V�����̍Đ����x�Ɠ����{���ɂ���
				speed *= kOnChargeAnimPlaySpeed;
			}

			//�ړ��������s���������W�܂ł̋��������������
			if ((m_moveTargetPos - m_pCharacter->GetPos()).Length() < speed * attackData.attackFrame)
			{
				//�ړ�������␳����
				speed = (m_moveTargetPos - m_pCharacter->GetPos()).Length() / attackData.attackFrame;
			}

			velo = dir * speed;

			//�����ݒ肳�ꂽ�ړ��x�N�g���������
			if (m_firstAttackMoveVec.SqLength() > 0)
			{
				//���̈ړ��x�N�g���ɂ���
				velo = m_firstAttackMoveVec;
			}

		}
	}
	//�C�e�U���Ȃ��
	else if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
	{
		//���R�Ɉړ��ł���悤�ɂ���
		MyEngine::Input& input = MyEngine::Input::GetInstance();
		//�ړ������x�N�g��
		MyEngine::Vector3 dir;

		MyEngine::Vector3 leftStickDir;

		if (m_isPlayer)
		{
			//�X�e�B�b�N�̏��擾
			MyEngine::Input::StickInfo stick = input.GetStickInfo();

			//���X�e�B�b�N�̌X���擾
			leftStickDir = MyEngine::Vector3(stick.leftStickX, 0, -stick.leftStickY);
		}
		//�ړ��x�N�g����0����Ȃ����
		if (leftStickDir.SqLength() > 0.001)
		{
			//�ړ�����
			dir = leftStickDir.Normalize();

			//�G�l�~�[�̕����Ɉړ���������]������
			float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
			float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

			float angle = std::atan2f(vX, vZ);

			MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			dir = dir.MatTransform(mat);

			//�ړ������ɃX�s�[�h��������
			velo = dir * GetSpeed();
		}
		//�W�����v�{�^���������ꂽ��
		if (m_isPlayer && input.IsPress("RB"))
		{
			velo.y = GetSpeed();
		}
		//���~�{�^���������ꂽ��
		else if (m_isPlayer && input.IsPushTrigger(true))
		{
			velo.y = -GetSpeed();
		}
	}

	//�U�����o���t���[���ɂȂ�����
	if (m_time > attackData.attackFrame &&
		!m_isAttacked)
	{
		m_isAttacked = true;
		CharacterBase::AttackData attack;


		//�`���[�W�������̃_���[�W
		if (m_chargeTime > 0.0f)
		{
			//�`���[�W�������Ԃɂ���ă_���[�W���㏸������
			attack.damage = static_cast<int>((attackData.damageRate + m_chargeTime) * m_pCharacter->GetPower());
		}
		//�`���[�W����Ă��Ȃ��Ƃ��̃_���[�W
		else
		{
			attack.damage = static_cast<int>(attackData.damageRate * m_pCharacter->GetPower());
		}

		attack.attackHitKind = attackData.attackHitKind;
		if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
		{
			attack.isPlayer = true;
		}
		else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
		{
			attack.isPlayer = false;
		}
		attack.speed = attackData.attackMoveSpeed;

		//�i���U���Ȃ�
		if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			attack.lifeTime = kPhysicalAttackLifeTime;
			attack.radius = kPhysicalAttackRadius;
		}
		//�C�e�U���Ȃ�
		else if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			attack.lifeTime = kEnergyAttackLifeTime;
			attack.radius = kEnergyAttackRadius;
		}
		attack.attackKind = attackData.attackKind;

		//�U�����쐬
		m_pCharacter->CreateAttack(attack);
	}

	//���̍U�����s�������肷��
	if (m_time < attackData.cancelFrame &&
		m_time > kNextAttackInputTime)
	{
		//�i���U���Ȃ�
		if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			//X�{�^���Ŏ��̒ʏ�U���Ɉڍs����
			if (MyEngine::Input::GetInstance().IsTrigger("X"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
				m_attackKey = "X";
			}
			//Y�{�^���Ŕh���U�����o��
			else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
			{
				//�O�̍U�����ʏ�U���Ȃ�h���U���Ɉڂ��
				if (m_attackKey == "X")
				{
					//Y�{�^���̔h���Z�͊�{�`���[�W���ł���
					m_isNextCharge = true;
					//����͂��Ȃ���̔h���U��
					if (MyEngine::Input::GetInstance().GetStickInfo().leftStickY < -kPhysicalAttackStickPower)
					{
						m_nextAttackName = kUpperAttackName;
					}
					else if (MyEngine::Input::GetInstance().GetStickInfo().leftStickY > kPhysicalAttackStickPower)
					{
						m_nextAttackName = kLegSweepAttackName;
					}
					else
					{
						m_nextAttackName = kStanAttackName;
					}
					m_isNextAttack = true;
				}
				//�G��������я�Ԃ̎��ɔh���U�������Ă�����
				CharacterBase::HitReactionKind kind = static_cast<CharacterBase::HitReactionKind>(GetTargetHitReaction());
				if (kind == CharacterBase::HitReactionKind::kUpBurst ||
					kind == CharacterBase::HitReactionKind::kFarBurst ||
					kind == CharacterBase::HitReactionKind::kDownBurst)
				{
					m_nextAttackName = kTeleportationAttack;
					//�u�Ԉړ��U�������`���[�W���ł��Ȃ�
					m_isNextCharge = false;
					m_isNextAttack = true;
				}
				m_attackKey = "Y";
			}
		}
		//�C�e�U���Ȃ�
		if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			//Y�{�^���Ŏ��̍U���Ɉڍs����
			if (MyEngine::Input::GetInstance().IsRelease("Y"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
			}
		}
	}

	SetCharacterVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Attack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateNormalAttack::Exit()
{
}