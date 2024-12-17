#include "CharacterBase.h"
#include "SphereColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include "Input.h"
#include "EnemyInput.h"
#include <cassert>
#include <cmath>
#include "GameManager.h"
#include "LocalPos.h"
#include "SceneGame.h"
#include "Game.h"
#include "GameSceneConstant.h"
#include "CharacterStateIdle.h"

namespace
{
	//�L�����N�^�[�̐g���̍���
	constexpr float kCharacterHeight = 4.5f;
	//�U���𔭐���������W
	constexpr float kAttackPopPos = GameSceneConstant::kCharacterRadius * 3.0f;
	//�U�����㉺�ɕ␳����ۂɂǂ��܂ŕ␳���邩
	constexpr float kAttackMaxShiftPosY = kCharacterHeight * 1.5f;
	//��{�I�ȃA�j���[�V�����u�����h�̑���
	constexpr float kAnimBlendSpeed = 0.08f;
	//�A�j���[�V�����̍Đ����x
	constexpr float kAnimPlaySpeed = 1.0f;
	//�����Ă��������ݒ肷��ۂɎg�p����萔
	constexpr float kFrontPosDistance = 3.0f;
	//1P���̃L�����N�^�[�̏������W
	const MyEngine::Vector3 kOnePlayerInitPos(-50, 0, -50);
	//2P���̃L�����N�^�[�̏������W
	const MyEngine::Vector3 kTwoPlayerInitPos(50, 0, 50);

	//�U���̎�ނ��O���t�@�C���̕����񂩂������AttackHitKind�ɕϊ�����ۂɎg�p����
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

	//�K�E�Z�̎�ނ��O���t�@�C���̕����񂩂������AttackKind�ɕϊ�����ۂɎg�p����
	const std::map<std::string, CharacterBase::AttackKind> kSpecialAttackKindMap =
	{
		{"�r�[��",CharacterBase::AttackKind::kBeam},
		{"�i��",CharacterBase::AttackKind::kRush},
		{"�C�e",CharacterBase::AttackKind::kEnergy},
		{"����",CharacterBase::AttackKind::kThrow},
		{"�̓�����",CharacterBase::AttackKind::kAssault}
	};


	//�U���̃A�j���[�V�������O���t�@�C���̕����񂩂������AnimKind�ɕϊ�����ۂɎg�p����
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
		{"EnergyChargeAttack",CharacterBase::AnimKind::kEnergyChargeAttack},
		{"DashAttack",CharacterBase::AnimKind::kDashAttack}
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

	sphereData->m_radius = GameSceneConstant::kCharacterRadius;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	SetNormalAttackData(data);

	m_playAnimKind = AnimKind::kSkyIdle;
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::Init()
{

	//���͏��̐ݒ�
	if (m_playerNumber == CharacterBase::PlayerNumber::kOnePlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(0);
	}
	else if (m_playerNumber == CharacterBase::PlayerNumber::kTwoPlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(1);

		m_pEnemyInput = std::make_shared<EnemyInput>(m_input);

		m_pEnemyInput->SetGameManager(m_pGameManager);
	}


	std::string path;

	//2P�Ȃ�ŏ���Enemy��t����
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		path = "Enemy";
	}

	//�L�����N�^�[�̎�ނɂ���ăp�X��ύX����
	if (m_characterKind == CharacterKind::kMouse)
	{
		path += "Mouse";
	}
	else if (m_characterKind == CharacterKind::kBigBlue)
	{
		path += "BigB";
	}

	path = "data/model/" + path + ".mv1";

	m_modelHandle = MV1LoadModel(path.c_str());

	MV1SetScale(m_modelHandle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));

	Collidable::Init();

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

	//�������W�̌���
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		m_rigidbody.SetPos(kOnePlayerInitPos);
		m_lookPos.SetCenterPos(kOnePlayerInitPos);
		SetFrontPos(kTwoPlayerInitPos);
	}
	else if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_rigidbody.SetPos(kTwoPlayerInitPos);
		m_lookPos.SetCenterPos(kTwoPlayerInitPos);
		SetFrontPos(kOnePlayerInitPos);
	}

	//�����X�e�[�g�̐ݒ�
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();

	//�G���̏���
	if (m_playerNumber == CharacterBase::PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->SetState(m_pState);
	}

}

