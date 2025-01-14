#include "TitleUi.h"
#include "DxLib.h"
#include <string>
#include "GraphManager.h"
#include "Game.h"

namespace
{
	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�����̑傫��
	constexpr int kFontSize = 64;

	//�J������NearFar
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;

	//PRESSANYBUTTON��\��������W
	constexpr int kStringPosX = 70;
	constexpr int kStringPosY = 700;

	//�\�����镶����
	const std::string kUiString = "PRESS \n    ANY BUTTON";

	//���������Ă��鎞��
	constexpr int kStringExistTime = 90;

	//���������Ă��Ȃ�����
	constexpr int kStringNotExistTime = 15;

	//�}�E�X��\������ŏ��̍��W
	const MyEngine::Vector2 kMouseInitPos(900, 0);

	//�u���[�w�b�h��\������ŏ��̍��W
	const MyEngine::Vector2 kBigBlueInitPos(Game::kWindowWidth, 150);

	//�ő剽�t���[���L�����N�^�[�������o����
	constexpr int kMaxCharacterMoveFrame = 100;

	//�L�����N�^�[�̈ړ����x
	constexpr int kCharacterMoveSpeed = 10;

	//�L�����N�^�[�̉摜�̕�
	constexpr int kCharacterSize = 120;
}

TitleUi::TitleUi() :
	m_skyDomeHandle(-1),
	m_time(0),
	m_mouseMoveTime(-1),
	m_blueHeadMoveTime(-1),
	m_isExistString(true)
{
	m_mouseSide = Side::kUp;
	m_blueHeadSide = Side::kRight;

	m_fontHandle = CreateFontToHandle(kFontName, kFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
}

TitleUi::~TitleUi()
{
	DeleteFontToHandle(m_fontHandle);
}

void TitleUi::Init()
{
	//�X�J�C�h�[���̃��f�����[�h
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");

	//�J�����̐ݒ�
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));

	//NearFar�̐ݒ�
	SetCameraNearFar(kCameraNear, kCameraFar);

	//�X�J�C�h�[���̐ݒ�
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

	//�L�����N�^�[�̍��W�ݒ�
	m_mousePos = kMouseInitPos;
	m_blueHeadPos = kBigBlueInitPos;


	//�L�����N�^�[���ړ�����܂ł̎��Ԑݒ�
	m_mouseMoveTime = GetRand(kMaxCharacterMoveFrame);
	m_blueHeadMoveTime = GetRand(kMaxCharacterMoveFrame);
}

void TitleUi::Update()
{
	//������̎��Ԍv���֌W
	m_time++;

	//�������\�����Ă����ꍇ
	if (m_isExistString)
	{
		//��莞�Ԍo�߂��Ă����當���������
		if (m_time > kStringExistTime)
		{
			//���Ԃ����Z�b�g
			m_time = 0;

			m_isExistString = false;
		}
	}
	//�������\�����Ă��Ȃ������ꍇ
	else
	{
		//��莞�Ԍo�߂��Ă����當�����\������
		if (m_time > kStringNotExistTime)
		{
			//���Ԃ����Z�b�g
			m_time = 0;

			m_isExistString = true;
		}
	}


	//�L�����N�^�[�̎��Ԍv���֌W
	m_mouseMoveTime--;
	m_blueHeadMoveTime--;

	//�ړ����鎞�ԂɂȂ�����
	if (m_mouseMoveTime < 0)
	{
		MyEngine::Vector2 moveVec = GetReturnMoveVec(m_mouseSide);

		m_mousePos += moveVec;

		//��ʊO�ɏo�����ǂ������`�F�b�N

		//�o�Ă�������W�������_���ŉ�ʊO�ɐݒ肷��
		if (!CheckPos(m_mousePos))
		{

			m_mouseSide = static_cast<Side>(GetRand(static_cast<int>(Side::kSideNum)));

			m_mousePos = SetRandomPos(m_mouseSide);

			m_mouseMoveTime = GetRand(kMaxCharacterMoveFrame);
		}
	}
	//����ȊO
	else
	{
		MyEngine::Vector2 moveVec = GetEnterMoveVec(m_mouseSide);

		m_mousePos += moveVec;
	}

	//�ړ����鎞�ԂɂȂ�����
	if (m_blueHeadMoveTime < 0)
	{
		MyEngine::Vector2 moveVec = GetReturnMoveVec(m_blueHeadSide);

		m_blueHeadPos += moveVec;

		//��ʊO�ɏo�����ǂ������`�F�b�N

		//�o�Ă�������W�������_���ŉ�ʊO�ɐݒ肷��
		if (!CheckPos(m_blueHeadPos))
		{
			m_blueHeadSide = static_cast<Side>(GetRand(static_cast<int>(Side::kSideNum)));

			m_blueHeadPos = SetRandomPos(m_blueHeadSide);

			m_blueHeadMoveTime = GetRand(kMaxCharacterMoveFrame);
		}
	}
	//����ȊO
	else
	{
		MyEngine::Vector2 moveVec = GetEnterMoveVec(m_blueHeadSide);

		m_blueHeadPos += moveVec;
	}

	m_mousePos = ClampPos(m_mouseSide, m_mousePos);

	m_blueHeadPos = ClampPos(m_blueHeadSide, m_blueHeadPos);


}

