#pragma once
class BattleUi
{
public:

	enum class UiSituation
	{
		kMenu,
		kResult,
		kNone
	};

	enum class ResultItem
	{
		kRetry,
		kCharacterSelect,
		kMenu,
		kItemNum = kMenu
	};

public:

	BattleUi();

	virtual ~BattleUi();

	void Update();

	void Draw();

	void ChangeSituation(UiSituation situation);

	/// <summary>
	/// ���������ǂ�����ݒ肷��
	/// </summary>
	/// <param name="flag">�����Ă���Ȃ�true�����Ă���Ȃ�false������</param>
	void SetResult(bool flag) { m_isWin = flag; }

private:

	void UpdateMenu();

	void UpdateResult();

	void DrawMenu();

	void DrawResult();

	/// <summary>
	/// �Ȃɂ����Ȃ�
	/// </summary>
	void None() {};

private:

	using UpdateSituationFunc = void (BattleUi::*)();
	using DrawSituationFunc = void (BattleUi::*)();

	UpdateSituationFunc m_updateFunc;
	DrawSituationFunc m_drawFunc;

	//���U���g��ʂ̕`����n�߂Ă���̎��Ԃ��v������
	int m_resultTime;

	//���U���g��ʂ̃��S�̊g�嗦
	double m_resultLogoScale;

	//���t���[���h�炵����
	int m_shakeTime;

	//�I�����Ă��鍀��
	int m_selectItem;

	//���������������
	int m_decisionItem;

	//���������ǂ�����ݒ肷��
	bool m_isWin;

	//�O��J�[�\���𓮂����Ă��牽�t���[����������
	int m_selectItemMoveTime;
};