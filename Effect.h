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
		kCharge
	};

public:

	/// <summary>
	/// ���̃G�t�F�N�g���Đ����邩��ݒ肷��
	/// </summary>
	/// <param name="kind">�G�t�F�N�g�̎��</param>
	Effect(EffectKind kind);

	/// <summary>
	/// �G�t�F�N�g��
	/// </summary>
	/// <param name="manager">�}�l�[�W���[�̃|�C���^</param>
	/// <param name="pos">�G�t�F�N�g�̍��W</param>
	void Init(std::shared_ptr<EffectManager> manager, MyEngine::Vector3 pos);

	/// <summary>
	/// �G�t�F�N�g�̍Đ�����߂�^�C�~���O�ŌĂ�
	/// </summary>
	void End();

	/// <summary>
	/// �G�t�F�N�g�̃p�X��Ԃ�
	/// </summary>
	/// <returns>�G�t�F�N�g�̃p�X</returns>
	std::string GetPath() { return m_path; }

	/// <summary>
	/// �n���h����ݒ肷��
	/// </summary>
	/// <param name="handle">�n���h��</param>
	void SetHandle(int handle) { m_handle = handle; }
private:

	int m_handle;

	std::string m_path;

	MyEngine::Vector3 m_pos;
};

