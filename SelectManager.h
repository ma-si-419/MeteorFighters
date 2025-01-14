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
	/// ���̃t���[���ňڍs����V�[�����擾����
	/// </summary>
	/// <returns>���̃V�[��(Game::Scene�L���X�g���Ďg�p)</returns>
	int GetNextScene() { return m_nextScene; };
private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

	void ConfirmCharacter();

private:

	using UpdateSelectFunc = void (SelectManager::*)();

	UpdateSelectFunc m_updateSelectFunc;

	std::shared_ptr<SelectUi> m_pUi;

	int m_selectNumber;

	int m_playerNumber;

	int m_enemyNumber;

	int m_nextScene;
};

