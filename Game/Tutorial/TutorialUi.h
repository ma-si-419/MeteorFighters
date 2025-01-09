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

	void InitSuccess();

	void DrawMenu();

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
	MenuItem GetSelectItem() { return m_selectItem; };

	/// <summary>
	/// �I�������`���[�g���A��
	/// </summary>
	/// <returns>���j���[��ʂőI�������`���[�g���A����Ԃ�</returns>
	int GetTutorialNumber() { return m_selectTutorialNumber; }

	/// <summary>
	/// ���ݑI�����Ă���`���[�g���A���ԍ���ݒ肷��
	/// </summary>
	/// <param name="number">���ݍs���Ă���`���[�g���A���ԍ�</param>
	void SetNowTutorialNumber(int number) { m_selectTutorialNumber = number; }

	/// <summary>
	/// �N���A���o���I��������ǂ�����Ԃ�
	/// </summary>
	bool IsSuccessEnd() { return m_isSuccessEnd; }

private:

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int fontHandle, int color, int edgeColor);

	void UpdateMenu();

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

	//�I�����Ă���`���[�g���A���̔ԍ�
	int m_selectTutorialNumber;

	//�I�����Ă���`���[�g���A���̍��E�̖��̗h�ꕝ
	float m_selectTutorialStringArrowPos;

	//�`�悵�Ă���摜�z��
	std::map<std::string, GraphData> m_drawGraphs;

	//���j���[��ʂŌ��ݑI�����Ă��鍀��
	MenuItem m_selectItem;

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
};

