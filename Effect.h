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
		kCharge,
		kDash,
		kDashEnd,
		kEnergy
	};

public:

	/// <summary>
	/// ���̃G�t�F�N�g���Đ����邩��ݒ肷��
	/// </summary>
	/// <param name="kind">�G�t�F�N�g�̎��</param>
	Effect(EffectKind kind);

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
	void SetHandle(int handle) { m_handle = handle; }

	/// <summary>
	/// �n���h�����擾����
	/// </summary>
	/// <returns>�n���h��</returns>
	int GetHandle() {return m_handle; }

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
	/// <param name="time">���[�v���鎞��</param>
	void SetLoop(int loopStart,int loopEnd);

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
	int GetLoopStartFrame() { return m_loopStartTime; }

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

private:

	int m_handle;

	std::string m_path;

	MyEngine::Vector3 m_pos;

	MyEngine::Vector3 m_rotation;

	int m_time;

	int m_loopStartTime;
	
	int m_loopEndTime;
};

