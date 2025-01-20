#include "GameUi.h"
#include "GraphManager.h"
#include "DxLib.h"
#include <cmath>
#include "Game.h"
#include "GameSceneConstant.h"
#include "Input.h"

namespace
{
	//��{��HP�o�[�̗̑͗�
	constexpr int kMaxHp = 10000;

	//Hp�o�[�̍��W
	constexpr int kHpBarPosX[2] = { 350,Game::kWindowWidth - 350 };
	constexpr int kHpBarPosY = 80;

	//Hp�o�[�̌��̃{�b�N�X�̑傫��
	constexpr int kBackBoxHalfWidth = 275;
	constexpr int kBackBoxHalfHeight = 40;

	//Hp�o�[�̌��̃{�b�N�X�̐F
	const int kBackBoxColor = GetColor(36, 36, 36);

	//Hp�o�[�̉E���̃{�b�N�X���W
	const int kBigHpBarStartPosX[2] = { kHpBarPosX[0] - 5 ,kHpBarPosX[1] + 5 };
	constexpr int kBigHpBarPosY = kHpBarPosY - 30;

	//Hp�o�[�̍����̃{�b�N�X���W
	constexpr int kSmallHpBarPosY = kHpBarPosY + 3;

	//�傫��Hp�o�[�̑傫��
	constexpr int kBigHpBoxHeight = 75;

	//������Hp�o�[�̏c�̑傫��
	constexpr int kSmallHpBoxHeight = 40;

	//Hp�o�[�̎��_
	const int kHpBarStartPosX[2] = { kHpBarPosX[0] - 263 ,kHpBarPosX[1] + 263 };
	//Hp�o�[�̏I�_
	constexpr int kHpBarEndPosX[2] = { kHpBarPosX[0] + 258,kHpBarPosX[1] - 258 };

	//Hp�o�[�̐�
	constexpr int kHpBarMaxNum = 5;

	//Hp�o�[��h�炷�傫��
	constexpr int kHpBarShakeScale = 10;

	//Hp�o�[�̐F�̎��
	const unsigned int kHpBarColor[kHpBarMaxNum] =
	{
		GetColor(255,255,24),//1�{
		GetColor(0,255,136),//2�{
		GetColor(0,128,68),//3�{
		GetColor(51,170,255),//4�{
		GetColor(0,85,255),//5�{
	};

	//Hp�̎c��{����\���{�b�N�X�̍��W
	constexpr int kRightHpBarNumBoxPosX[kHpBarMaxNum] =
	{
		-999,//0�̎��͕\�����Ȃ��̂Ń}�C�i�X�̒l�����Ă���
		88,
		121,
		154,
		188
	};

	//Hp�̎c��{����\���{�b�N�X�̍��W
	const int kLeftHpBarNumBoxPosX[kHpBarMaxNum] =
	{
		-999,//0�̎��͕\�����Ȃ��̂Ń}�C�i�X�̒l�����Ă���
		Game::kWindowWidth - 88,
		Game::kWindowWidth - 121,
		Game::kWindowWidth - 154,
		Game::kWindowWidth - 188
	};

	constexpr int kHpBarNumBoxPosY = 47;

	//�c��{����\���{�b�N�X�̑傫��
	constexpr int kHpBarNumBoxWidth = 18;
	constexpr int kHpBarNumBoxHeight = 30;

	//�Ԃ��Q�[�W�������Ȃ�܂ł̎���
	constexpr int kRedBarSubStartTime = 60;

	//�Ԃ��Q�[�W�̐F
	const int kRedBarColor = GetColor(255, 12, 12);

	//�Ԃ��Q�[�W�����炷����
	constexpr float kRedBarSubSpeed = 250;

	//Mp�o�[�̐F
	const COLOR_U8 kMpBarColor = GetColorU8(255, 255, 0, 255);

	//���������\��Mp�o�[�̐F
	const COLOR_U8 kSubMpBarColor = GetColorU8(255, 0, 0, 255);

	//�����Mp�����炵�Ă������x
	constexpr float kRedMpBarSubSpeed = 300.0f;

	//Mp�o�[�̉摜��\��������W
	constexpr int kMpBarGraphPosX[2] = { 350,Game::kWindowWidth - 350 };
	constexpr int kMpBarGraphPosY = 150;

