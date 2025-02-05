#pragma once
#include <string>
#include "Vector2.h"
#include <vector>
#include <map>


class TutorialUi
{
public:

	enum class PlayMenuItem
	{
		kReset,
		kChangeTutorial,
		kChangeMode,
		kSelectMenu,
		kMenuEnd,
		kTutorialEnd,
		kItemEnd = kTutorialEnd,
		kItemNum
	};

	enum class StartMenuItem
	{
		kStartTutorial,
		kSelectTutorial,
		kEnd,
		kItemEnd = kEnd,
		kItemNum
	};

public:

	TutorialUi();

	virtual ~TutorialUi();

	void Init();

	void Update();

	void Final();

	void InitStartMenu();

	void InitPlayMenu();

	void InitStart();

	void InitSelectMenu();

	void InitPlaying(int number);

	void InitSuccess();

	void DrawStartMenu();

	void DrawSelectMenu();

	void DrawPlayMenu();

	void DrawStart(int number);

	void DrawPlaying(int number);

	void DrawSuccess(int number);


	/// <summary>
	/// ���s���Ă���`���[�g���A�����N���A�������ǂ�����ݒ肷��
	/// </summary>
	void SetSuccessTutorial(bool flag) { m_isSuccessTutorial = flag; }
	
	/// <summary>
	/// ���j���[��ʂőI���������̂��擾����
	/// </summary>
	/// <returns>�Ȃɂ��I�����Ă��Ȃ��Ƃ���-1��Ԃ�</returns>
	PlayMenuItem GetPlayMenuSelectItem() { return m_playMenuSelectItem; };

	/// <summary>
	/// �X�^�[�g��ʂőI���������̂��擾����
	/// </summary>
	/// <returns>�����I�����Ă��Ȃ��Ƃ���-1��Ԃ�</returns>
	StartMenuItem GetStartMenuSelectItem() { return m_startMenuSelectItem; }

	/// <summary>
	/// �I�������`���[�g���A��
	/// </summary>
	/// <returns>���j���[��ʂőI�������`���[�g���A����Ԃ�</returns>
	int GetTutorialNumber() { return m_selectTutorialNumber; }

	/// <summary>
	/// �Z���N�g���j���[�őI�����Ă���`���[�g���A���̖��O���擾����
	/// </summary>
	/// <returns>�`���[�g���A���̖��O</returns>
	std::string GetSelectTutorialName() { return m_tutorialSelectMenuData[m_selectMenuIndexX + 1][m_selectMenuIndexY]; }

	/// <summary>
	/// ���ݑI�����Ă���`���[�g���A���ԍ���ݒ肷��
	/// </summary>
	/// <param name="number">���ݍs���Ă���`���[�g���A���ԍ�</param>
	void SetTutorialNumber(int number) { m_selectTutorialNumber = number; }

	/// <summary>
	/// ���ݑI�����Ă���`���[�g���A���̃��[�h���擾����
	/// </summary>
	/// <returns></returns>
	int GetNowTutorialMode() { return m_nowTutorialMode; }

	/// <summary>
	/// �N���A���o���I��������ǂ�����Ԃ�
	/// </summary>
	bool IsSuccessEnd() { return m_isSuccessEnd; }

	/// <summary>
	/// �X�^�[�g���j���[�őI���ł��邩�ǂ������擾����
	/// </summary>
	/// <returns>�I���ł���̂Ȃ�true</returns>
	bool IsSelectStartMenu() { return m_isSelectStartMenu; }

	/// <summary>
	/// �Z���N�g���j���[�őI���ł��邩�ǂ������擾����
	/// </summary>
	/// <returns>�I���ł���̂Ȃ�</returns>
	bool IsSelectSelectMenu() { return m_isSelectSelectMenu; }

	/// <summary>
	/// �`���[�g���A���̃f�[�^��ݒ肷��
	/// </summary>
	/// <param name="data">�O������ǂݍ��񂾃f�[�^</param>
	void SetTutorialPlayData(std::vector<std::vector<std::string>> data) { m_tutorialPlayData = data; }

private:

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int fontHandle, int color, int edgeColor);

	void UpdateStartMenu();

	void UpdateSelectMenu();

	void UpdatePlayMenu();

	void UpdateStart();

	void UpdatePlaying();

	void UpdateSuccess();

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

	//�`���[�g���A���̃v���C�f�[�^
	std::vector<std::vector<std::string>> m_tutorialPlayData;

	//�`���[�g���A���̃X�^�[�g���j���[�̃f�[�^
	std::vector<std::vector<std::string>> m_tutorialSelectMenuData;

	//�J�n���̕����̃t�H���g�n���h��
	int m_startFontHandle;

	//�v���C���̐����̃t�H���g�n���h��
	int m_playingFontHandle;

	//���j���[�̕����̃t�H���g�n���h��
	int m_playMenuFontHandle;

	//�X�^�[�g���j���[�̕����̃t�H���g�n���h��
	int m_startMenuFontHandle;

	//���j���[��ʂőI�����Ă��鍀�ڂ�����������̃t�H���g�n���h��
	int m_selectMenuFontHandle;

	//���j���[��ʂőI�����Ă��鍀�ڂ�����������̃t�H���g�n���h��
	int m_selectTutorialKindFontHandle;

	//�\�����Ă���{�^���̐�
	int m_drawButtonNum;

	//�I�����Ă���`���[�g���A���̔ԍ�
	int m_selectTutorialNumber;

	//���s���Ă���`���[�g���A���̃��[�h
	int m_nowTutorialMode;

	//�I�����Ă���`���[�g���A���̍��E�̖��̗h�ꕝ
	float m_selectTutorialStringArrowPos;

	//�`�悵�Ă���摜�z��
	std::map<std::string, GraphData> m_drawGraphs;

	//���j���[��ʂŌ��ݑI�����Ă��鍀��
	PlayMenuItem m_playMenuSelectItem;

	//�X�^�[�g���j���[�Ō��ݑI�����Ă��鍀��
	StartMenuItem m_startMenuSelectItem;

	//�Z���N�g���j���[�Ō��ݑI�����Ă��鍀��
	int m_selectMenuIndexX;
	int m_selectMenuIndexY;

	//���j���[��ʂőO�񍀖ڂ𓮂����Ă���̎���
	int m_selectItemMoveTime;

	//���ݍs���Ă���`���[�g���A��
	int m_tutorialNumber;

	//���ݍs���Ă���`���[�g���A�����N���A������true�ɂ���
	bool m_isSuccessTutorial;

	//�N���A���̉摜��h�炵�Ă��鎞��
	int m_successTime;

	//�N���A�̉��o���I��������ǂ���
	bool m_isSuccessEnd;

	//�X�^�[�g���j���[�őI���ł��邩�ǂ���
	bool m_isSelectStartMenu;

	//�Z���N�g���j���[�őI���ł��邩�ǂ���
	bool m_isSelectSelectMenu;

};

