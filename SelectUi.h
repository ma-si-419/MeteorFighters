#pragma once
#include <map>
#include <string>

class SelectUi
{
public:
	enum class UiSituation
	{
		kSelect1P,
		kSelect2P
	};

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
		kIconFrame1P,
		kIconFrame2P,
		kIconFrameBoth,
		kHeadSet,
		kVs,
		kLeftArrow,
		kRightArrow,
		kRB,
		kLB,
		kMusicNote
	};

	struct GraphData
	{
		int posX = 0;
		int posY = 0;
		double scale = 1.0;
		int alpha = 255;
		int handle = -1;
		bool isReverseX = false;
	};

public:

	SelectUi();
	virtual ~SelectUi();
	
	void Init();

	void Update();

	void Draw();

	/// <summary>
	/// ���ݑI�����Ă��郌�x����ݒ肷��
	/// </summary>
	/// <param name="level">�I�����Ă��郌�x��</param>
	void SetLevel(int level) { m_level = level; }

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
	/// <param name="isPlayer">1P���Ȃ�true</param>
	void SetIconFrame(int number,bool isPlayer);

	/// <summary>
	/// ���݂̏󋵂�ݒ肷��
	/// </summary>
	/// <param name="situation">���̏�</param>
	void ChangeSituation(UiSituation situation);

	/// <summary>
	/// �X�J�C�h�[���̃e�N�X�`���n���h����ݒ肷��
	/// </summary>
	/// <param name="handle">�摜�̃n���h��</param>
	void SetSkyDomeHandle(int handle);

	/// <summary>
	/// �������Ă���Ȃ̖��O��ݒ肷��
	/// </summary>
	/// <param name="musicName">�Ȃ̖��O</param>
	void SetMusicName(std::string musicName);

private:

	void Update1P();

	void Update2P();

private:

	using UpdateFunc = void (SelectUi::*)();

	//�X�V�֐�
	UpdateFunc m_updateFunc;

	std::map<GraphName,GraphData> m_drawGraphs;

	//��Փx��\������t�H���g�n���h��
	int m_levelFontHandle;

	//�Ȗ���\������t�H���g�n���h��
	int m_musicNameFontHandle;

	//�I�����Ă��郌�x��
	int m_level;

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

	//���݂̏�
	UiSituation m_situation;

	//���Ԍv���p�̕ϐ�
	float m_time;

	//�Ȃ̖��O
	std::string m_musicName;

	//�Ȗ��̃A���t�@�l
	int m_musicNameAlpha;

	//�Ȗ��̕\�����W
	int m_musicNamePosX;
};