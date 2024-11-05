#include "CharacterBase.h"
#include "SphereColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include <cassert>
#include "GameManager.h"
#include "LocalPos.h"

namespace
{
	constexpr float kCharacterHeight = 4.5f;
	constexpr float kCharacterRadius = 5.0f;

	constexpr float kAttackPopPos = kCharacterRadius * 3.0f;

	constexpr float kAttackMaxShiftY = kCharacterHeight * 0.5f;

	const MyEngine::Vector3 kAttackPos(0.0f, 0.0f, kAttackPopPos);

	constexpr float kAnimBlendSpeed = 0.08f;

	constexpr float kAnimPlaySpeed = 1.0f;

	const std::map<std::string, CharacterBase::AttackHitKind> kAttackHitKindMap =
	{
		{"��",CharacterBase::AttackHitKind::kLow},
		{"��",CharacterBase::AttackHitKind::kMiddle},
		{"���������",CharacterBase::AttackHitKind::kFarBurst},
		{"�㐁�����",CharacterBase::AttackHitKind::kUpBurst},
		{"���������",CharacterBase::AttackHitKind::kDownBurst},
		{"���X�^��",CharacterBase::AttackHitKind::kMiddleStan},
		{"���X�^��",CharacterBase::AttackHitKind::kBottomStan}
	};

	const std::map<std::string, CharacterBase::AttackKind> kSpecialAttackKindMap =
	{
		{"�r�[��",CharacterBase::AttackKind::kBeam},
		{"�i��",CharacterBase::AttackKind::kRush},
		{"�C�e",CharacterBase::AttackKind::kEnergy},
		{"����",CharacterBase::AttackKind::kThrow},
		{"�̓�����",CharacterBase::AttackKind::kAssault}
	};

	const std::map<std::string, CharacterBase::AnimKind> kAttackAnimKindMap =
	{
		{"LowAttack1",CharacterBase::AnimKind::kLowAttack1},
		{"LowAttack2",CharacterBase::AnimKind::kLowAttack2},
		{"LowAttack3",CharacterBase::AnimKind::kLowAttack3},
		{"LowAttack4",CharacterBase::AnimKind::kLowAttack4},
		{"LowAttack5",CharacterBase::AnimKind::kLowAttack5},
		{"LowAttack6",CharacterBase::AnimKind::kLowAttack6},
		{"LowAttack7",CharacterBase::AnimKind::kLowAttack7},
		{"LowAttack8",CharacterBase::AnimKind::kLowAttack8},
		{"EnergyAttackRight",CharacterBase::AnimKind::kEnergyAttackRight},
		{"EnergyAttackLeft",CharacterBase::AnimKind::kEnergyAttackLeft},
		{"UpperAttack",CharacterBase::AnimKind::kUpperAttack},
		{"StanAttack",CharacterBase::AnimKind::kStanAttack},
		{"LegSweepAttack",CharacterBase::AnimKind::kLegSweepAttack},
		{"UpChargeAttack",CharacterBase::AnimKind::kUpChargeAttack},
		{"MiddleChargeAttack",CharacterBase::AnimKind::kMiddleChargeAttack},
		{"DownChargeAttack",CharacterBase::AnimKind::kDownChargeAttack},
		{"EnergyChargeAttack",CharacterBase::AnimKind::kEnergyChargeAttack}
	};

}

CharacterBase::CharacterBase(ObjectTag tag, CharacterKind kind) :
	Actor(tag, ColliderData::Kind::kSphere),
	m_modelHandle(-1),
	m_attachAnim(-1),
	m_totalAnimTime(-1),
	m_nowPlayAnimTime(-1),
	m_animPlaySpeed(0),
	m_isLoop(false),
	m_characterKind(kind),
	m_lastAnim(-1),
	m_animBlendRate(1.0f),
	m_animBlendSpeed(kAnimBlendSpeed),
	m_isEndAnimationBlend(true),
	m_isEndAnim(false)
{
	auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);

	sphereData->m_radius = kCharacterRadius;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	SetNormalAttackData(data);
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::SetGameManager(std::shared_ptr<GameManager> manager)
{
	m_pGameManager = manager;
}

MyEngine::Vector3 CharacterBase::GetPos()
{
	return m_rigidbody.GetPos();
}

MyEngine::Vector3 CharacterBase::GetVelo()
{
	return m_rigidbody.GetVelo();
}

