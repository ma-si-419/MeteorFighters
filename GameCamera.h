#pragma once
#include "LocalPos.h"

class GameCamera
{
public:
	GameCamera();
	~GameCamera();

	void Init(MyEngine::Vector3 centerPos);
	void Update();

	/// <summary>
	/// �v���C���[�̍��W�ƃJ�����̃^�[�Q�b�g���W��ݒ肷��
	/// </summary>
	/// <param name="player">�v���C���[�̍��W</param>
	/// <param name="target">�J�����̃^�[�Q�b�g�̍��W</param>
	void SetPlayerPosAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target);

	/// <summary>
	/// �v���C���[�̌����Ă��������ݒ肷��
	/// </summary>
	/// <param name="pos">�v���C���[�̑O�����Ƃ�����W</param>
	void SetPlayerFrontPos(MyEngine::Vector3 pos);

	/// <summary>
	/// �v���C���[�̃x���V�e�B��ݒ肷��
	/// </summary>
	/// <param name="velo">�v���C���[�̃x���V�e�B</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// �X�N���[���Ȃǂ�ύX�������ɃJ�����̍��W�Ȃǂ�ݒ肷��
	/// </summary>
	void SetCamera();

	/// <summary>
	/// �J�������~�߂�
	/// </summary>
	void StopCamera() { m_isStop = true; }

	/// <summary>
	/// �J�����̈ړ����n�߂�
	/// </summary>
	void StartMoveCamera() { m_isStop = false; }

	/// <summary>
	/// �J�����������ړ��œ����������Ƃ��ɌĂԊ֐�
	/// </summary>
	void StartFastCameraMove() { m_isFastMove = true; }

private:

	LocalPos m_localPos;

	MyEngine::Vector3 m_targetPos;

	MyEngine::Vector3 m_moveVec;

	MyEngine::Vector3 m_nextCameraPos;

	MyEngine::Vector3 m_lastCameraPos;

	MyEngine::Vector3 m_playerVelo;

	int m_stopTime;

	int m_lightHandle;

	bool m_isFastMove;

	bool m_isStop;
};

