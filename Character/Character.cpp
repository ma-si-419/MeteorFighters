#include "Character.h"
#include "SphereColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include "Input.h"
#include "EnemyInput.h"
#include <cassert>
#include <cmath>
#include "BattleManager.h"
#include "SoundManager.h"
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
	const MyEngine::Vector3 kOnePlayerInitPos(-50, 150, -50);
	//2P���̃L�����N�^�[�̏������W
	const MyEngine::Vector3 kTwoPlayerInitPos(50, 150, 50);

	//�G�����ʂɂ��Ȃ��Ƃ��̃^�[�Q�b�g���W�܂ł̋���
	constexpr float kTargetDistance = 30.0f;

	//�{�C�X�̑傫��(255��MAX)
	constexpr int kVoiceVolume = 40;

	//�U���̎�ނ��O���t�@�C���̕����񂩂������AttackHitKind�ɕϊ�����ۂɎg�p����
	const std::map<std::string, Character::AttackHitKind> kAttackHitKindMap =
	{
		{"��",Character::AttackHitKind::kLow},
		{"��",Character::AttackHitKind::kMiddle},
		{"���������",Character::AttackHitKind::kFarBurst},
		{"�㐁�����",Character::AttackHitKind::kUpBurst},
		{"���������",Character::AttackHitKind::kDownBurst},
		{"���X�^��",Character::AttackHitKind::kMiddleStan},
		{"���X�^��",Character::AttackHitKind::kBottomStan}
	};

	//�K�E�Z�̎�ނ��O���t�@�C���̕����񂩂������AttackKind�ɕϊ�����ۂɎg�p����
	const std::map<std::string, Character::AttackKind> kSpecialAttackKindMap =
	{
		{"�r�[��",Character::AttackKind::kLaser},
		{"�i��",Character::AttackKind::kRush},
		{"�C�e",Character::AttackKind::kEnergy},
		{"����",Character::AttackKind::kThrow},
		{"�̓�����",Character::AttackKind::kAssault}
	};

	//�L�����N�^�[�̐���VoiceKind���當����ɕϊ�����ۂɎg�p����
	const std::map<Character::VoiceKind, std::string> kVoiceKindMap =
	{
		{Character::VoiceKind::kStart,"Start"},
		{Character::VoiceKind::kLowAttack,"LowAttack"},
		{Character::VoiceKind::kMiddleAttack,"MiddleAttack"},
		{Character::VoiceKind::kHighAttack,"HighAttack"},
		{Character::VoiceKind::kSpecialAttack,"SpecialAttack"},
		{Character::VoiceKind::kDodge1,"Dodge1"},
		{Character::VoiceKind::kDodge2,"Dodge2"},
		{Character::VoiceKind::kWinBashing,"WinBashing"}
	};

	//�U���̃A�j���[�V�������O���t�@�C���̕����񂩂������AnimKind�ɕϊ�����ۂɎg�p����
	const std::map<std::string, Character::AnimKind> kAttackAnimKindMap =
	{
		{"LowAttack1",Character::AnimKind::kLowAttack1},
		{"LowAttack2",Character::AnimKind::kLowAttack2},
		{"LowAttack3",Character::AnimKind::kLowAttack3},
		{"LowAttack4",Character::AnimKind::kLowAttack4},
		{"LowAttack5",Character::AnimKind::kLowAttack5},
		{"LowAttack6",Character::AnimKind::kLowAttack6},
		{"LowAttack7",Character::AnimKind::kLowAttack7},
		{"LowAttack8",Character::AnimKind::kLowAttack8},
		{"EnergyAttackRight",Character::AnimKind::kEnergyAttackRight},
		{"EnergyAttackLeft",Character::AnimKind::kEnergyAttackLeft},
		{"UpperAttack",Character::AnimKind::kUpperAttack},
		{"StanAttack",Character::AnimKind::kStanAttack},
		{"LegSweepAttack",Character::AnimKind::kLegSweepAttack},
		{"UpChargeAttack",Character::AnimKind::kUpChargeAttack},
		{"MiddleChargeAttack",Character::AnimKind::kMiddleChargeAttack},
		{"DownChargeAttack",Character::AnimKind::kDownChargeAttack},
		{"EnergyChargeAttack",Character::AnimKind::kEnergyChargeAttack},
		{"DashAttack",Character::AnimKind::kDashAttack}
	};
}

