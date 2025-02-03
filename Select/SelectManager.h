#pragma once
#include <memory>

class SelectUi;
class SelectManager
{
public:

	enum class CharacterNumber
	{
		kMouse,
		kRandom,
		kBlueHead,
		kCharacterNum
	};

public:
	SelectManager();

	virtual ~SelectManager();

	void Init();

	void Update();

	void Draw();

	/// <summary>
	/// 1�v���C���[���̃L�����N�^�[�̔ԍ���Ԃ�
	/// </summary>
	/// <returns>1�v���C���[�̃L�����N�^�[�̔ԍ���Ԃ�</returns>
	int GetPlayerNumber();

	/// <summary>
	/// 2�v���C���[���̃L�����N�^�[�̔ԍ���Ԃ�
	/// </summary>
	/// <returns>2�v���C���[�̃L�����N�^�[�̔ԍ���Ԃ�</returns>
	int GetEnemyNumber();

	/// <summary>
	/// �G�l�~�[�̃��x����Ԃ�
	/// </summary>
	/// <returns>�G�l�~�[�̃��x��</returns>
	int GetEnemyLevel() { return m_enemyLevel; }

	/// <summary>
	/// ���̃t���[���ňڍs����V�[�����擾����
	/// </summary>
	/// <returns>���̃V�[��(Game::Scene�L���X�g���Ďg�p)</returns>
	int GetNextScene() { return m_nextScene; };

	/// <summary>
	/// BGM�̔ԍ����擾����
	/// </summary>
	/// <returns>BGM�̔ԍ�</returns>
	int GetBgmNumber() { return m_bgmNumber; }

private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

private:

	using UpdateSelectFunc = void (SelectManager::*)();

	//�X�V�֐�
	UpdateSelectFunc m_updateSelectFunc;

	//UI�̃|�C���^
	std::shared_ptr<SelectUi> m_pUi;

	//�I�𒆂̔ԍ�
	int m_selectNumber;

	//�v���C���[�̔ԍ�
	int m_playerNumber;

	//�G�l�~�[�̔ԍ�
	int m_enemyNumber;

	//���̃V�[��
	int m_nextScene;

	//�G�l�~�[��AI���x��
	int m_enemyLevel;

	//���Đ����Ă���BGM�̔ԍ�
	int m_bgmNumber;
};

