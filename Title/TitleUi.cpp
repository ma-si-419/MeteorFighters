#include "TitleUi.h"
#include "DxLib.h"
#include <string>
#include "GraphManager.h"
#include "SoundManager.h"
#include "Game.h"

namespace
{
	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�����̑傫��
	constexpr int kFontSize = 50;

	//�����̐F(�O���[)
	const unsigned int kFontColor = GetColor(192, 192, 192);
	 
	//�J������NearFar
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 400.0f;

	//�X�J�C�h�[���̊g�嗦
	constexpr float kSkyDomeScale = 3.0f;

	//�X�J�C�h�[���̉�]���x
	constexpr float kSkyDomeRotaSpeed = 0.001f;

	//PRESSANYBUTTON��\��������W
	constexpr int kStringPosX = 56;
	constexpr int kStringPosY = 710;

	//�\�����镶����
	//const std::string kUiString = "PRESS \n    ANY BUTTON";
	const std::string kUiString = "PRESS ANY BUTTON";

	//���������Ă��鎞��
	constexpr int kStringExistTime = 90;

	//���������Ă��Ȃ�����
	constexpr int kStringNotExistTime = 15;

	//�L�����N�^�[��`�悷����W
	const MyEngine::Vector2 kCharacterDrawPos[static_cast<int>(TitleUi::DrawCharacterKind::kCharacterKindNum)] =
	{
		{1100,0},
		{Game::kWindowWidth,350},
		{0,600},
		{500,Game::kWindowHeight}
	};

	//�L�����N�^�[�̏����T�C�h
	const TitleUi::DrawCharacterSide kCharacterSide[static_cast<int>(TitleUi::DrawCharacterKind::kCharacterKindNum)] =
	{
		TitleUi::DrawCharacterSide::kUp,
		TitleUi::DrawCharacterSide::kRight,
		TitleUi::DrawCharacterSide::kLeft,
		TitleUi::DrawCharacterSide::kDown
	};


	//�T�C�h���Ƃ̉�]�x
	const double kSideRota[4] =
	{
		DX_PI,//180
		DX_PI + DX_PI / 2 ,//270
		0,//0
		DX_PI / 2//90
	};

	//�ő剽�t���[���L�����N�^�[�������o����
	constexpr int kMaxCharacterMoveFrame = 60;

	//�Œች�t���[���L�����N�^�[�������o����
	constexpr int kMinCharacterMoveFrame = 30;

	//�ŏ����t���[���L�����N�^�[�������o����
	constexpr int kInitCharacterMoveFrame = 120;

	//�L�����N�^�[�̈ړ����x
	constexpr int kCharacterMoveSpeed = 20;

	//�������x�̍ő�l
	constexpr int kMaxFallSpeed = 20;
	//�������x�̍ŏ��l
	constexpr int kMinFallSpeed = 10;

	//�������̉�]���x�̍ő�l
	constexpr double kMaxFallRotaSpeed = 0.3;
	//�������̉�]���x�̍ŏ��l
	constexpr double kMinFallRotaSpeed = 0.05;

	//�W�����v���̈ړ����x�̍ő�l
	constexpr int kMaxJumpSpeed = 40;
	//�W�����v���̈ړ����x�̍ŏ��l
	constexpr int kMinJumpSpeed = 30;

	//�W�����v���ɂ�����d��
	constexpr float kJumpGravity = 0.65f;

	//������ю��̉��ړ����x�̍ő�l
	constexpr int kMaxSideJumpSpeed = 30;
	//������ю��̉��ړ����x�̍ŏ��l
	constexpr int kMinSideJumpSpeed = 20;
	//������ю��̏����㏸���x�̍ő�l
	constexpr int kMaxSideJumpUpSpeed = 15;
	//������ю��̏����㏸���x�̍ŏ��l
	constexpr int kMinSideJumpUpSpeed = 5;
	//������ю��ɂ�����d��
	constexpr float kSideJumpGravity = 0.3f;

	//������ł����L�����N�^�[�̈ړ����x
	constexpr int kBackCharacterMoveSpeed = 10;
	//������ł����L�����N�^�[��Y���W(��ʒ���)
	constexpr int kBackCharacterPosY = Game::kWindowHeight / 2;
	//������ł����L�����N�^�[�̊g�嗦
	constexpr double kBackCharacterScale = 0.3;
	//������ł����L�����N�^�[�̈ړ����x�̍ő�l
	constexpr int kMaxBackCharacterMoveSpeed = 15;
	//������ł����L�����N�^�[�̈ړ����x�̍ŏ��l
	constexpr int kMinBackCharacterMoveSpeed = 5;
	//������ł����L�����N�^�[�̉�]���x�̍ő�l
	constexpr double kMaxBackCharacterRotaSpeed = 0.35;
	//������ł����L�����N�^�[�̉�]���x�̍ŏ��l
	constexpr double kMinBackCharacterRotaSpeed = 0.15;