Character::Character(ObjectTag tag, CharacterKind kind) :
	Collidable(tag, ColliderData::Kind::kSphere),
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
	m_isEndAnim(false),
	m_modelPath("empty"),
	m_nowHitReaction(Character::HitReactionKind::kNone),
	m_isDrawCharacter(true)
{
	auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);

	sphereData->m_radius = GameSceneConstant::kCharacterRadius;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	SetNormalAttackData(data);

	m_playAnimKind = AnimKind::kIdle;
}

Character::~Character()
{
}

void Character::Init()
{

	//���͏��̐ݒ�
	if (m_playerNumber == Character::PlayerNumber::kOnePlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(0);
	}
	else if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
	{
		m_input = MyEngine::Input::GetInstance().GetInputData(1);
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
	else if (m_characterKind == CharacterKind::kTheBlue)
	{
		path += "BigB";
	}

	path = "data/model/" + path + ".mv1";

	m_modelPath = path;

	Collidable::Init();

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

	//�����X�e�[�g�̐ݒ�
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();

	//�������W�̌���
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		m_rigidbody.SetPos(kOnePlayerInitPos);
		m_targetLocalPos.SetCenterPos(kOnePlayerInitPos);
		m_lookPos.SetCenterPos(kOnePlayerInitPos);
		SetFrontPos(kTwoPlayerInitPos);
	}
	else if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_rigidbody.SetPos(kTwoPlayerInitPos);
		m_targetLocalPos.SetCenterPos(kTwoPlayerInitPos);
		m_lookPos.SetCenterPos(kTwoPlayerInitPos);
		SetFrontPos(kOnePlayerInitPos);
	}

	//�G���̏���
	if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
	{
		//�G�l�~�[�̃X�e�[�g��ݒ肷��
		m_pEnemyInput->SetEnemyState(m_pState);
		//�v���C���[�̃X�e�[�g��ݒ肷��
		m_pEnemyInput->SetPlayerState(m_pBattleManager->GetOnePlayerPointer()->m_pState);
	}
}

void Character::SetEnemyInput(int level)
{
	m_pEnemyInput = std::make_shared<EnemyInput>(MyEngine::Input::GetInstance().GetInputData(1));

	m_pEnemyInput->SetGameManager(m_pBattleManager);

	m_pEnemyInput->SetAiLevel(static_cast<EnemyInput::AiLevel>(level));
}

void Character::Update()
{
	m_targetLocalPos.SetCenterPos(GetPos());

	//�G�l�~�[�̓��͏����X�V����
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->Update();

		//�`���[�g���A���̎���MP������Ȃ��悤�ɂ���
		if (m_pBattleManager->GetGameKind() == GameManagerBase::GameKind::kTutorial)
		{
			m_nowMp = GameSceneConstant::kMaxMp;
		}
	}

	//��ɍX�V�������
	m_pState->UpdateCommon();

	//EnemyInput�Ƀv���C���[�̃X�e�[�g��ݒ肷��
	if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
	{
		m_pEnemyInput->SetPlayerState(m_pBattleManager->GetOnePlayerPointer()->m_pState);
	}

	//State���ύX����Ă�����State�����ւ���
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;

		//�G���̏���
		if (m_playerNumber == Character::PlayerNumber::kTwoPlayer)
		{
			m_pEnemyInput->SetEnemyState(m_pState);
		}
	}

	//�󋵂ɂ���ĕω�����A�b�v�f�[�g
	(this->*m_updateSituationFunc)();

	//���[�J�����W�̒��S���W�̍X�V
	m_lookPos.SetCenterPos(m_rigidbody.GetPos());

	//�`����W�̐ݒ�
	SetDrawPos(m_rigidbody.GetPos());
}

void Character::Draw()
{
	//���U���g����2P��������
	if (m_pBattleManager->GetNowSituation() == GameManagerBase::BattleSituation::kResult ||
		m_pBattleManager->GetNowSituation() == GameManagerBase::BattleSituation::kRetry)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer) return;
	}

	//�c���̕`��
	for (auto item : m_afterImageList)
	{
		MV1DrawModel(item.handle);
	}

	MyEngine::Vector3 pos = (GetFrontPos() - m_rigidbody.GetPos()).Normalize() * 10.0f + m_rigidbody.GetPos();

	//�`�悷��Ƃ���Ă�����
	if (m_isDrawCharacter)
	{
		//���f���̕`��
		MV1DrawModel(m_modelHandle);
	}
}