void CharacterBase::Update()
{

	//�G�l�~�[�̓��͏����X�V����
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->Update();
	}

	//State���ύX����Ă�����State�����ւ���
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;
		
		//�G���̏���
		if (m_playerNumber == CharacterBase::PlayerNumber::kTwoPlayer)
		{
			m_pEnemyInput->SetState(m_pState);
		}
	}

	//�󋵂ɂ���ĕω�����A�b�v�f�[�g
	(this->*m_updateSituationFunc)();

	//���[�J�����W�̒��S���W�̍X�V
	m_lookPos.SetCenterPos(m_rigidbody.GetPos());


	//�`����W�̐ݒ�
	SetDrawPos(m_rigidbody.GetPos());
}

void CharacterBase::Draw()
{
	//���U���g����2P��������
	if (m_pGameManager->GetNowSituation() == GameManager::Situation::kResult ||
		m_pGameManager->GetNowSituation() == GameManager::Situation::kRetry)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer) return;
	}

	//�c���̕`��
	for (auto item : m_afterImageList)
	{
		MV1DrawModel(item.handle);
	}

	//���f���̕`��
	MV1DrawModel(m_modelHandle);
}

void CharacterBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_pState->OnCollide(collider);

#ifdef _DEBUG

	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		DrawString(0, 64, "1P���Ȃɂ��ƂԂ�����", GetColor(255, 255, 255));
	}
	else
	{
		DrawString(0, 80, "2P���Ȃɂ��ƂԂ�����", GetColor(255, 255, 255));
	}

#endif // _DEBUG
}

void CharacterBase::Final()
{
	Collidable::Final();
	MV1DeleteModel(m_modelHandle);
	for (auto& item : m_afterImageList)
	{
		MV1DeleteModel(item.handle);
	}
}

void CharacterBase::RetryInit()
{
	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	m_knockOutVelo = MyEngine::Vector3();

	auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

	//�������W�̌���
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		m_rigidbody.SetPos(kOnePlayerInitPos);
		m_lookPos.SetCenterPos(kOnePlayerInitPos);
		SetFrontPos(kTwoPlayerInitPos);
	}
	else if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_rigidbody.SetPos(kTwoPlayerInitPos);
		m_lookPos.SetCenterPos(kTwoPlayerInitPos);
		SetFrontPos(kOnePlayerInitPos);
	}

	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));

	//�����X�e�[�g�̐ݒ�
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();
}

