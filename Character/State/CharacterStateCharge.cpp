#include "CharacterStateCharge.h"
#include "CharacterStateIdle.h"
#include "CharacterStateRush.h"
#include "CharacterStateSpecialAttack.h"
#include "Character.h"
#include "Input.h"
#include "DxLib.h"
#include "Effect.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"

namespace
{
	//�`���[�W�I���A�j���[�V�����̎���
	constexpr int kEndChargeTime = 10;

	//�`���[�W�n�߂̃A�j���[�V�����̍Đ����x
	constexpr float kStartAnimSpeed = 2.0f;

	//�`���[�W���߂̃J������h�炷����
	constexpr int kCameraShakeTime = 5;

	//�G�t�F�N�g�̃��[�v�X�^�[�g�t���[��
	constexpr int kEffectStartFrame = 30;

	//�G�t�F�N�g�̃��[�v�G���h�t���[��
	constexpr int kEffectEndFrame = 33;
}

CharacterStateCharge::CharacterStateCharge(std::shared_ptr<Character> character) :
	CharacterStateBase(character)
{
}

void CharacterStateCharge::Enter()
{
	//����State�̃|�C���^�����g�ɐݒ肵�Ă���
	m_pNextState = shared_from_this();
	//StateKind��ݒ�
	m_kind = CharacterStateKind::kCharge;
	//�A�j���[�V������ݒ�
	m_pCharacter->ChangeAnim(Character::AnimKind::kStartCharge, false);
	//�A�j���[�V�����̍Đ����x��ݒ�
	m_pCharacter->SetAnimPlaySpeed(kStartAnimSpeed);
	//�G�t�F�N�g�̃|�C���^�쐬
	m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kCharge);
	//�G�t�F�N�g�̍��W�ݒ�
	m_pEffect->SetPos(m_pCharacter->GetPos());
	//�G�t�F�N�g�̃��[�v�ݒ�
	m_pEffect->SetLoop(kEffectStartFrame, kEffectEndFrame);
	//�G�t�F�N�g���}�l�[�W���[�ɓo�^
	m_pManager->EntryEffect(m_pEffect);
}

void CharacterStateCharge::Update()
{

	auto input = GetCharacterInput();

	m_pCharacter->ChargeMp();

	//�`���[�W�J�n���Đ����Ă���
	if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kStartCharge)
	{
		//�A�j���[�V�������I����Ă�����
		if (m_pCharacter->IsEndAnim())
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kInCharge, true);
			m_pCharacter->SetAnimPlaySpeed();
			m_pManager->ShakeCamera(kCameraShakeTime);
		}
	}

	//���t�g�V�����_�[��������Ȃ��Ȃ�����A�C�h��State�ɖ߂�
	if (!input->IsPushTrigger(false))
	{
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		next->SetEndAnim(static_cast<int>(Character::AnimKind::kEndCharge), kEndChargeTime);

		ChangeState(next);

		return;
	}

	//A�{�^���������ꂽ��
	if (input->IsTrigger("A"))
	{
		MyEngine::Vector3 moveDir(input->GetStickInfo().leftStickX, 0, -input->GetStickInfo().leftStickY);

		//�ˌ�State�Ɉڍs����
		auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

		if (moveDir.SqLength() > 0.01f)
		{
			moveDir = moveDir.Normalize();
			next->SetMoveDir(moveDir);

			ChangeState(next);

			return;
		}

		moveDir = MyEngine::Vector3(0, 0, 1);

		next->SetMoveDir(moveDir);

		ChangeState(next);

		return;
	}

	//Y�{�^���������ꂽ��
	if (input->IsTrigger("Y"))
	{
		float cost = static_cast<float>(m_pCharacter->GetSpecialAttackData(1).cost);

		//�C�͂�����Ȃ���������s���Ȃ�
		if (m_pCharacter->SubMp(cost))
		{
			auto next = std::make_shared<CharacterStateSpecialAttack>(m_pCharacter);

			next->SetSpecialAttackNumber(1);

			ChangeState(next);
		}
	}

	//1P�����s���Ă�����J�����������h�炷
	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		m_pManager->SwayCamera();
	}

	//�G�t�F�N�g�̍��W��ݒ肷��
	m_pEffect->SetPos(m_pCharacter->GetPos());

	SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

	//�C�͂��}�b�N�X�ɂȂ��Ă�����
	if (m_pCharacter->GetMp() >= GameSceneConstant::kMaxMp)
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kEnergyCharge));
	}

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Charge", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateCharge::Exit()
{
	//�G�t�F�N�g���폜����
	m_pManager->ExitEffect(m_pEffect);
}