void Character::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_pState->OnCollide(collider);
}

void Character::Final()
{
	Collidable::Final();
	MV1DeleteModel(m_modelHandle);
	for (auto& item : m_afterImageList)
	{
		MV1DeleteModel(item.handle);
	}
}

void Character::RetryInit()
{
	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	m_knockOutVelo = MyEngine::Vector3();

	auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

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

	if (m_pState)
	{
		//State�̏I������
		m_pState->Exit();
	}
	//�����X�e�[�g�̐ݒ�
	m_pState = std::make_shared<CharacterStateIdle>(thisPointer);
	m_pState->Enter();
}

void Character::UpdateAfterImage()
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

void Character::SetGameManager(std::shared_ptr<GameManagerBase> manager)
{
	m_pBattleManager = manager;
}

MyEngine::Vector3 Character::GetPos()
{
	return m_rigidbody.GetPos();
}

MyEngine::Vector3 Character::GetVelo()
{
	return m_rigidbody.GetVelo();
}

bool Character::SubMp(float subMp)
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

void Character::ChargeMp()
{
	m_nowMp += m_status.chargeSpd;

	//�ő�𒴂��Ȃ��悤�ɃN�����v
	m_nowMp = std::fmin(m_nowMp, GameSceneConstant::kMaxMp);
}

void Character::ChangeAnim(AnimKind animKind, bool loop)
{
	ChangeAnim(animKind, loop, kAnimBlendSpeed);
}

void Character::ChangeAnim(AnimKind animKind, bool loop, float blendSpeed)
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

void Character::PlayAnim()
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

void Character::SetAnimPlaySpeed(float speed)
{
	m_animPlaySpeed = speed;
}

void Character::SetAnimPlaySpeed()
{
	m_animPlaySpeed = kAnimPlaySpeed;
}

bool Character::IsAnimEnd()
{
	return false;
}

Character::AttackKind Character::GetSpecialAttackKind(std::string kind)
{
	return kSpecialAttackKindMap.at(kind);
}

void Character::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto& item : normalAttackData)
	{
		NormalAttackData pushData;

		pushData.attackName = item[static_cast<int>(NormalAttackDataSort::kAttackName)];
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
		pushData.animationSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kAnimationSpeed)]);
		pushData.effectName = item[static_cast<int>(NormalAttackDataSort::kEffectName)];
		pushData.soundName = item[static_cast<int>(NormalAttackDataSort::kSoundName)];

		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

std::shared_ptr<Attack> Character::CreateAttack(AttackData attackData)
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
		auto player1 = m_pBattleManager->GetOnePlayerPointer();

		targetPos = m_pBattleManager->GetTargetPos(player1);
		toTarget = targetPos - m_rigidbody.GetPos();
	}
	else
	{
		auto player2 = m_pBattleManager->GetTwoPlayerPointer();

		targetPos = m_pBattleManager->GetTargetPos(player2);
		toTarget = targetPos - m_rigidbody.GetPos();
	}

	//�����G�����ʂɂ�����
	if (IsFrontTarget())
	{
		localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget.Normalize());
	}
	//�G�����ʂɂ��Ȃ��ꍇ
	else
	{
		localPos.SetFrontPos(GetFrontPos());

		//�^�[�Q�b�g���W�����g�̐��ʕ����ɂ���
		targetPos = (GetFrontPos() - m_rigidbody.GetPos()).Normalize() * kTargetDistance;
	}
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
	status.effectName = attackData.effectName;
	status.attackName = attackData.attackName;
	status.hitSoundName = attackData.hitSoundName;

	ans->Init(status, m_pBattleManager->GetEffectManagerPointer());

	m_pBattleManager->AddAttack(ans);

	return ans;
}

Character::NormalAttackData Character::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}

Character::SpecialAttackData Character::GetSpecialAttackData(int specialNumber)
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

void Character::SetFrontPos(MyEngine::Vector3 frontPos)
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

MyEngine::Vector3 Character::GetFrontPos()
{
	return m_lookPos.GetWorldPos();
}