void CharacterBase::ChangeAnim(AnimKind animKind, bool loop)
{
	ChangeAnim(animKind, loop, kAnimBlendSpeed);
}

void CharacterBase::ChangeAnim(AnimKind animKind, bool loop, float blendSpeed)
{

	//�ЂƂO�̃A�j���[�V�����������Ă���
	if (m_lastAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_lastAnim);
	}

	//�A�j���[�V�����ԍ���ݒ�
	int animNumber = static_cast<int>(animKind);
	//���Đ����Ă����A�j���[�V������ۑ�����
	m_lastAnim = m_attachAnim;
	//������Đ�����A�j���[�V������ݒ肷��
	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	//������Đ�����A�j���[�V������������ۑ�����
	m_playAnimKind = animKind;
	//�A�j���[�V�����̍��v�Đ����Ԃ�ݒ肷��
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	//�������܂ōĐ����Ă����A�j���[�V�����̍Đ����Ԃ�ۑ�����(�u�����h�̍ۂɂ��������Ȃ�Ȃ��悤��)
	m_lastPlayAnimTime = m_nowPlayAnimTime;
	//�Đ����Ԃ����Z�b�g
	m_nowPlayAnimTime = 0;
	//�A�j���[�V�����̍Đ����Ԃ�ݒ�
	m_animPlaySpeed = 1.0f;
	//�A�j���[�V���������[�v���邩�ǂ�����ݒ�
	m_isLoop = loop;
	//�A�j���[�V�����u�����h���̃��Z�b�g
	m_animBlendRate = 0.0f;
	//�A�j���[�V�������u�����h���鑬����ݒ�
	m_animBlendSpeed = blendSpeed;
	//�u�����h���I��������ǂ�����false�ɂ���
	m_isEndAnimationBlend = false;
	//�A�j���[�V�������I������t���O��false�ɂ���
	m_isEndAnim = false;
}

void CharacterBase::PlayAnim()
{
	//�A�j���[�V�����̍Đ��t���[����ݒ�
	MV1SetAttachAnimTime(m_modelHandle, m_lastAnim, m_lastPlayAnimTime);
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_nowPlayAnimTime);
	//�Đ����̃A�j���[�V������i�߂�
	m_nowPlayAnimTime += m_animPlaySpeed;
	//�A�j���[�V�������Ō�܂Ō������ꍇ
	if (m_nowPlayAnimTime > m_totalAnimTime)
	{
		//���[�v����Ȃ�
		if (m_isLoop)
		{
			//�Đ����Ԃ����Z�b�g����
			m_nowPlayAnimTime = 0;
		}
		//���[�v���Ȃ��Ȃ�
		else
		{
			//�A�j���[�V�������I������t���O��true�ɂ���
			m_isEndAnim = true;
		}
	}

	//�u�����h���̒���
	m_animBlendRate += m_animBlendSpeed;

	//�A�j���[�V�����̃u�����h���I�������
	if (m_animBlendRate > 1.0f)
	{
		//�ςȒl�ɂȂ�Ȃ��悤�ɂ���
		m_animBlendRate = 1.0f;
		//�A�j���[�V�����u�����h���I������t���O��true�ɂ���
		m_isEndAnimationBlend = true;
		//�ЂƂO�̃A�j���[�V����������
		m_lastPlayAnimTime = 0;
		MV1DetachAnim(m_modelHandle,m_lastAnim);
	}

	//�A�j���[�V�����̃u�����h
	MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnim, 1.0f - m_animBlendRate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_attachAnim, m_animBlendRate);
}

void CharacterBase::SetAnimPlaySpeed(float speed)
{
	m_animPlaySpeed = speed;
}

void CharacterBase::SetAnimPlaySpeed()
{
	m_animPlaySpeed = kAnimPlaySpeed;
}

bool CharacterBase::IsAnimEnd()
{
	return false;
}

CharacterBase::AttackKind CharacterBase::GetSpecialAttackKind(std::string kind)
{
	return kSpecialAttackKindMap.at(kind);
}

