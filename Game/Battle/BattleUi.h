#pragma once
#include "Vector2.h"
#include <string>

class BattleUi
{
public:

	enum class UiSituation
	{
		kMenu,
		kResult,
		kStart1P,
		kStart2P,
		kNone
	};

	enum class MenuItem
	{
		kReturnBattle,
		kRetry,
		kBackCharacterSelect,
		kBackMenu,
		kItemEnd = kBackMenu,
		kItemNum
	};

	enum class ResultItem
	{
		kRetry,
		kCharacterSelect,
		kMenu,
		kItemEnd = kMenu,
		kItemNum
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

	/// <summary>
	/// �ǂ̍��ڂ�I�����������擾����
	/// </summary>
	/// <returns>�I����������,�����I�����Ă��Ȃ����-1</returns>
	int GetDecisionItem() { return m_decisionItem; }


	/// <summary>
	/// ���U���g���j���[���\������Ă��邩�ǂ�����ݒ肷��
	///	</summary>
	/// <param name= "flag">���U���g���j���[��\������Ȃ�true</param>
	void SetResultMenuDisplay(bool flag) { m_isResultMenuDisplay = flag; }

private:

	void UpdateMenu();

	void UpdateResult();

	void UpdateStart1P();

	void UpdateStart2P();

	void DrawMenu();

	void DrawResult();

	void DrawStart1P();

	void DrawStart2P();

	/// <summary>
	/// �Ȃɂ����Ȃ�
	/// </summary>
	void None() {};

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor);

private:

	using UpdateSituationFunc = void (BattleUi::*)();
	using DrawSituationFunc = void (BattleUi::*)();

	UpdateSituationFunc m_updateFunc;
	DrawSituationFunc m_drawFunc;

	//���U���g�̃��j���[�̃t�H���g�n���h��
	int m_menuFontHandle;

	//���U���g��ʂ̕`����n�߂Ă���̎��Ԃ��v������
	int m_resultTime;

	//���U���g��ʂ̃��S�̊g�嗦
	double m_resultLogoScale;

	//���U���g�̃��j���[�̃A���t�@�l
	int m_resultMenuAlpha;

	//���j���[�̃A���t�@�l
	int m_menuAlpha;

	//���t���[���h�炵����
	int m_shakeTime;

	//�I�����Ă��鍀��
	int m_selectItem;

	//���������������
	int m_decisionItem;

	//���������ǂ�����ݒ肷��
	bool m_isWin;

	//���U���g���j���[���\������Ă��邩�ǂ���
	bool m_isResultMenuDisplay;

	//�O��J�[�\���𓮂����Ă��牽�t���[����������
	int m_selectItemMoveTime;

	//�I�����Ă��鍀�ڂ̌��̉摜�̑傫��
	double m_selectItemBoxScale;

};