bool Character::IsFrontTarget()
{

	MyEngine::Vector3 toTargetDir;

	//�v���C���[�Ȃ�
	if (m_playerNumber == PlayerNumber::kOnePlayer)
	{
		auto player1 = m_pBattleManager->GetOnePlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pBattleManager->GetTargetPos(player1));
	}
	//2P���Ȃ�(��{CPU)
	else
	{
		auto player2 = m_pBattleManager->GetTwoPlayerPointer();
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pBattleManager->GetTargetPos(player2));
	}

	toTargetDir = toTargetDir.Normalize();

	if (toTargetDir.z > 0.0f)
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

void Character::SetDrawPos(MyEngine::Vector3 pos)
{
	pos.y -= kCharacterHeight;

	pos += m_drawShiftVec;

	MV1SetPosition(m_modelHandle, pos.CastVECTOR());
}

Character::AnimKind Character::GetAttackAnimKind(std::string animName)
{
	return kAttackAnimKindMap.at(animName);
}

int Character::GetStateKind()
{
	return static_cast<int>(m_pState->GetKind());
}

void Character::LookTarget()
{
	auto pointer = std::dynamic_pointer_cast<Character>(shared_from_this());


	//�^�[�Q�b�g�̕���������
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pBattleManager->GetTargetPos(pointer)).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	m_lookPos.SetLocalPos(m_lookPos.ChangeWorldToLocal(m_pBattleManager->GetTargetPos(pointer)));

	//���ʕ�����ݒ�
	SetFrontPos(m_pBattleManager->GetTargetPos(pointer));
}

void Character::CreateAfterImage()
{
	AfterImage data;

	data.DeleteSpeed = GameSceneConstant::kAfterImageDeleteSpeed;

	data.nowOpacityRate = GameSceneConstant::kAfterImageInitOpacityRate;

	data.maxOpacityRate = GameSceneConstant::kAfterImageMaxOpacityRate;

	CreateAfterImage(data);
}

void Character::CreateAfterImage(AfterImage afterImageInfo)
{
	Character::AfterImage ans;

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

void Character::StartFastCameraMove()
{
	m_pBattleManager->StartFastCameraMove();
}

MyEngine::Vector3 Character::GetBackPos(float distance)
{
	MyEngine::Vector3 toBackPos = m_rigidbody.GetPos() - m_lookPos.GetWorldPos();

	toBackPos = toBackPos.Normalize();

	MyEngine::Vector3 ans = m_rigidbody.GetPos() + (toBackPos * (distance + GameSceneConstant::kCharacterRadius));

	MyEngine::LocalPos local;

	local.SetCenterPos(m_rigidbody.GetPos());

	MyEngine::Vector3 pos = m_rigidbody.GetPos();

	return ans;
}

void Character::SetIsTrigger(bool flag)
{
	m_pColData->SetIsTrigger(flag);
}

void Character::ChangeSituationUpdate(int situation)
{
	auto sit = static_cast<GameManagerBase::BattleSituation>(situation);

	//���g���C�������ꂽ�Ƃ�
	if (sit == GameManagerBase::BattleSituation::kRetry)
	{
		//�A�j���[�V�������~�߂�
		m_updateSituationFunc = &Character::UpdateNone;
	}
	//1P�̊J�n���o�����Ă���Ƃ�
	else if (sit == GameManagerBase::BattleSituation::kStart1P)
	{
		InitStart();
		if (m_playerNumber == PlayerNumber::kOnePlayer)
		{
			//1P���̓X�^�[�g�������s��
			m_updateSituationFunc = &Character::UpdateStart;
		}
		else
		{
			//2P���͉������Ȃ��悤�ɐݒ肷��
			m_updateSituationFunc = &Character::UpdateNone;
		}
	}
	//2P�̊J�n���o�����Ă���Ƃ�
	else if (sit == GameManagerBase::BattleSituation::kStart2P)
	{
		if (m_playerNumber == PlayerNumber::kTwoPlayer)
		{
			//2P���̓X�^�[�g�������s��
			m_updateSituationFunc = &Character::UpdateStart;
		}
		else
		{
			//1P���͉������Ȃ��悤�ɐݒ肷��
			m_updateSituationFunc = &Character::UpdateNone;
		}
	}
	//�o�g����
	else if (sit == GameManagerBase::BattleSituation::kBattle)
	{
		m_updateSituationFunc = &Character::UpdateBattle;
	}
	//�m�b�N�A�E�g�̉��o��
	else if (sit == GameManagerBase::BattleSituation::kKnockOut)
	{
		m_updateSituationFunc = &Character::UpdateKnockOut;
	}
	//���U���g��
	else if (sit == GameManagerBase::BattleSituation::kResult)
	{
		m_updateSituationFunc = &Character::UpdateResult;
	}
	//���j���[��ꎞ��~���s���Ă���Ƃ�
	else if (sit == GameManagerBase::BattleSituation::kMenu)
	{
		m_updateSituationFunc = &Character::UpdateNone;
	}
	//������󂯕t���Ȃ��Ƃ�
	else if (sit == GameManagerBase::BattleSituation::kIdle)
	{
		m_updateSituationFunc = &Character::UpdateIdle;
	}
}

void Character::SetModelHandle(int handle)
{
	m_modelHandle = handle;

	MV1SetScale(m_modelHandle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));
}