	//�L�����N�^�[�̉摜�̕�
	constexpr int kCharacterWidth = 350;
	constexpr int kCharacterHeight = 200;

	//�L�����N�^�[�̃n���h���̎��
	constexpr int kCharacterHandleKind = 1;

	//�A�b�v�f�[�g�̎�ނ̐�
	constexpr int kUpdateKindNum = 5;

	//�A�b�v�f�[�g���ꂼ��̊m��
	constexpr int kUpdateProb[kUpdateKindNum] =
	{
		100,//�ʏ�
		10,//������
		20,//�������
		20,//������ł���
		16 //�W�����v
	};

	//�m�������ׂđ������l
	constexpr int kUpdateProbTotal = kUpdateProb[0] + kUpdateProb[1] + kUpdateProb[2] + kUpdateProb[3] + kUpdateProb[4];
}

TitleUi::TitleUi() :
	m_skyDomeHandle(-1),
	m_time(0),
	m_isExistString(true),
	m_fontHandle(-1)
{
	//�t�H���g�̃n���h���̎擾
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

	//�X�J�C�h�[���̊g�嗦�ݒ�
	MV1SetScale(m_skyDomeHandle, VGet(kSkyDomeScale, kSkyDomeScale, kSkyDomeScale));

	//�L�����N�^�[�̏����ݒ�
	for (auto& item : m_characterDrawInfo)
	{
		//�L�����N�^�[�̎�ސݒ�
		item.kind = static_cast<DrawCharacterKind>(&item - &m_characterDrawInfo[0]);

		//�L�����N�^�[�̏������W�ݒ�
		item.pos = kCharacterDrawPos[static_cast<int>(item.kind)];

		//�L�����N�^�[�̏�����]���x�ݒ�
		item.rotaSpeed = 0.0;

		//�L�����N�^�[�̏����ړ����Ԑݒ�
		item.moveTime = kInitCharacterMoveFrame;

		//�L�����N�^�[�̏����T�C�h�ݒ�
		item.side = kCharacterSide[static_cast<int>(item.kind)];

		//�L�����N�^�[�̏�����]�x�ݒ�
		item.rota = kSideRota[static_cast<int>(item.side)];

		//�L�����N�^�[�̏����g�嗦�ݒ�
		item.scale = 1.0;

		//�L�����N�^�[�̏����n���h���ݒ�
		item.handle = GetCharacterHandle(item.kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

		//�L�����N�^�[�̏����X�V�֐��ݒ�
		item.updateFunc = &TitleUi::NormalUpdate;
	}
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

	//�X�J�C�h�[������]������
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kSkyDomeRotaSpeed, 0));

	//�L�����N�^�[�̍X�V���s��
	for (auto& item : m_characterDrawInfo)
	{
		//�L�����N�^�[�̍X�V�֐����Ă�
		(this->*item.updateFunc)(static_cast<int>(item.kind));
	}
}

void TitleUi::Draw()
{
	auto& manager = GraphManager::GetInstance();

	//�X�J�C�h�[���̕`��
	MV1DrawModel(m_skyDomeHandle);

	//������ł����L�����N�^�[�̕`��
	for (auto& item : m_characterDrawInfo)
	{
		if (item.updateFunc == &TitleUi::BackFryUpdate)
		{
			DrawRotaGraph(static_cast<int>(item.pos.x), static_cast<int>(item.pos.y), item.scale, item.rota, item.handle, true);
		}
	}

	//�^�C�g�����S�̕`��
	DrawGraph(0, 0, manager.GetHandle("TitleLogo"), true);

	//������ł����L�����N�^�[�ȊO�̕`��
	for (auto& item : m_characterDrawInfo)
	{
		if (item.updateFunc != &TitleUi::BackFryUpdate)
		{
			DrawRotaGraph(static_cast<int>(item.pos.x), static_cast<int>(item.pos.y), 1.0, item.rota, item.handle, true);
		}
	}

	//������̕`��
	if (m_isExistString)
	{
		DrawStringToHandle(kStringPosX, kStringPosY, kUiString.c_str(), kFontColor, m_fontHandle, GetColor(0, 0, 0));
	}


}