void CharacterBase::UpdateAfterImage()
{
	//�c����������
	int deleteNum = 0;

	for (auto& item : m_afterImageList)
	{
		item.nowOpacityRate -= item.DeleteSpeed;

		float rate = fmin(item.maxOpacityRate, item.nowOpacityRate);

		MV1SetOpacityRate(item.handle, rate);

		if (item.nowOpacityRate < 0.0f)
		{
			MV1DeleteModel(item.handle);

			deleteNum++;
		}
	}

	for (int i = 0; i < deleteNum; i++)
	{
		m_afterImageList.pop_front();
	}
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

bool CharacterBase::SubMp(float subMp)
{
	if (m_nowMp >= subMp)
	{
		m_nowMp -= subMp;
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void CharacterBase::ChargeMp()
{
	m_nowMp += m_status.chargeSpd;

	//�ő�𒴂��Ȃ��悤�ɃN�����v
	m_nowMp = std::fmin(m_nowMp, GameSceneConstant::kMaxMp);
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
		MV1DetachAnim(m_modelHandle, m_lastAnim);
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
		pushData.targetHitReaction = item[static_cast<int>(NormalAttackDataSort::kTargetHitReaction)];
		pushData.attackHitKind = kAttackHitKindMap.at(item[static_cast<int>(NormalAttackDataSort::kAttackHitKind)]);

		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

std::shared_ptr<Attack> CharacterBase::CreateAttack(AttackData attackData)
{
	//�U���N���X
	std::shared_ptr<Attack> ans;

	//�U���̃^�O
	ObjectTag tag;

	//�v���C���[�ƃG�l�~�[�ǂ���̍U�������f
	if (attackData.isPlayer)
	{
		tag = ObjectTag::kOnePlayerAttack;
	}
	else
	{
		tag = ObjectTag::kTwoPlayerAttack;
	}
	//�U�����W�̐ݒ�
	MyEngine::Vector3 attackPos;

	//�L�����N�^�[�̐��ʕ����ɍU�����o��
	MyEngine::LocalPos localPos;

	//���g�𒆐S�Ƃ��郍�[�J�����W���쐬
	localPos.SetCenterPos(m_rigidbody.GetPos());

	//���ʕ�����ݒ�
	MyEngine::Vector3 toTarget;
	MyEngine::Vector3 targetPos;

	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		auto player1 = m_pGameManager->GetOnePlayerPointer();

		targetPos = m_pGameManager->GetTargetPos(player1);
		toTarget = targetPos - m_rigidbody.GetPos();
	}
	else
	{
		auto player2 = m_pGameManager->GetTwoPlayerPointer();

		targetPos = m_pGameManager->GetTargetPos(player2);
		toTarget = targetPos - m_rigidbody.GetPos();
	}

	localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget.Normalize());

	//�ǂ̂��炢���炷����ݒ�
	MyEngine::Vector3 localAttackPos;

	localAttackPos.z = std::fmin(std::fabs(toTarget.z), kAttackPopPos);

	localPos.SetLocalPos(localAttackPos);

	//���[�J�����W���烏�[���h���W�ɕϊ�
	attackPos = localPos.GetWorldPos();

	//y���W�̂�������l�܂ł͕␳
	float shiftY = toTarget.y;

	shiftY = std::fmin(shiftY, kAttackMaxShiftPosY);
	shiftY = std::fmax(shiftY, -kAttackMaxShiftPosY);

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

	ans->Init(status, m_pGameManager->GetEffectManagerPointer());

	m_pGameManager->AddAttack(ans);

	return ans;
}

CharacterBase::NormalAttackData CharacterBase::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}

CharacterBase::SpecialAttackData CharacterBase::GetSpecialAttackData(int specialNumber)
{

	//��ڂ̕K�E�Z���擾����
	if (specialNumber == 1)
	{
		return m_status.firstSpecialAttackData;
	}
	//��ڂ̕K�E�Z���擾����
	else if (specialNumber == 2)
	{
		return m_status.secondSpecialAttackData;
	}

	//�����܂ŗ�����~�߂�
	assert(false);

	return SpecialAttackData();
}

void CharacterBase::SetFrontPos(MyEngine::Vector3 frontPos)
{
	//frontPos�ւ̕���
	MyEngine::Vector3 dir = (frontPos - m_rigidbody.GetPos()).Normalize();

	MyEngine::Vector3 pos = (dir * kFrontPosDistance) + m_rigidbody.GetPos();

	//���[�J�����W�̑O�������C��
	m_targetLocalPos.SetFrontPos(pos);

	//�O������ۑ�����
	m_lookPos.SetLocalPos(m_lookPos.ChangeWorldToLocal(pos));

	//���f���̑O�������C������
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - pos).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
}

MyEngine::Vector3 CharacterBase::GetFrontPos()
{
	return m_lookPos.GetWorldPos();
}

bool CharacterBase::IsFrontTarget()
{

	MyEngine::Vector3 toTargetDir;

	//�v���C���[�Ȃ�
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		auto player1 = m_pGameManager->GetOnePlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetTargetPos(player1));
	}
	//2P���Ȃ�(��{CPU)
	else
	{
		auto player2 = m_pGameManager->GetTwoPlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetTargetPos(player2));
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

	MV1SetPosition(m_modelHandle, pos.CastVECTOR());
}