	//Mp�o�[�̊J�n���W
	constexpr int kMpBarStartPosX[2] = { 75,Game::kWindowWidth - 75 };
	constexpr int kMpBarUpPosY = 140;
	constexpr int kMpBarUnderPosY = 160;

	//Mp�o�[�̒���
	constexpr int kMpBarMaxLength = 365;

	//Mp�o�[�̏�Ɖ���X���W�̂���
	constexpr int kMpBarGapX = 58;

	//Mp�o�[�̉��ɕ\������D�F�̃{�b�N�X�̑傫��
	constexpr int kMpBarBackBoxHalfWidth = 280;
	constexpr int kMpBarBackBoxHalfHeight = 30;

	//�A�ł���{�^����\��������W
	constexpr int kBashButtonPosX = Game::kWindowWidth / 2;
	constexpr int kBashButtonPosY = 650;

	//�A�ł���{�^���̉摜�̑傫��
	constexpr int kBashButtonGraphWidth = 96;
	constexpr int kBashButtonGraphHeight = 84;

	//�A�ł���{�^����؂�ւ��鎞��
	constexpr int kBashButtonChangeTime = 10;
}

GameUi::GameUi() :
	m_lastHp(),
	m_lastHpBarNum(),
	m_onHitDamageHp(),
	m_hitDamageTime(),
	m_onSubMp(),
	m_bashButtonChangeTime(0)
{
}

void GameUi::RetryInit()
{
	//�l�����ׂď���������
	for (auto& num : m_lastHp)			 num = 0.0f;
	for (auto& num : m_lastHpBarNum)	 num = 0.0f;
	for (auto& num : m_onHitDamageHp)	 num = 0.0f;
	for (auto& num : m_onSubMp)			 num = 0.0f;
	for (auto& num : m_hitDamageTime)	 num = 0;

	for (auto& pos : m_shakePos) pos = MyEngine::Vector2(0, 0);
}