MyEngine::Vector2 TitleUi::GetEnterMoveVec(DrawCharacterSide side)
{
	if (side == DrawCharacterSide::kUp)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}
	else if (side == DrawCharacterSide::kRight)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kLeft)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kDown)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::GetReturnMoveVec(DrawCharacterSide side)
{
	if (side == DrawCharacterSide::kUp)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}
	else if (side == DrawCharacterSide::kRight)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kLeft)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kDown)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::ClampPos(DrawCharacterSide side, MyEngine::Vector2 pos)
{
	MyEngine::Vector2 ans = pos;

	if (side == DrawCharacterSide::kUp)
	{
		ans.y = min(pos.y, kCharacterHeight / 2);
	}
	else if (side == DrawCharacterSide::kRight)
	{
		ans.x = max(pos.x, Game::kWindowWidth - kCharacterHeight / 2);
	}
	else if (side == DrawCharacterSide::kLeft)
	{
		ans.x = min(pos.x, kCharacterHeight / 2);
	}
	else if (side == DrawCharacterSide::kDown)
	{
		ans.y = max(pos.y, Game::kWindowHeight - kCharacterHeight / 2);
	}

	return ans;
}

MyEngine::Vector2 TitleUi::SetRandomPos(DrawCharacterSide side)
{
	//���������o��ꍇ
	if (side == DrawCharacterSide::kUp)
	{
		int randomX = GetRand(Game::kWindowWidth - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(randomX, -kCharacterWidth);
	}
	//�E��������o��ꍇ
	else if (side == DrawCharacterSide::kRight)
	{
		int randomY = GetRand(Game::kWindowHeight - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(Game::kWindowWidth + kCharacterWidth, randomY);
	}
	//����������o��ꍇ
	else if (side == DrawCharacterSide::kLeft)
	{
		int randomY = GetRand(Game::kWindowHeight - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(-kCharacterWidth, randomY);
	}
	//����������o��ꍇ
	else if (side == DrawCharacterSide::kDown)
	{
		int randomX = GetRand(Game::kWindowWidth - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(randomX, Game::kWindowHeight + kCharacterWidth);
	}

	return MyEngine::Vector2(0, 0);
}

bool TitleUi::CheckPos(MyEngine::Vector2 pos)
{

	if (pos.x >= -kCharacterHeight &&
		pos.x <= Game::kWindowWidth + kCharacterHeight &&
		pos.y >= -kCharacterHeight &&
		pos.y <= Game::kWindowHeight + kCharacterHeight)
	{
		return true;
	}
	return false;
}

int TitleUi::GetCharacterHandle(DrawCharacterKind characterKind, CharacterGraphKind graphKind)
{
	//�Ԃ��n���h��
	int ans = -1;

	//�O���t�B�b�N�}�l�[�W���[�̎擾
	auto& manager = GraphManager::GetInstance();

	//�n���h����
	std::string handleName;

	if (characterKind == DrawCharacterKind::kMouse)
	{
		handleName = "Mouse1P";
	}
	else if (characterKind == DrawCharacterKind::kBlue)
	{
		handleName = "Blue1P";
	}
	else if (characterKind == DrawCharacterKind::kMouse2P)
	{
		handleName = "Mouse2P";
	}
	else if (characterKind == DrawCharacterKind::kBlue2P)
	{
		handleName = "Blue2P";
	}

	if (graphKind == CharacterGraphKind::kChoki)
	{
		handleName += "Choki";
	}
	else if (graphKind == CharacterGraphKind::kPar)
	{
		handleName += "Par";
	}

	ans = manager.GetHandle(handleName);

	if (ans == -1)
	{
		printfDx("�n���h�����擾�ł��܂���ł���\n");
	}

	return ans;
}

void TitleUi::SetCharacterUpdateFunc(CharacterUpdateKind kind, DrawCharacterKind character)
{

	//�ʏ�̍X�V
	if (kind == CharacterUpdateKind::kNormal)
	{
		InitNormal(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::NormalUpdate;
	}
	//�����̍X�V
	else if (kind == CharacterUpdateKind::kFall)
	{
		InitFall(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::FallUpdate;
	}
	//�W�����v�̍X�V
	else if (kind == CharacterUpdateKind::kJump)
	{
		InitJump(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::JumpUpdate;
	}
	//������т̍X�V
	else if (kind == CharacterUpdateKind::kSideJump)
	{
		InitSideJump(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::SideJumpUpdate;
	}
	//������ł����L�����N�^�[�̍X�V
	else if (kind == CharacterUpdateKind::kBackFry)
	{
		InitBackFry(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::BackFryUpdate;
	}
}

void TitleUi::InitNormal(int number)
{
	//�T�C�h�������_���Őݒ�
	m_characterDrawInfo[number].side = static_cast<DrawCharacterSide>(GetRand(static_cast<int>(DrawCharacterSide::kSideNum)));

	//���W�������_���Őݒ�
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);

	//�ړ����Ԃ������_���Őݒ�
	m_characterDrawInfo[number].moveTime = GetRand(kMaxCharacterMoveFrame) + kMinCharacterMoveFrame;

	//�n���h���������_���Őݒ�
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

	//��]�x��ݒ�
	m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(m_characterDrawInfo[number].side)];

	//��]���x��ݒ�
	m_characterDrawInfo[number].rotaSpeed = 0.0;

	//�g�嗦��ݒ�
	m_characterDrawInfo[number].scale = kBackCharacterScale;

}

void TitleUi::InitFall(int number)
{
	//������T�E���h���Đ�
	SoundManager::GetInstance().PlayOnceSound("Fall");
	//�ړ����Ԃ�ݒ�
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;

	//�T�C�h����ɐݒ�
	m_characterDrawInfo[number].side = DrawCharacterSide::kUp;

	//���W�������_���Őݒ�
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);

	//�n���h���������_���Őݒ�
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

	//�ړ��x�N�g����ݒ�
	m_characterDrawInfo[number].moveVec = MyEngine::Vector2(0, GetRand(kMaxFallSpeed - kMinFallSpeed) + kMinFallSpeed);

	//�g�嗦��ݒ�
	m_characterDrawInfo[number].scale = kBackCharacterScale;

	//��]���x��ݒ�
	float randomRota = static_cast<float>(GetRand(static_cast<int>((kMaxFallRotaSpeed - kMinFallRotaSpeed) * 100)));

	m_characterDrawInfo[number].rotaSpeed = (randomRota / 100) + kMinFallRotaSpeed;
}

void TitleUi::InitJump(int number)
{
	//������T�E���h���Đ�
	SoundManager::GetInstance().PlayOnceSound("Jump");
	//�ړ����Ԃ�ݒ�
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;
	//�T�C�h�����ɐݒ�
	m_characterDrawInfo[number].side = DrawCharacterSide::kDown;
	//���W�������_���Őݒ�
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);
	//�n���h���������_���Őݒ�
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));
	//�ړ��x�N�g����ݒ�
	m_characterDrawInfo[number].moveVec = MyEngine::Vector2(0, -(GetRand(kMaxJumpSpeed - kMinJumpSpeed) + kMinJumpSpeed));
	//��]���x��ݒ�
	m_characterDrawInfo[number].rotaSpeed = 0.0;
	//��]�x��ύX
	m_characterDrawInfo[number].rota = 0.0;
	//�g�嗦��ݒ�
	m_characterDrawInfo[number].scale = kBackCharacterScale;
}

void TitleUi::InitSideJump(int number)
{
	//������т̃T�E���h���Đ�
	SoundManager::GetInstance().PlayOnceSound("SideJump");

	//�ړ����Ԃ�ݒ�
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;

	//�T�C�h�����E�ǂ��炩�Őݒ�
	int random = GetRand(1);

	//���E�ǂ��炩�o�邩�������_���Őݒ�
	if (random == 0)
	{
		//�T�C�h��ݒ�
		m_characterDrawInfo[number].side = DrawCharacterSide::kLeft;
		//�����ړ��x�N�g����ݒ�
		m_characterDrawInfo[number].moveVec = MyEngine::Vector2(GetRand(kMaxSideJumpSpeed - kMinSideJumpSpeed) + kMinSideJumpSpeed, -GetRand(kMaxSideJumpUpSpeed - kMinSideJumpUpSpeed) - kMinSideJumpUpSpeed);
		//��]�x��ݒ�
		m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(DrawCharacterSide::kLeft)];
	}
	else
	{
		//�T�C�h��ݒ�
		m_characterDrawInfo[number].side = DrawCharacterSide::kRight;
		//�����ړ��x�N�g����ݒ�
		m_characterDrawInfo[number].moveVec = MyEngine::Vector2(-(GetRand(kMaxSideJumpSpeed - kMinSideJumpSpeed) + kMinSideJumpSpeed), -GetRand(kMaxSideJumpUpSpeed - kMinSideJumpUpSpeed) - kMinSideJumpUpSpeed);
		//��]�x��ݒ�
		m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(DrawCharacterSide::kRight)];
	}

	//���W�������_���Őݒ�
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);

	//�n���h���������_���Őݒ�
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

	//�g�嗦��ݒ�
	m_characterDrawInfo[number].scale = kBackCharacterScale;
}

