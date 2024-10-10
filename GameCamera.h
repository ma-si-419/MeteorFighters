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
	void SetFrontPos(MyEngine::Vector3 pos);


private:

	LocalPos m_localPos;

	MyEngine::Vector3 m_targetPos;

	int m_cameraHandle;
};