void TitleUi::Draw()
{
	auto& manager = GraphManager::GetInstance();

	//�X�J�C�h�[���̕`��
	MV1DrawModel(m_skyDomeHandle);

	if (m_isExistString)
	{
		DrawStringToHandle(kStringPosX, kStringPosY, kUiString.c_str(), GetColor(255, 255, 255), m_fontHandle, GetColor(0, 0, 0));
	}

	DrawGraph(0, 0, manager.GetHandle("TitleLogo"), true);

	DrawCharacter(m_mouseSide, m_mousePos, manager.GetHandle("Mouse"));
	DrawCharacter(m_blueHeadSide, m_blueHeadPos, manager.GetHandle("BlueHead"));

}

void TitleUi::DrawCharacter(Side side, MyEngine::Vector2 pos, int handle)
{
	double rota = 0.0;

	if (side == Side::kUp)
	{
		//180�x��]
		rota = DX_PI;
	}
	else if (side == Side::kRight)
	{
		//270�x��]
		rota = (DX_PI / 180) * 270;
	}
	else if (side == Side::kLeft)
	{
		//90�x��]
		rota = (DX_PI / 2);
	}
	else if (side == Side::kDown)
	{
		rota = 0.0;
	}

	DrawRotaGraph(pos.x, pos.y, 1.0, rota, handle, true);
}

MyEngine::Vector2 TitleUi::GetEnterMoveVec(Side side)
{
	if (side == Side::kUp)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}
	else if (side == Side::kRight)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kLeft)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kDown)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::GetReturnMoveVec(Side side)
{
	if (side == Side::kUp)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}
	else if (side == Side::kRight)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kLeft)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kDown)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::ClampPos(Side side, MyEngine::Vector2 pos)
{
	MyEngine::Vector2 ans = pos;

	if (side == Side::kUp)
	{
		ans.y = min(pos.y, kCharacterSize);
	}
	else if (side == Side::kRight)
	{
		ans.x = max(pos.x, Game::kWindowWidth - kCharacterSize);
	}
	else if (side == Side::kLeft)
	{
		ans.x = min(pos.x, kCharacterSize);
	}
	else if (side == Side::kDown)
	{
		ans.y = max(pos.y, Game::kWindowHeight - kCharacterSize);
	}

	return ans;
}

MyEngine::Vector2 TitleUi::SetRandomPos(Side side)
{
	if (side == Side::kUp)
	{
		int randomX = GetRand(Game::kWindowWidth - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(randomX, -kCharacterSize);
	}
	else if (side == Side::kRight)
	{
		int randomY = GetRand(Game::kWindowHeight - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(Game::kWindowWidth + kCharacterSize, randomY);
	}
	else if (side == Side::kLeft)
	{
		int randomY = GetRand(Game::kWindowHeight - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(-kCharacterSize, randomY);
	}
	else if (side == Side::kDown)
	{
		int randomX = GetRand(Game::kWindowWidth - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(randomX, Game::kWindowHeight + kCharacterSize);
	}


	return MyEngine::Vector2(0, 0);
}

bool TitleUi::CheckPos(MyEngine::Vector2 pos)
{

	if (pos.x >= -kCharacterSize &&
		pos.x <= Game::kWindowWidth + kCharacterSize &&
		pos.y >= -kCharacterSize &&
		pos.y <= Game::kWindowHeight + kCharacterSize)
	{
		return true;
	}
	return false;
}
