#pragma once
#include "LocalPos.h"

class GameCamera
{
public:
	GameCamera();
	~GameCamera();

	void SetPoseCamera();
	void Update();

	/// <summary>
	/// ���̃N���X���폜����Ƃ��Ɏg�p����֐�
	/// </summary>
	void Final();

	void SetBattleCamera();

	/// <summary>
	/// �v���C���[�̍��W�ƃJ�����̃^�[�Q�b�g���W��ݒ肷��
	/// </summary>
	/// <param name="player">�v���C���[�̍��W</param>
	/// <param name="target">�J�����̃^�[�Q�b�g�̍��W</param>
	void SetCenterAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target);

	/// <summary>
	/// �v���C���[�̌����Ă��������ݒ肷��
	/// </summary>
	/// <param name="pos">�v���C���[�̑O�����Ƃ�����W</param>
	void SetFrontPos(MyEngine::Vector3 pos);

	/// <summary>
	/// �v���C���[�̃x���V�e�B��ݒ肷��
	/// </summary>
	/// <param name="velo">�v���C���[�̃x���V�e�B</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// �J�����̃��[�J�����W������������
	/// </summary>
	void SetLocalPos();

	/// <summary>
	/// �J�����̃��[�J�����W��ݒ肷��
	/// </summary>
	/// <param name="pos">�J�������猩�Ăǂ��ɂ��邩<param>
	void SetLocalPos(MyEngine::Vector3 pos) { m_localPos.SetLocalPos(pos); }

	/// <summary>
	/// �J�������~�߂�
	/// </summary>
	void StopCamera() { m_isStop = true; }

	/// <summary>
	/// �J�������~�߂�(���Ԏw��)
	/// </summary>
	/// <param name="stopTime">�J�������~�߂鎞�Ԃ�ݒ肷��</param>
	void StopCamera(int stopTime) { m_stopTime = stopTime; m_isStop = true; }

	/// <summary>
	/// �J�����̈ړ����n�߂�
	/// </summary>
	void StartMoveCamera() { m_isStop = false; }

	/// <summary>
	/// �J�����������ړ��œ����������Ƃ��ɌĂԊ֐�
	/// </summary>
	void StartFastCameraMove() { m_isFastMove = true; }

	/// <summary>
	/// �����J������h�炷(�h�炷�傫���Œ�)
	/// </summary>
	/// <param name="time">�h�炷����</param>
	void ShakeCamera(int time);

	/// <summary>
	/// �J������h�炷(�h�炷�傫����)
	/// </summary>
	/// <param name="time">�h�炷����</param>
	/// <param name="power">�h�炷�傫��</param>
	void ShakeCamera(int time,int power);

	/// <summary>
	/// ���̊֐����Ă�ł���Ԋɂ₩�ɃJ������h�炷
	/// </summary>
	void SwayCamera() { m_isSway = true;}

private:
	//���o���̃J����
	void PoseUpdate();
	//�o�g�����̃J����
	void BattleUpdate();

private:

	using UpdateFunc = void (GameCamera::*)();
	UpdateFunc m_updateFunc;

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

	//�J�����̕␳���~�߂鎞��
	int m_stopCorrectTime;

	//�J�������~�߂鎞��
	int m_stopTime;

	//�J�����������ړ������邩
	bool m_isFastMove;

	//�J�������~�߂邩�ǂ���
	bool m_isStop;
	
	//�h�炷����
	int m_shakeTime;

	//�h�炷�傫��
	int m_shakePower;

	//�J������h�炷���ǂ���
	bool m_isSway;

	//�J�������ɂ₩�ɗh�炷����
	float m_swayTime;

	//���C�g�n���h��
	int m_lightHandle;


};