MyEngine::Vector3 Character::GetNearStagePos()
{
	MyEngine::Vector3 pos = m_rigidbody.GetPos();
	pos.y = 0;
	return pos;
}

std::shared_ptr<EnemyInput> Character::GetEnemyInput()
{
	//2P���Ȃ�
	if (m_playerNumber == PlayerNumber::kTwoPlayer)
	{
		return m_pEnemyInput;
	}

	//1P�̏ꍇ�͉����Ԃ��Ȃ�
	return std::shared_ptr<EnemyInput>();
}

void Character::PlayVoice(VoiceKind kind)
{
	//�����{�C�X���Đ�����Ă�����
	if (m_voiceHandle != -1)
	{
		//�{�C�X���~�߂�
		StopSoundMem(m_voiceHandle);
	}

	//�{�C�X�̖��O
	std::string voiceName = kVoiceKindMap.at(kind);

	//�L�����N�^�[�ɂ���ă{�C�X�̃p�X��ύX����
	if (m_characterKind == CharacterKind::kMouse)
	{
		voiceName = "Mouse" + voiceName;
	}
	else if (m_characterKind == CharacterKind::kTheBlue)
	{
		voiceName = "Blue" + voiceName;
	}

	//�{�C�X�̍Đ�
	m_voiceHandle = SoundManager::GetInstance().PlayOnceSound(voiceName);

}

void Character::InitStart()
{
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
}

void Character::UpdateStart()
{
	//�A�j���[�V������ύX���Ă��Ȃ���ΕύX����
	if (m_playAnimKind != AnimKind::kStartPose)
	{
		ChangeAnim(AnimKind::kStartPose, false);
		//���̃^�C�~���O�ňꏏ�ɉ������Đ�����
		PlayVoice(VoiceKind::kStart);
	}
	PlayAnim();
}

void Character::UpdateBattle()
{
	//State�̍X�V
	m_pState->Update();

	//�c���̍X�V���s��
	UpdateAfterImage();

	//�A�j���[�V�����̍X�V
	PlayAnim();

}

void Character::UpdateKnockOut()
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

void Character::UpdateResult()
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
			auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pBattleManager->GetTargetPos(thisPointer);

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
			auto thisPointer = std::dynamic_pointer_cast<Character>(shared_from_this());

			MyEngine::Vector3 frontPos = m_pBattleManager->GetTargetPos(thisPointer);

			frontPos.y = m_rigidbody.GetPos().y;

			SetFrontPos(frontPos);
		}
	}

	PlayAnim();
}

void Character::UpdateIdle()
{
	//�ړ������Ȃ��悤�ɂ���
	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));

	//�A�j���[�V�����̍Đ����x�����Z�b�g����
	SetAnimPlaySpeed();

	//�c���̍X�V���s��
	UpdateAfterImage();

	//�A�j���[�V������ύX���Ă��Ȃ���ΕύX����
	if (m_playAnimKind != AnimKind::kSkyIdle)
	{
		ChangeAnim(AnimKind::kSkyIdle, true);
	}

	//�A�j���[�V�����̍X�V���s��
	PlayAnim();
}

void Character::UpdateNone()
{
	//�ړ������Ȃ��悤�ɂ���
	m_rigidbody.SetVelo(MyEngine::Vector3(0, 0, 0));
}