void TitleUi::InitBackFry(int number)
{
	//�ړ����Ԃ�ݒ�
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;
	//�T�C�h���E���ɐݒ�
	m_characterDrawInfo[number].side = static_cast<DrawCharacterSide>(GetRand(static_cast<int>(DrawCharacterSide::kSideNum)));
	//���W�������_���Őݒ�
	m_characterDrawInfo[number].pos = MyEngine::Vector2(Game::kWindowWidth + kCharacterWidth, kBackCharacterPosY);
	//�n���h���������_���Őݒ�
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));
	//�ړ��x�N�g����ݒ�
	m_characterDrawInfo[number].moveVec = MyEngine::Vector2(-(GetRand(kMaxBackCharacterMoveSpeed - kMinBackCharacterMoveSpeed) + kMinBackCharacterMoveSpeed), 0);
	//��]���x��ݒ�
	m_characterDrawInfo[number].rotaSpeed = -static_cast<float>(GetRand(static_cast<int>((kMaxBackCharacterRotaSpeed - kMinBackCharacterRotaSpeed) * 100)) / 100 + kMinBackCharacterRotaSpeed);
	//��]�x��ݒ�
	m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(m_characterDrawInfo[number].side)];
	//�g�嗦��ݒ�
	m_characterDrawInfo[number].scale = kBackCharacterScale;
}

