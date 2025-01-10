#pragma once
class SelectManager
{
public:
	SelectManager();

	virtual ~SelectManager();

	void Update();

	/// <summary>
	/// 1�v���C���[���̃L�����N�^�[�̔ԍ���Ԃ�
	/// </summary>
	/// <returns>1�v���C���[�̃L�����N�^�[�̔ԍ���Ԃ�</returns>
	int GetPlayerNumber() { return m_playerNumber; }

	/// <summary>
	/// 2�v���C���[���̃L�����N�^�[�̔ԍ���Ԃ�
	/// </summary>
	/// <returns>2�v���C���[�̃L�����N�^�[�̔ԍ���Ԃ�</returns>
	int GetEnemyNumber() { return m_enemyNumber; }

	/// <summary>
	/// ���̃t���[���ňڍs����V�[�����擾����
	/// </summary>
	/// <returns>���̃V�[��(Game::Scene�L���X�g���Ďg�p)</returns>
	int GetNextScene() {return m_nextScene};
private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

	void ConfirmCharacter();

private:

	using UpdateSelectFunc = void (SelectManager::*)();

	UpdateSelectFunc m_updateSelectFunc;

	int m_playerNumber;

	int m_enemyNumber;

	int m_nextScene;
};