CharacterBase::AnimKind CharacterBase::GetAttackAnimKind(std::string animName)
{
	return kAttackAnimKindMap.at(animName);
}

void CharacterBase::LookTarget()
{
	auto pointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetTargetPos(pointer)).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	m_lookPos.SetLocalPos(m_lookPos.ChangeWorldToLocal(m_pGameManager->GetTargetPos(pointer)));
}

void CharacterBase::CreateAfterImage()
{
	AfterImage data;

	data.DeleteSpeed = GameSceneConstant::kAfterImageDeleteSpeed;

	data.nowOpacityRate = GameSceneConstant::kAfterImageInitOpacityRate;

	data.maxOpacityRate = GameSceneConstant::kAfterImageMaxOpacityRate;

	CreateAfterImage(data);
}

void CharacterBase::CreateAfterImage(AfterImage afterImageInfo)
{
	CharacterBase::AfterImage ans;

	//���f�����R�s�[���X�P�[������W��ݒ肷��
	int handle = MV1DuplicateModel(m_modelHandle);
	MyEngine::Vector3 drawPos = MV1GetPosition(m_modelHandle);
	MV1SetScale(handle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));
	MV1SetPosition(handle, drawPos.CastVECTOR());
	MV1SetRotationZYAxis(handle, (m_rigidbody.GetPos() - m_lookPos.GetWorldPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);

	//�A�j���[�V�����u�����h���I����Ă��Ȃ����
	if (!m_isEndAnimationBlend)
	{
		MV1AttachAnim(handle, MV1GetAttachAnim(m_modelHandle, m_lastAnim));
		MV1SetAttachAnimTime(handle, m_lastAnim, m_lastPlayAnimTime);
		MV1SetAttachAnimBlendRate(handle, m_lastAnim, 1.0f - m_animBlendRate);
	}

	//�A�j���[�V������ݒ肷��
	int anim = MV1AttachAnim(handle, MV1GetAttachAnim(m_modelHandle, m_attachAnim));
	MV1SetAttachAnimTime(handle, anim, m_nowPlayAnimTime);
	MV1SetAttachAnimBlendRate(handle, anim, m_animBlendRate);

	//�c���̏���ݒ�
	ans.handle = handle;

	ans.nowOpacityRate = afterImageInfo.nowOpacityRate;

	ans.maxOpacityRate = afterImageInfo.maxOpacityRate;

	ans.DeleteSpeed = afterImageInfo.DeleteSpeed;

	//�c�����X�g�ɓo�^
	m_afterImageList.push_back(ans);
}

void CharacterBase::StartFastCameraMove()
{
	m_pGameManager->StartFastCameraMove();
}

MyEngine::Vector3 CharacterBase::GetBackPos(float distance)
{
	MyEngine::Vector3 toBackPos = m_rigidbody.GetPos() - m_lookPos.GetWorldPos();

	toBackPos = toBackPos.Normalize();

	MyEngine::Vector3 ans = m_rigidbody.GetPos() + (toBackPos * (distance + GameSceneConstant::kCharacterRadius));

	MyEngine::LocalPos local;

	local.SetCenterPos(m_rigidbody.GetPos());
	MyEngine::Vector3 pos = m_rigidbody.GetPos();

	return ans;
}

void CharacterBase::SetIsTrigger(bool flag)
{
	m_pColData->SetIsTrigger(flag);
}