void TitleUi::NormalUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;

	//�ړ����鎞�ԂɂȂ�����
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//�O���Ɍ������ړ��x�N�g�����擾
		MyEngine::Vector2 moveVec = GetReturnMoveVec(m_characterDrawInfo[number].side);

		//���W���ړ�
		m_characterDrawInfo[number].pos += moveVec;

		//��ʊO�ɏo�����ǂ������`�F�b�N

		//�o�Ă�������W�������_���ŉ�ʊO�ɐݒ肷��
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{

			//�X�V�֐��������_���Őݒ�
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);

			//�����_������
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//�����Ō���
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}
	//����ȊO
	else
	{
		//�����Ɍ������ړ��x�N�g�����擾
		MyEngine::Vector2 moveVec = GetEnterMoveVec(m_characterDrawInfo[number].side);

		//���W���ړ�
		m_characterDrawInfo[number].pos += moveVec;
	}

	m_characterDrawInfo[number].pos = ClampPos(m_characterDrawInfo[number].side, m_characterDrawInfo[number].pos);
}

void TitleUi::FallUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;

	//��莞�Ԃ���ʊO�̔�������Ȃ�
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//��ʊO�ɏo����
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//�X�V�֐��������_���Őݒ�
			//�X�V�֐��������_���Őݒ�
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);

			//�����_������
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//�����Ō���
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}
	//���W���ړ�
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//��]�x��ύX
	m_characterDrawInfo[number].rota += m_characterDrawInfo[number].rotaSpeed;

}

void TitleUi::JumpUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;
	//��莞�Ԃ���ʊO�̔�������Ȃ�
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//��ʊO�ɏo����
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//�X�V�֐��������_���Őݒ�
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);

			//�����_������
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//�����Ō���
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}

		}
	}
	//���W���ړ�
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//�d�͂�������
	m_characterDrawInfo[number].moveVec.y += kJumpGravity;
}

void TitleUi::SideJumpUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;
	//��莞�Ԃ���ʊO�̔�������Ȃ�
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//��ʊO�ɏo����
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//�X�V�֐��������_���Őݒ�
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);


			//�����_������
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//�����Ō���
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}
	//���W���ړ�
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//�d�͂�������
	m_characterDrawInfo[number].moveVec.y += kSideJumpGravity;
}

void TitleUi::BackFryUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;
	//��莞�Ԃ���ʊO�̔�������Ȃ�
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//��ʊO�ɏo����
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//�X�V�֐��������_���Őݒ�
			int random = 0;
			int prob = GetRand(kUpdateProbTotal);
			//�����_������
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//�����Ō���
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}

	//���W���ړ�
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//��]�x��ύX
	m_characterDrawInfo[number].rota += m_characterDrawInfo[number].rotaSpeed;
}