void GameUi::DrawHpBar(float hp, bool isLeft)
{
	int barGraphHandle = GraphManager::GetInstance().GetHandle("HpBar");
	int barNumGraphHandle = GraphManager::GetInstance().GetHandle("HpBarNum");

	if (isLeft)
	{
		//���̊֐������߂ēǂ񂾃^�C�~���O�Ȃ��
		if (m_lastHp[0] == 0)
		{
			//����������
			m_lastHp[0] = hp;
			m_lastHpBarNum[0] = (hp / kMaxHp) - 1;
		}

		//�_���[�W���󂯂Ă�������
		if (m_lastHp[0] > hp)
		{
			//Hp�o�[��h�炷
			m_shakePos[0].x = static_cast<float>(GetRand(kHpBarShakeScale));
			m_shakePos[0].y = static_cast<float>(GetRand(kHpBarShakeScale));
			if (m_hitDamageTime[0] < 0)
			{
				m_onHitDamageHp[0] = m_lastHp[0];
			}
			//�Ԃ��o�[������܂ł̎��Ԃ�ݒ�
			m_hitDamageTime[0] = kRedBarSubStartTime;
		}
		else
		{
			m_shakePos[0].x = 0;
			m_shakePos[0].y = 0;
		}

		//���̍����{�b�N�X�`��
		DrawBox(static_cast<int>(kHpBarPosX[0] - kBackBoxHalfWidth + m_shakePos[0].x), static_cast<int>(kHpBarPosY - kBackBoxHalfHeight + m_shakePos[0].y),//�n�_
			static_cast<int>(kHpBarPosX[0] + kBackBoxHalfWidth + m_shakePos[0].x), static_cast<int>(kHpBarPosY + kBackBoxHalfHeight + m_shakePos[0].y),//�I�_
			kBackBoxColor, true);

		//���݂̗̑͂�1���ŋ�؂�
		int nowHp = static_cast<int>(hp) % kMaxHp;
		int hpBarNum = static_cast<int>(hp / kMaxHp);

		//�̗͂����傤��10000�Ŋ���؂ꂽ�ꍇ�̏���
		if (hp > 0 && nowHp == 0)
		{
			nowHp = kMaxHp;
			hpBarNum--;
		}

		//Hp�o�[�̐�
		hpBarNum = std::min<int>(hpBarNum, kHpBarMaxNum - 1);

		//Hp�o�[�̐��������Ă�����
		if (m_lastHpBarNum[0] > hpBarNum)
		{
			//�Ԃ��o�[�����Z�b�g����
			m_onHitDamageHp[0] = m_lastHpBarNum[0] * kMaxHp;

			m_hitDamageTime[0] = kRedBarSubStartTime;
		}


		//Hp�o�[�̐���ۑ����Ă���
		m_lastHpBarNum[0] = static_cast<float>(hpBarNum);

		//Hp�o�[�̐F
		int barColor = kHpBarColor[hpBarNum];

		//�̗͂̊���
		float hpRate = nowHp / static_cast<float>(kMaxHp);

		//0�ȉ��ɂȂ�Ȃ��悤��
		hpRate = std::fmax(hpRate, 0.0f);

		//Hp�o�[�̏I�_
		int hpBarEndPos = static_cast<int>((kHpBarEndPosX[0] - kHpBarStartPosX[0]) * hpRate) + kHpBarStartPosX[0];


		//Hp�o�[��2�{�ȏ゠���
		if (hpBarNum > 0)
		{
			//����̃o�[�̊k���g�p����
			int backBarColor = kHpBarColor[hpBarNum - 1];

			//�����Hp�o�[��\�����Ă���
			DrawBox(static_cast<int>(kHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[0].y),//�n�_
				static_cast<int>(kHpBarEndPosX[0] + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[0].y),//�I�_
				backBarColor, true);

			DrawBox(static_cast<int>(kBigHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + m_shakePos[0].y),//�n�_
				static_cast<int>(kHpBarEndPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[0].y),//�I�_
				backBarColor, true);
		}


		//�Ԃ��Q�[�W�����炷�^�C�~���O��ݒ�
		m_hitDamageTime[0]--;

		int lastHp = static_cast<int>(m_onHitDamageHp[0]) % kMaxHp;

		//�̗͂����傤��10000�Ŋ���؂ꂽ�ꍇ�̏���
		if (hp > 0 && lastHp == 0)
		{
			lastHp = kMaxHp;
		}

		float redBarRate = lastHp / static_cast<float>(kMaxHp);

		redBarRate = std::fmax(redBarRate, 0.0f);

		int damageBarEndPos = static_cast<int>((kHpBarEndPosX[0] - kHpBarStartPosX[0]) * redBarRate) + kHpBarStartPosX[0];

		//�������ق��̃Q�[�W�̕\��
		DrawBox(static_cast<int>(hpBarEndPos + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[0].y),//�n�_
			static_cast<int>(damageBarEndPos + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[0].y),
			kRedBarColor, true);

		//�I�_���n�_���������ɂ����
		if (damageBarEndPos > kBigHpBarStartPosX[0])
		{
			//�傫���ق��̃Q�[�W�̕\��
			DrawBox(static_cast<int>(kBigHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + m_shakePos[0].y),//�n�_
				static_cast<int>(damageBarEndPos + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[0].y),//�I�_
				kRedBarColor, true);
		}

		//�_���[�W���󂯂Ă����莞�Ԃ�������
		if (m_hitDamageTime[0] < 0)
		{
			m_onHitDamageHp[0] -= kRedBarSubSpeed;
			m_onHitDamageHp[0] = std::fmax(m_onHitDamageHp[0], hp);
		}

		//�������ق��̗̑͂̕\��
		DrawBox(static_cast<int>(kHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[0].y),//�n�_
			static_cast<int>(hpBarEndPos + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[0].y),//�I�_
			barColor, true);

		//�I�_���n�_���������ɂ����
		if (hpBarEndPos > kBigHpBarStartPosX[0])
		{
			//�傫���ق��̗̑͂̕\��
			DrawBox(static_cast<int>(kBigHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + m_shakePos[0].y),//�n�_
				static_cast<int>(hpBarEndPos + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[0].y),//�I�_
				barColor, true);
		}

		//�c���Ă���̗̖͂{����\������
		for (int i = 0; i <= hpBarNum; i++)
		{
			DrawBox(static_cast<int>(kRightHpBarNumBoxPosX[i] + m_shakePos[0].x), static_cast<int>(kHpBarNumBoxPosY + m_shakePos[0].y),
				static_cast<int>(kRightHpBarNumBoxPosX[i] + kHpBarNumBoxWidth + m_shakePos[0].x), static_cast<int>(kHpBarNumBoxPosY + kHpBarNumBoxHeight + m_shakePos[0].y),//�I�_
				GetColor(32, 255, 32), true);
		}


		//�g�̕\��
		DrawRotaGraph(static_cast<int>(kHpBarPosX[0] + m_shakePos[0].x), static_cast<int>(kHpBarPosY + m_shakePos[0].y), 1.0, 0.0, barGraphHandle, true, false);

		m_lastHp[0] = hp;
	}
	else
	{
		//���̊֐������߂ēǂ񂾃^�C�~���O�Ȃ��
		if (m_lastHp[1] == 0)
		{
			//����������
			m_lastHp[1] = hp;
			m_lastHpBarNum[1] = (hp / kMaxHp) - 1;
		}

		//�_���[�W���󂯂Ă�������
		if (m_lastHp[1] > hp)
		{
			//Hp�o�[��h�炷
			m_shakePos[1].x = static_cast<float>(GetRand(kHpBarShakeScale));
			m_shakePos[1].y = static_cast<float>(GetRand(kHpBarShakeScale));
			if (m_hitDamageTime[1] < 0)
			{
				m_onHitDamageHp[1] = m_lastHp[1];
			}
			//�Ԃ��o�[������܂ł̎��Ԃ�ݒ�
			m_hitDamageTime[1] = kRedBarSubStartTime;
		}
		else
		{
			m_shakePos[1].x = 0;
			m_shakePos[1].y = 0;
		}

		//���̍����{�b�N�X�`��
		DrawBox(static_cast<int>(kHpBarPosX[1] + kBackBoxHalfWidth + m_shakePos[1].x), static_cast<int>(kHpBarPosY - kBackBoxHalfHeight + m_shakePos[1].y),//�n�_
			static_cast<int>(kHpBarPosX[1] - kBackBoxHalfWidth + m_shakePos[1].x), static_cast<int>(kHpBarPosY + kBackBoxHalfHeight + m_shakePos[1].y),//�I�_
			kBackBoxColor, true);

		//���݂̗̑͂�1���ŋ�؂�
		int nowHp = static_cast<int>(hp) % kMaxHp;
		int hpBarNum = static_cast<int>(hp / kMaxHp);

		//�̗͂����傤��10000�Ŋ���؂ꂽ�ꍇ�̏���
		if (hp > 0 && nowHp == 0)
		{
			nowHp = kMaxHp;
			hpBarNum--;
		}

		//Hp�o�[�̐�
		hpBarNum = std::min<int>(hpBarNum, kHpBarMaxNum - 1);

		//Hp�o�[�̐��������Ă�����
		if (m_lastHpBarNum[1] > hpBarNum)
		{
			//�Ԃ��o�[�����Z�b�g����
			m_onHitDamageHp[1] = m_lastHpBarNum[1] * kMaxHp;

			m_hitDamageTime[1] = kRedBarSubStartTime;
		}


		//Hp�o�[�̐���ۑ����Ă���
		m_lastHpBarNum[1] = static_cast<float>(hpBarNum);

		//Hp�o�[�̐F
		int barColor = kHpBarColor[hpBarNum];

		//�̗͂̊���
		float hpRate = nowHp / static_cast<float>(kMaxHp);

		//0�ȉ��ɂȂ�Ȃ��悤��
		hpRate = std::fmax(hpRate, 0.0f);

		//Hp�o�[�̏I�_
		int hpBarEndPos = kHpBarStartPosX[1] - static_cast<int>((kHpBarStartPosX[1] - kHpBarEndPosX[1]) * hpRate);


		//Hp�o�[��2�{�ȏ゠���
		if (hpBarNum > 0)
		{
			//����̃o�[�̊k���g�p����
			int backBarColor = kHpBarColor[hpBarNum - 1];

			//�����Hp�o�[��\�����Ă���
			DrawBox(static_cast<int>(kHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[1].y),//�n�_
				static_cast<int>(kHpBarEndPosX[1] + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[1].y),//�I�_
				backBarColor, true);

			DrawBox(static_cast<int>(kBigHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + m_shakePos[1].y),//�n�_
				static_cast<int>(kHpBarEndPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[1].y),//�I�_
				backBarColor, true);
		}


		//�Ԃ��Q�[�W�����炷�^�C�~���O��ݒ�
		m_hitDamageTime[1]--;

		int lastHp = static_cast<int>(m_onHitDamageHp[1]) % kMaxHp;

		//�̗͂����傤��10000�Ŋ���؂ꂽ�ꍇ�̏���
		if (hp > 0 && lastHp == 0)
		{
			lastHp = kMaxHp;
		}

		float redBarRate = lastHp / static_cast<float>(kMaxHp);

		redBarRate = std::fmax(redBarRate, 0.0f);

		int damageBarEndPos = kHpBarStartPosX[1] - static_cast<int>((kHpBarStartPosX[1] - kHpBarEndPosX[1]) * redBarRate);

		//�������ق��̃Q�[�W�̕\��
		DrawBox(static_cast<int>(hpBarEndPos + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[1].y),//�n�_
			static_cast<int>(damageBarEndPos + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[1].y),//�I�_
			kRedBarColor, true);

		//�I�_���n�_���������ɂ����
		if (damageBarEndPos < kBigHpBarStartPosX[1])
		{
			//�傫���ق��̃Q�[�W�̕\��
			DrawBox(static_cast<int>(kBigHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + m_shakePos[1].y),//�n�_
				static_cast<int>(damageBarEndPos + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[1].y),//�I�_
				kRedBarColor, true);
		}

		//�_���[�W���󂯂Ă����莞�Ԃ�������
		if (m_hitDamageTime[1] < 0)
		{
			m_onHitDamageHp[1] -= kRedBarSubSpeed;
			m_onHitDamageHp[1] = std::fmax(m_onHitDamageHp[1], hp);
		}

		//�������ق��̗̑͂̕\��
		DrawBox(static_cast<int>(kHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[1].y),//�n�_
			static_cast<int>(hpBarEndPos + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[1].y),//�I�_
			barColor, true);

		//�I�_���n�_�����E���ɂ����
		if (hpBarEndPos < kBigHpBarStartPosX[1])
		{
			//�傫���ق��̗̑͂̕\��
			DrawBox(static_cast<int>(kBigHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + m_shakePos[1].y),//�n�_
				static_cast<int>(hpBarEndPos + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[1].y),//�I�_
				barColor, true);
		}

		//�c���Ă���̗̖͂{����\������
		for (int i = 0; i <= hpBarNum; i++)
		{
			DrawBox(static_cast<int>(kLeftHpBarNumBoxPosX[i] + m_shakePos[1].x), static_cast<int>(kHpBarNumBoxPosY + m_shakePos[1].y),//�n�_
				static_cast<int>(kLeftHpBarNumBoxPosX[i] - kHpBarNumBoxWidth + m_shakePos[1].x), static_cast<int>(kHpBarNumBoxPosY + kHpBarNumBoxHeight + m_shakePos[1].y),//�I�_
				GetColor(32, 255, 32), true);
		}


		//�g�̕\��
		DrawRotaGraph(static_cast<int>(kHpBarPosX[1] + m_shakePos[1].x), static_cast<int>(kHpBarPosY + m_shakePos[1].y), 1.0, 0.0, barGraphHandle, true, true);

		m_lastHp[1] = hp;
	}
}

