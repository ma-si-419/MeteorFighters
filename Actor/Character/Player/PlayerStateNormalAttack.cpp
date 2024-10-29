#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
	//�i���U���̍U�������t���[��
	constexpr int kPhysicalAttackLifeTime = 2;
	//�i���U���̔���̑傫��
	constexpr float kPhysicalAttackRadius = 5.0f;
	//�C�e�U���̍U�����肪�c�鎞��
	constexpr int kEnergyAttackLifeTime = 240;
	//�C�e�U���̔���̑傫��
	constexpr float kEnergyAttackRadius = 3.0f;
	//�h���U���̃X�e�B�b�N�̌X��
	constexpr int kPhysicalAttackStickPower = 500;
	//����͂̋��U��
	const std::string kUpperAttackName = "Upper";
	//�����͂̋��U��
	const std::string kStanAttackName = "Stan";
	//�����͂̋��U��
	const std::string kLegSweepAttackName = "LegSweep";
}

PlayerStateNormalAttack::PlayerStateNormalAttack(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_nowAttackName("empty"),
	m_nextAttackName("empty"),
	m_isNextAttack(false)
{
}
void PlayerStateNormalAttack::SetAttack(std::string attackName)
{
	m_nowAttackName = attackName;
}
void PlayerStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kAttack;

	std::string animName = m_pPlayer->GetNormalAttackData(m_nowAttackName).animationName;

	CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(animName));

	m_pPlayer->ChangeAnim(anim, false);

	MyEngine::Vector3 shiftVec = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

	shiftVec *= kPhysicalAttackRadius;

	shiftVec.y = 0;

	m_moveTargetPos = GetEnemyPos() + shiftVec;

}

void PlayerStateNormalAttack::Update()
{
	//State�ɂ���t���[�����𐔂��Ă���
	m_time++;

	auto attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

	MyEngine::Vector3 velo;

	//���̍U���Ɉڍs�ł���t���[���ł����
	if (m_time >= attackData.cancelFrame)
	{
		//���ɍU�����s���ƌ��肵�Ă����
		if (m_isNextAttack)
		{
			//���Ԃ̃��Z�b�g
			m_time = 0;
			//���ɍs���U���̐ݒ�
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";

			//�U�����̍X�V
			attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

			//�u�Ԉړ�����U���ł���Ώu�Ԉړ�����
			if (attackData.isTeleportation)
			{
				//���̍U�������t���[�����ɓG������ꏊ���v�Z����
				MyEngine::Vector3 teleportationPos = GetEnemyPos() + (GetEnemyVelo() * (attackData.attackFrame));
				//�u�Ԉړ���ɍU���̍U���͈͕���������𑫂�
				MyEngine::Vector3 attackShiftVec = GetEnemyVelo();
				//attackShiftVec.y = 0;

				teleportationPos += attackShiftVec.Normalize() * (kPhysicalAttackRadius);

				SetPlayerPos(teleportationPos);
			}

			//�U�����s��������ݒ肷��
			LocalPos attackPos;

			attackPos.SetCenterPos(GetEnemyPos());

			attackPos.SetFrontPos(GetEnemyPos() + (m_pPlayer->GetPos() - GetEnemyPos()).Normalize());

			attackPos.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, kPhysicalAttackRadius));

			MyEngine::Vector3 shiftVec = (attackPos.GetWorldPos() - m_pPlayer->GetPos()).Normalize();

			shiftVec *= kPhysicalAttackRadius;

			shiftVec.y = 0;

			m_moveTargetPos = GetEnemyPos() + shiftVec;
			CharacterBase::AnimKind anim = static_cast<CharacterBase::AnimKind>(GetAttackAnimKind(attackData.animationName));

			m_pPlayer->ChangeAnim(anim, false);

			m_isNextAttack = false;
		}
	}


	//�U���̍��v�t���[���𒴂�����
	if (m_time >= attackData.totalFrame)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}

	//�i���U���Ȃ��
	if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
	{
		//�U�����o���t���[���܂ł͈ړ�����
		if (m_time <= attackData.attackFrame)
		{

			MyEngine::Vector3 dir;

			dir = (m_moveTargetPos - m_pPlayer->GetPos()).Normalize();

			float speed = attackData.moveSpeed;

			//�ړ��������s���������W�܂ł̋��������������
			if ((m_moveTargetPos - m_pPlayer->GetPos()).Length() < speed * attackData.attackFrame)
			{
				//�ړ�������␳����
				speed = (m_moveTargetPos - m_pPlayer->GetPos()).Length() / attackData.attackFrame;
			}

			velo = dir * speed;
		}
	}
	//�C�e�U���Ȃ��
	else if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
	{
		//���R�Ɉړ��ł���悤�ɂ���
		MyEngine::Input& input = MyEngine::Input::GetInstance();
		//�ړ������x�N�g��
		MyEngine::Vector3 dir;

		//�X�e�B�b�N�̏��擾
		MyEngine::Input::StickInfo stick = input.GetStickInfo();

		//���X�e�B�b�N�̌X���擾
		MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
		//�ړ��x�N�g����0����Ȃ����
		if (leftStickDir.SqLength() > 0.001)
		{
			//�ړ�����
			dir = leftStickDir.Normalize();

			//�G�l�~�[�̕����Ɉړ���������]������
			float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
			float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

			float angle = std::atan2f(vX, vZ);

			MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			dir = dir.MatTransform(mat);

			//�ړ������ɃX�s�[�h��������
			velo = dir * GetSpeed();
		}
		//�W�����v�{�^���������ꂽ��
		if (input.IsPress("RB"))
		{
			velo.y = GetSpeed();
		}
		//���~�{�^���������ꂽ��
		else if (input.IsPushTrigger(true))
		{
			velo.y = -GetSpeed();
		}
	}

	//�U�����o���t���[���ɂȂ�����
	if (m_time == attackData.attackFrame)
	{
		CharacterBase::AttackData attack;

		attack.damage = static_cast<int>(attackData.damageRate * m_pPlayer->GetPower());
		attack.attackHitKind = attackData.attackHitKind;
		attack.isPlayer = true;
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

		m_pPlayer->CreateAttack(attack);
	}

	//���̍U�����s�������肷��
	if (!m_isNextAttack)
	{
		//�i���U���Ȃ�
		if (attackData.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			//X�{�^���Ŏ��̒ʏ�U���Ɉڍs����
			if (MyEngine::Input::GetInstance().IsTrigger("X"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
			}
			//Y�{�^���Ŕh���U�����o��
			else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
			{
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
		}
		//�C�e�U���Ȃ�
		if (attackData.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			//Y�{�^���Ŏ��̍U���Ɉڍs����
			if (MyEngine::Input::GetInstance().IsTrigger("Y"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
			}
		}
	}

	SetPlayerVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Attack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void PlayerStateNormalAttack::Exit()
{
}

void PlayerStateNormalAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}