#pragma once
#include <map>
class SelectUi
{
private:
	enum class GraphName
	{
		kLastPlayer,
		kPlayer,
		kLastEnemy,
		kEnemy,
		kIcon0,
		kIcon1,
		kIcon2,
		kIconFrame,
		kHeadSet,
		kVs
	};

	struct GraphData
	{
		int posX = 0;
		int posY = 0;
		double scale = 1.0;
		int alpha = 255;
		int handle = -1;
	};

public:

	SelectUi();
	virtual ~SelectUi();
	
	void Init();

	void Update();

	void Draw();

	/// <summary>
	/// �ǂ̃L�����N�^�[��I�����Ă��邩��ݒ肷��
	/// </summary>
	/// <param name="number">�I�����Ă���ԍ�</param>
	/// <param name="isPlayer">1P���Ȃ�true</param>
	void SetNumber(int number,bool isPlayer);

	/// <summary>
	/// �ǂ̃A�C�R����I�����Ă��邩��ݒ肷��
	/// </summary>
	/// <param name="number">�I�����Ă���A�C�R��</param>
	void SetIconFrame(int number);

	/// <summary>
	/// �I�����Ă���A�C�R���������t���[����`�悷�邩�ǂ�����ݒ肷��
	/// </summary>
	/// <param name="flag">�`�悷��Ȃ�true</param>
	void SetDrawIconFrame(bool flag);

private:

	std::map<GraphName,GraphData> m_drawGraphs;

	//���\�����Ă���1P���̃L�����N�^�[
	int m_playerNumber;

	//�ЂƂO�ɕ\�����Ă���1P���̃L�����N�^�[
	int m_lastPlayerNumber;

	//���\�����Ă���2P���̃L�����N�^�[
	int m_enemyNumber;

	//�ЂƂO�ɕ\�����Ă���2P���̃L�����N�^�[
	int m_lastEnemyNumber;

	//�X�J�C�h�[���̃n���h��
	int m_skyDomeHandle;

	//�I�����Ă���A�C�R���̃t���[���̊g�k�̂��߂̕ϐ�
	float m_iconFrameScalling;
};