void GameUi::DrawMpBar(float mp, bool isLeft)
{
	int barGraphHandle = GraphManager::GetInstance().GetHandle("MpBar");

	if (isLeft)
	{
		//mp�ʂ������Ă�����
		if (mp >= m_onSubMp[0])
		{
			//����������
			m_onSubMp[0] = mp;
		}
		//mp�ʂ������Ă�����
		else
		{
			m_onSubMp[0] -= kRedMpBarSubSpeed;
			//�����肷���Ȃ��悤�ɃN�����v
			m_onSubMp[0] = std::fmax(m_onSubMp[0], mp);
		}

		//Mp�o�[�̒���
		float subMpBarLength = kMpBarMaxLength * (m_onSubMp[0] / GameSceneConstant::kMaxMp);

		VERTEX2D vertex[6];

		//�Ԃ��Q�[�W�̍��W(���������̋C�͗ʂ�\��)

		//����̃|���S��
		vertex[0].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX, kMpBarUpPosY, 0);
		vertex[0].dif = kSubMpBarColor;
		vertex[0].rhw = 1.0f;
		vertex[0].u = 0.0f;
		vertex[0].v = 0.0f;

		//�E��
		vertex[1].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX + subMpBarLength, kMpBarUpPosY, 0);
		vertex[1].dif = kSubMpBarColor;
		vertex[1].rhw = 1.0f;
		vertex[1].u = 0.0f;
		vertex[1].v = 0.0f;

		//����
		vertex[2].pos = VGet(kMpBarStartPosX[0], kMpBarUnderPosY, 0);
		vertex[2].dif = kSubMpBarColor;
		vertex[2].rhw = 1.0f;
		vertex[2].u = 0.0f;
		vertex[2].v = 0.0f;


		//�E��
		vertex[3].pos = VGet(kMpBarStartPosX[0] + subMpBarLength, kMpBarUnderPosY, 0);
		vertex[3].dif = kSubMpBarColor;
		vertex[3].rhw = 1.0f;
		vertex[3].u = 0.0f;
		vertex[3].v = 0.0f;

		//2�ڂ̎O�p�`�̑�2���_�͍����̒��_�Ȃ̂ŃR�s�[
		vertex[4] = vertex[2];

		//2�|���S���ڂ̑�3���_�͉E��̒��_�Ȃ̂ŃR�s�[
		vertex[5] = vertex[1];

		//Mp�o�[�̉��̊D�F�̃{�b�N�X�\��
		DrawBox(kMpBarGraphPosX[0] - kMpBarBackBoxHalfWidth, kMpBarUpPosY, kMpBarGraphPosX[0] + kMpBarBackBoxHalfWidth, kMpBarUnderPosY, GetColor(192, 192, 192), true);

		//Mp�o�[���������ʂ�\���Ԃ��o�[��\��
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		for (auto& poly : vertex)
		{
			poly.dif = kMpBarColor;
		}

		//�ʏ�̍��W��ݒ�

		float mpBarLength = kMpBarMaxLength * (mp / GameSceneConstant::kMaxMp);

		//����̃|���S��
		vertex[0].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX, kMpBarUpPosY, 0);

		//�E��
		vertex[1].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX + mpBarLength, kMpBarUpPosY, 0);

		//����
		vertex[2].pos = VGet(kMpBarStartPosX[0], kMpBarUnderPosY, 0);

		//�E��
		vertex[3].pos = VGet(kMpBarStartPosX[0] + mpBarLength, kMpBarUnderPosY, 0);


		//2�ڂ̎O�p�`�̑�2���_�͍����̒��_�Ȃ̂ŃR�s�[
		vertex[4].pos = vertex[2].pos;

		//2�|���S���ڂ̑�3���_�͉E��̒��_�Ȃ̂ŃR�s�[
		vertex[5].pos = vertex[1].pos;

		//Mp�o�[��\��
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		DrawRotaGraph(kMpBarGraphPosX[0], kMpBarGraphPosY, 1.0, 0.0, barGraphHandle, true);
	}
	else
	{
		//mp�ʂ������Ă�����
		if (mp >= m_onSubMp[1])
		{
			//����������
			m_onSubMp[1] = mp;
		}
		//mp�ʂ������Ă�����
		else
		{
			m_onSubMp[1] -= kRedMpBarSubSpeed;
			//�����肷���Ȃ��悤�ɃN�����v
			m_onSubMp[1] = std::fmax(m_onSubMp[1], mp);
		}

		//Mp�o�[�̒���
		float subMpBarLength = kMpBarMaxLength * (m_onSubMp[1] / GameSceneConstant::kMaxMp);

		VERTEX2D vertex[6];

		//�Ԃ��Q�[�W�̍��W(���������̋C�͗ʂ�\��)

		//�E��̃|���S��
		vertex[0].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX, kMpBarUpPosY, 0);
		vertex[0].dif = kSubMpBarColor;
		vertex[0].rhw = 1.0f;
		vertex[0].u = 0.0f;
		vertex[0].v = 0.0f;

		//����
		vertex[1].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX - subMpBarLength, kMpBarUpPosY, 0);
		vertex[1].dif = kSubMpBarColor;
		vertex[1].rhw = 1.0f;
		vertex[1].u = 0.0f;
		vertex[1].v = 0.0f;

		//����
		vertex[2].pos = VGet(kMpBarStartPosX[1], kMpBarUnderPosY, 0);
		vertex[2].dif = kSubMpBarColor;
		vertex[2].rhw = 1.0f;
		vertex[2].u = 0.0f;
		vertex[2].v = 0.0f;


		//�E��
		vertex[3].pos = VGet(kMpBarStartPosX[1] - subMpBarLength, kMpBarUnderPosY, 0);
		vertex[3].dif = kSubMpBarColor;
		vertex[3].rhw = 1.0f;
		vertex[3].u = 0.0f;
		vertex[3].v = 0.0f;

		//2�ڂ̎O�p�`�̑�2���_�͍����̒��_�Ȃ̂ŃR�s�[
		vertex[4] = vertex[2];

		//2�|���S���ڂ̑�3���_�͉E��̒��_�Ȃ̂ŃR�s�[
		vertex[5] = vertex[1];

		//Mp�o�[�̉��̊D�F�̃{�b�N�X�\��
		DrawBox(kMpBarGraphPosX[1] - kMpBarBackBoxHalfWidth, kMpBarUpPosY, kMpBarGraphPosX[1] + kMpBarBackBoxHalfWidth, kMpBarUnderPosY, GetColor(192, 192, 192), true);

		//Mp�o�[���������ʂ�\���Ԃ��o�[��\��
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		for (auto& poly : vertex)
		{
			poly.dif = kMpBarColor;
		}

		//�ʏ�̍��W��ݒ�

		float mpBarLength = kMpBarMaxLength * (mp / GameSceneConstant::kMaxMp);

		//����̃|���S��
		vertex[0].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX, kMpBarUpPosY, 0);

		//�E��
		vertex[1].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX - mpBarLength, kMpBarUpPosY, 0);

		//����
		vertex[2].pos = VGet(kMpBarStartPosX[1], kMpBarUnderPosY, 0);

		//�E��
		vertex[3].pos = VGet(kMpBarStartPosX[1] - mpBarLength, kMpBarUnderPosY, 0);


		//2�ڂ̎O�p�`�̑�2���_�͍����̒��_�Ȃ̂ŃR�s�[
		vertex[4].pos = vertex[2].pos;

		//2�|���S���ڂ̑�3���_�͉E��̒��_�Ȃ̂ŃR�s�[
		vertex[5].pos = vertex[1].pos;

		//Mp�o�[��\��
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		DrawRotaGraph(kMpBarGraphPosX[1], kMpBarGraphPosY, 1.0, 0.0, barGraphHandle, true, true);
	}

}

void GameUi::DrawFade(int color, int alpha)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameUi::DrawBashButton(std::string button)
{
	auto& graphManager = GraphManager::GetInstance();

	int buttonHandle = graphManager.GetHandle("Bash" + button);

	//�{�^����؂�ւ��鎞�Ԃ�}���Ă���
	m_bashButtonChangeTime++;

	int cutPos = kBashButtonGraphWidth;

	if (m_bashButtonChangeTime % static_cast<int>(kBashButtonChangeTime * 2) < kBashButtonChangeTime)
	{
		cutPos = 0;
	}

	//�{�^���̌��̔w�i��\������
	DrawRotaGraph(kBashButtonPosX, kBashButtonPosY,1.0,0.0,graphManager.GetHandle("BashBack"),true);

	//�{�^����`�悷��
	DrawRectRotaGraph(kBashButtonPosX, kBashButtonPosY,cutPos,0,kBashButtonGraphWidth,kBashButtonGraphHeight,
	1.0,0.0,buttonHandle,true);
}
