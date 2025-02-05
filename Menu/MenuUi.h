#pragma once
#include <string>
#include <map>
class MenuUi
{
public:
	enum class SelectItem
	{
		kSelect,
		kTutorial,
		kOption,
		kEndGame,
		kItemEnd = kEndGame,
		kItemNum
	};

private:

	struct StringUiStatus
	{
		std::string showString = "empty";
		int showPosX = 0;
		int showPosY = 0;
	};

	struct GraphUiStatus
	{
		int handle = -1;
		int showPosX = 0;
		int showPosY = 0;
		int alpha = 255;
		int srcPosX = 0;
		int graphSizeX = 0;
		int graphSizeY = 0;
	};

public:

	MenuUi();

	~MenuUi();

	void Init();

	int Update();

	void DrawItem();

	void DrawModel();
	
	void EntryGraph(std::string name,GraphUiStatus status);

	GraphUiStatus& GetGraphRef(std::string name);

private:

	void UpdateCommon();

	int UpdateSelect();

	int UpdateOption();

	int UpdateEndGame();

	void DrawOption();

	void DrawEndGame();

	void DrawNone() {};//�������Ȃ�

private:

	using UpdateFunc = int (MenuUi::*)();

	using DrawFunc = void (MenuUi::*)();

	//�X�V�֐�
	UpdateFunc m_updateFunc;

	//�`��֐�
	DrawFunc m_drawFunc;

	SelectItem m_lastSelectItem;

	SelectItem m_selectItem;

	StringUiStatus m_selectStringUi[static_cast<int>(SelectItem::kItemNum)];

	std::map<std::string, GraphUiStatus> m_selectGraphMap;

	int m_selectItemFontHandle;

	int m_skyDomeHandle;

	int m_optionFontHandle;

	int m_optionButtonFontHandle;

	//���j���[��ʂőO�񍀖ڂ𓮂����Ă���̎���
	int m_selectItemMoveTime;

	//�E���ɕ\������摜�̐؂�����W
	int m_rightGraphSrcNum;

	//�E���ɕ\������摜��؂�����W��؂�ւ��鎞��
	int m_rightGraphChangeTime;

	//�I�v�V������ʂőI�����Ă��鍀��
	int m_optionSelectItem;

	//�I�v�V������ʂ̃A���t�@�l
	int m_optionAlpha;

	//�I�v�V������ʂ��I���Ƃ���true�ɂ���
	bool m_isEndOption;

	//�V�[�N�o�[�𓮂������ǂ���
	bool m_isMoveSeekBar;

	//�V�[�N�o�[�̂܂݂��g�k������ۂɎg�p����ϐ�
	float m_thumbSize;

	//���ʂ̐ݒ�
	float m_masterVolume;
	float m_seVolume;
	float m_bgmVolume;
	float m_voiceVolume;

};