void CharacterBase::ChangeSituationUpdate(int situation)
{
	auto sit = static_cast<GameManager::Situation>(situation);

	//���g���C�������ꂽ�Ƃ�
	if (sit == GameManager::Situation::kRetry)
	{
		//�A�j���[�V�������~�߂�
		m_updateSituationFunc = &CharacterBase::UpdateNone;
	}
	//1P�̊J�n���o�����Ă���Ƃ�
	else if (sit == GameManager::Situation::kStart1P)
	{

		if (m_playerNumber == PlayerNumber::kOnePlayer)
		{
			//1P���̓X�^�[�g�������s��
			m_updateSituationFunc = &CharacterBase::UpdateStart;
		}
		else
		{
			//2P���͉������Ȃ��悤�ɐݒ肷��
			m_updateSituationFunc = &CharacterBase::UpdateNone;
		}
	}
	//2P�̊J�n���o�����Ă���Ƃ�
	else if (sit == GameManager::Situation::kStart2P)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer)
		{
			//2P���̓X�^�[�g�������s��
			m_updateSituationFunc = &CharacterBase::UpdateStart;
		}
		else
		{
			//1P���͉������Ȃ��悤�ɐݒ肷��
			m_updateSituationFunc = &CharacterBase::UpdateNone;
		}
	}
	//�o�g����
	else if (sit == GameManager::Situation::kBattle)
	{
		m_updateSituationFunc = &CharacterBase::UpdateBattle;
	}
	//�m�b�N�A�E�g�̉��o��
	else if (sit == GameManager::Situation::kKnockOut)
	{
		m_updateSituationFunc = &CharacterBase::UpdateKnockOut;
	}
	//���U���g��
	else if (sit == GameManager::Situation::kResult)
	{
		m_updateSituationFunc = &CharacterBase::UpdateResult;
	}
}

void CharacterBase::UpdateStart()
{
	//�A�j���[�V������ύX���Ă��Ȃ���ΕύX����
	if (m_playAnimKind != AnimKind::kStartPose)
	{
		ChangeAnim(AnimKind::kStartPose, false);
	}
	PlayAnim();
}

void CharacterBase::UpdateBattle()
{
	//State�̍X�V
	m_pState->Update();

	//�c���̍X�V���s��
	UpdateAfterImage();

	//�A�j���[�V�����̍X�V
	PlayAnim();

}

void CharacterBase::UpdateKnockOut()
{
	//�ړ��x�N�g�����ݒ肳��Ă��Ȃ����
	if (m_knockOutVelo.SqLength() < 0.01f)
	{
		//�ړ��x�N�g����ݒ肷��
		m_knockOutVelo = m_rigidbody.GetVelo();
	}

	//�ړ���x������
	m_rigidbody.SetVelo(m_knockOutVelo * 0.1f);
	//�A�j���[�V�������������Đ�����
	SetAnimPlaySpeed(0.1f);

	//�c���̍X�V���s��
	UpdateAfterImage();

	//�A�j���[�V�����̍X�V
	PlayAnim();
}

void CharacterBase::UpdateResult()
{
	//�ړ������Ȃ��悤�ɂ���
	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));

	//�A�j���[�V�����̍Đ����x�����Z�b�g����
	SetAnimPlaySpeed();

	//�c���̍X�V���s��
	UpdateAfterImage();

	//�A�j���[�V������ύX���Ă��Ȃ���ΕύX����
	if (m_nowHp > 0)
	{
		if (m_playAnimKind != AnimKind::kWinPose)
		{
			ChangeAnim(AnimKind::kWinPose, true);
			//�A�j���[�V������ύX����^�C�~���O�Ő��ʕ������ύX����
			auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pGameManager->GetTargetPos(thisPointer);

			frontPos.y = m_rigidbody.GetPos().y;

			SetFrontPos(frontPos);
		}
	}
	else
	{
		if (m_playAnimKind != AnimKind::kLosePose)
		{
			ChangeAnim(AnimKind::kLosePose, true);
			//�A�j���[�V������ύX����^�C�~���O�Ő��ʕ������ύX����
			auto thisPointer = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pGameManager->GetTargetPos(thisPointer);

			frontPos.y = m_rigidbody.GetPos().y;

			SetFrontPos(frontPos);
		}
	}

	PlayAnim();
}

void CharacterBase::UpdateNone()
{
	//�����X�V���Ȃ�
}
