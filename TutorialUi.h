#pragma once
#include <string>
#include "Vector2.h"
#include <vector>
#include <map>

class TutorialUi
{
public:

	enum class MenuItem
	{
		kReset,
		kChangeTutorial,
		kMenuEnd,
		kTutorialEnd,
		kItemEnd = kTutorialEnd,
		kItemNum
	};

public:

	TutorialUi();

	virtual ~TutorialUi();

	void Init();

	void Update();

	void Final();

	void InitMenu();

	void InitStart();

	void InitPlaying(int number);

	void DrawMenu();

	void DrawStart(int number);

	void DrawPlaying(int number);

	
	/// <summary>
	/// ���j���[��ʂőI���������̂��擾����
	/// </summary>
	/// <returns>�Ȃɂ��I�����Ă��Ȃ��Ƃ���-1��Ԃ�</returns>
	MenuItem GetSelectItem() { return m_selectItem; };

private:

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int fontHandle, int color, int edgeColor);

	void UpdateMenu();

	void UpdateStart();

	void UpdatePlaying();

private:

	struct GraphData
	{
		int handle = -1;
		MyEngine::Vector2 pos;
		double scale = 1.0;
		int alpha = 255;
	};

private:

	using UpdateFunc = void (TutorialUi::*)();

	UpdateFunc m_updateFunc;

	//�`���[�g���A����UI�f�[�^
	std::vector<std::vector<std::string>> m_tutorialData;

	//�J�n���̕����̃t�H���g�n���h��
	int m_startFontHandle;

	//�v���C���̐����̃t�H���g�n���h��
	int m_playingFontHandle;

	//���j���[�̕����̃t�H���g�n���h��
	int m_menuFontHandle;

	//�\�����Ă���{�^���̐�
	int m_drawButtonNum;

	//�`�悵�Ă���摜�z��
	std::map<std::string, GraphData> m_drawGraphs;

	//���j���[��ʂŌ��ݑI�����Ă��鍀��
	MenuItem m_selectItem;

	//���j���[��ʂőO�񍀖ڂ𓮂����Ă���̎���
	int m_selectItemMoveTime;

	//���ݍs���Ă���`���[�g���A��
	int m_tutorialNumber;
};