void CharacterBase::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto& item : normalAttackData)
	{
		NormalAttackData pushData;

		pushData.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
		pushData.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
		pushData.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
		pushData.cancelFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kCancelFrame)]);
		pushData.moveSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kMoveSpeed)]);
		pushData.attackMoveSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kAttackMoveSpeed)]);
		pushData.isTeleportation = static_cast<bool>(stoi(item[static_cast<int>(NormalAttackDataSort::kIsTeleportation)]));
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAnimationName)];
		pushData.attackKind = static_cast<AttackKind>(stoi(item[static_cast<int>(NormalAttackDataSort::kAttackKind)]));
		pushData.nextComboName = item[static_cast<int>(NormalAttackDataSort::kNextComboName)];
		pushData.attackHitKind = kAttackHitKindMap.at(item[static_cast<int>(NormalAttackDataSort::kAttackHitKind)]);

		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

void CharacterBase::CreateAttack(AttackData attackData)
{
	//�U���N���X
	std::shared_ptr<Attack> ans;

	//�U���̃^�O
	ObjectTag tag;

	//�v���C���[�ƃG�l�~�[�ǂ���̍U�������f
	if (attackData.isPlayer)
	{
		tag = ObjectTag::kPlayerAttack;
	}
	else
	{
		tag = ObjectTag::kEnemyAttack;
	}
	//�U�����W�̐ݒ�
	MyEngine::Vector3 attackPos;

	//�L�����N�^�[�̐��ʕ����ɍU�����o��
	LocalPos localPos;

	//���g�𒆐S�Ƃ��郍�[�J�����W���쐬
	localPos.SetCenterPos(m_rigidbody.GetPos());

	//���ʕ�����ݒ�
	MyEngine::Vector3 toTarget;
	MyEngine::Vector3 targetPos;
	if (attackData.isPlayer)
	{
		targetPos = m_pGameManager->GetEnemyPos();
		toTarget = targetPos - m_pGameManager->GetPlayerPos();
	}
	else
	{
		targetPos = m_pGameManager->GetPlayerPos();
		toTarget = targetPos - m_pGameManager->GetEnemyPos();
	}

	localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget.Normalize());

	//�ǂ̂��炢���炷����ݒ�
	localPos.SetLocalPos(kAttackPos);

	//���[�J�����W���烏�[���h���W�ɕϊ�
	attackPos = localPos.GetWorldPos();

	//y���W�̂�������l�܂ł͕␳
	float shiftY = toTarget.y;

	if (shiftY > kAttackMaxShiftY)
	{
		shiftY = kAttackMaxShiftY;
	}
	else if (shiftY < -kAttackMaxShiftY)
	{
		shiftY = -kAttackMaxShiftY;
	}

	attackPos.y += shiftY;

	//�U���̃|�C���^�쐬
	ans = std::make_shared<Attack>(tag, attackPos);

	Attack::AttackStatus status;

	status.targetPos = targetPos;
	status.damage = attackData.damage;
	status.speed = attackData.speed;
	status.lifeTime = attackData.lifeTime;
	status.radius = attackData.radius;
	status.attackHitKind = attackData.attackHitKind;
	status.attackKind = attackData.attackKind;

	ans->Init(status);

	m_pGameManager->AddAttack(ans);
}

CharacterBase::NormalAttackData CharacterBase::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}

void CharacterBase::SetFrontPos(MyEngine::Vector3 frontPos)
{
	//���[�J�����W�̑O�������C��
	m_targetLocalPos.SetFrontPos(frontPos);

	//���f���̑O�������C������
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - frontPos).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
}

bool CharacterBase::IsFrontTarget(bool isPlayer)
{

	MyEngine::Vector3 toTargetDir;

	//�v���C���[�Ȃ�
	if (isPlayer)
	{
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetEnemyPos());
	}
	//2P���Ȃ�(��{CPU)
	else
	{
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetPlayerPos());
	}

	toTargetDir = toTargetDir.Normalize();

	if (toTargetDir.z > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
	
	//�x�������̂���
	return true;

}

void CharacterBase::SetDrawPos(MyEngine::Vector3 pos)
{
	pos.y -= kCharacterHeight;

	MV1SetPosition(m_modelHandle,pos.CastVECTOR());
}

CharacterBase::AnimKind CharacterBase::GetAttackAnimKind(std::string animName)
{	
	return kAttackAnimKindMap.at(animName);
}

float CharacterBase::GetRadius()
{
	return kCharacterRadius;
}

void CharacterBase::LookTarget(bool isPlayer)
{
	if (isPlayer)
	{
		MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetEnemyPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	}
	else
	{
		MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetPlayerPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	}
}
