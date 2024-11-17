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

	/// <summary>
	/// �J������h���ݒ肷��
	/// </summary>
	/// <param name="time">�h�炷����</param>
	void ShakeCamera(int time) { m_shakeTime = time; }
private:

	//���[�J�����W(�v���C���[�𒆐S�Ƃ��āA���g�̍��W���v�Z����)
	MyEngine::LocalPos m_localPos;

	//�J�����̃^�[�Q�b�g���W
	MyEngine::Vector3 m_targetPos;

	//�ړ��x�N�g��
	MyEngine::Vector3 m_moveVec;

	//���̃t���[���̃J�����̍��W
	MyEngine::Vector3 m_nextCameraPos;

	//�O�̃t���[���̃J�����̍��W
	MyEngine::Vector3 m_lastCameraPos;

	//�v���C���[�̈ړ��x�N�g��
	MyEngine::Vector3 m_playerVelo;

	//�J�������~�߂鎞��
	int m_stopTime;

	//�J�����������ړ������邩
	bool m_isFastMove;

	//�J�������~�߂邩�ǂ���
	bool m_isStop;
	
	//�h�炷����
	int m_shakeTime;

#ifdef _DEBUG

	//���C�g�n���h��
	int m_lightHandle;

#endif // _DEBUG

};


