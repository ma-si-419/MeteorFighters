#pragma once
#include "Vector3.h"
#include <memory>
#include <string>
class EffectManager;
class Effect : public std::enable_shared_from_this<Effect>
{
public:
	enum class EffectKind
	{
		kNone,
		kCharge,
		kDash,
		kDashEnd,
		kEnergy,
		kBigEnergy,
		kGuardHit,
		kLowHit,
		kMiddleHit,
		kHighHit,
		kEnergyHit,
		kTeleportaion,
		kStageHit,
		kLaserHit,
		kStartLaser,
		kFalls,
		kShock
	};

public:

	/// <summary>
	/// ���̃G�t�F�N�g���Đ����邩��ݒ肷��
	/// </summary>
	/// <param name="kind">�G�t�F�N�g�̎��</param>
	Effect(EffectKind kind);

	/// <summary>
	/// �G�t�F�N�g�̃p�X���w�肵�Đݒ�
	/// </summary>
	/// <param name="string">�G�t�F�N�g�̃p�X</param>
	Effect(std::string string);

	/// <summary>
	/// �G�t�F�N�g�̍��W�Ȃǂ�ݒ肵�A�}�l�[�W���[�ɃG�t�F�N�g��o�^����
	/// </summary>
	/// <param name="manager">�}�l�[�W���[�̃|�C���^</param>
	/// <param name="pos">�G�t�F�N�g�̍��W</param>
	void Init(std::shared_ptr<EffectManager> manager, MyEngine::Vector3 pos);

	/// <summary>
	/// �Đ����ԂȂǂ��X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �G�t�F�N�g�̍Đ�����߂�^�C�~���O�ŌĂ�
	/// </summary>
	void End();

	/// <summary>
	/// �G�t�F�N�g�̃p�X��Ԃ�
	/// </summary>
	/// <returns>�G�t�F�N�g�̃p�X</returns>
	std::string GetPath() const { return m_path; }

	/// <summary>
	/// �n���h����ݒ肷��
	/// </summary>
	/// <param name="handle">�n���h��</param>
	void SetPlayHandle(int handle) { m_handle = handle; }

	/// <summary>
	/// �n���h�����擾����
	/// </summary>
	/// <returns>�n���h��</returns>
	int GetPlayHandle() {return m_handle; }

	/// <summary>
	/// ���݂̍��W���擾����
	/// </summary>
	/// <returns></returns>
	MyEngine::Vector3 GetPos() { return m_pos; }

	/// <summary>
	/// ���݂̍��W��ݒ肷��
	/// </summary>
	/// <param name="pos">�ݒ肵�������W</param>
	void SetPos(MyEngine::Vector3 pos) { m_pos = pos; }


	/// <summary>
	/// ���[�v�̐ݒ������
	/// </summary>
	/// <param name="loopStart">���[�v�̊J�n�t���[��</param>
	/// <param name="loopEnd">���[�v�̏I���t���[��</param>
	void SetLoop(float loopStart,float loopEnd);

	/// <summary>
	/// ���[�v�����Ȃ����ɌĂ�(�G�t�F�N�g��ύX������ȂǂɎg�p)
	/// </summary>
	void SetLoop();

	/// <summary>
	/// ���[�v�̏I���t���[���ɂȂ��������擾����(���[�v���Ȃ����̂�false��Ԃ�)
	/// </summary>
	/// <returns>���[�v����t���[���ɗ������ǂ���</returns>
	bool IsEndLoop();
	
	/// <summary>
	/// ���[�v�̃X�^�[�g�t���[�����擾����
	/// </summary>
	/// <returns>���[�v�̃X�^�[�g</returns>
	float GetLoopStartFrame() { return m_loopStartTime; }

	/// <summary>
	/// ���[�v�̎��Ԃ����Z�b�g����
	/// </summary>
	void ResetLoop() { m_time = m_loopStartTime; }

	/// <summary>
	/// ��]��ݒ肷��
	/// </summary>
	/// <param name="rotation">�ݒ肵������]�x�N�g��</param>
	void SetRotation(MyEngine::Vector3 rotation) { m_rotation = rotation; }
	

	/// <summary>
	/// ��]���擾����
	/// </summary>
	/// <returns>��]�x�N�g��</returns>
	MyEngine::Vector3 GetRotation() { return m_rotation; }

	/// <summary>
	/// ���C�t�^�C����ݒ肷��
	/// </summary>
	/// <param name="time">�����ŏ����܂ł̎���</param>
	void SetLifeTime(float time) { m_lifeTime = time; }

	/// <summary>
	/// �Đ����x�ɍ��킹�Đ������Ԃ��L�΂�
	/// </summary>
	/// <param name="speed">�Đ����x</param>
	void SetPlaySpeed(float speed) { m_playSpeed = speed; }

	/// <summary>
	/// �Đ����x���f�t�H���g�ɖ߂�
	/// </summary>
	void SetPlaySpeed();

	/// <summary>
	/// �������Ԃ��I���������ǂ�����Ԃ�(�ݒ肳��Ă��Ȃ����false��Ԃ�)
	/// </summary>
	/// <returns>�I�����Ă����true��Ԃ�</returns>
	bool IsEndLifeTime();
private:

	int m_handle;

	std::string m_path;

	MyEngine::Vector3 m_pos;

	MyEngine::Vector3 m_rotation;

	float m_time;

	float m_lifeTime;

	float m_playSpeed;

	float m_loopStartTime;
	
	float m_loopEndTime;
};

