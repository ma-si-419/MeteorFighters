#pragma once
#include <list>
#include <map>
#include <memory>
#include <string>
#include "Vector3.h"

class Effect;
class EffectManager
{
public:

	EffectManager();

	/// <summary>
	/// �G�t�F�N�g��o�^����
	/// </summary>
	/// <param name="effect">�o�^����G�t�F�N�g�̃|�C���^</param>
	/// <param name="pos">�G�t�F�N�g�̍��W</param>
	void Entry(std::shared_ptr<Effect> effect,MyEngine::Vector3 pos);

	/// <summary>
	/// �o�^���Ă���G�t�F�N�g���폜����
	/// </summary>
	/// 
	/// <param name="effect">�폜����G�t�F�N�g�̃|�C���^</param>
	void Exit(std::shared_ptr<Effect> effect);

	/// <summary>
	/// ���̃N���X���폜����Ƃ��Ɏg�p����֐�
	/// </summary>
	void Final();

	/// <summary>
	/// �G�t�F�N�g�̍X�V���s��
	/// </summary>
	void Update();

	/// <summary>
	/// �G�t�F�N�g�̕`����s��
	/// </summary>
	void Draw();

	/// <summary>
	/// �G�t�F�N�g�̍Đ����x��ݒ肷��
	/// </summary>
	/// <param name="playSpeed"></param>
	void SetEffectPlaySpeed(float playSpeed);
	
	/// <summary>
	/// �G�t�F�N�g�̍Đ����x���f�t�H���g�ɖ߂�
	/// </summary>
	void SetEffectPlaySpeed();

	/// <summary>
	/// �G�t�F�N�g�����ׂč폜����
	/// </summary>
	void DeletePlayEffect();

private:

	std::list<std::shared_ptr<Effect>> m_effects;

	std::map<std::string, int> m_handles;
};

