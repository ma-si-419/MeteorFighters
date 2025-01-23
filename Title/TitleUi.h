#pragma once
#include "Vector2.h"
struct CharacterDrawInfo;
class TitleUi
{
public:

	/// <summary>
	/// �`�悷��L�����N�^�[�̎��
	/// </summary>
	enum class DrawCharacterKind
	{
		kMouse,
		kBlue,
		kMouse2P,
		kBlue2P,
		kCharacterKindNum
	};

	/// <summary>
	/// �L�����N�^�[�̕`��T�C�h
	/// </summary>
	enum class DrawCharacterSide
	{
		kUp,
		kRight,
		kDown,
		kLeft,
		kSideNum = kLeft
	};

private:

	/// <summary>
	/// �L�����N�^�[�̉摜�̎��
	/// </summary>
	enum class CharacterGraphKind
	{
		kChoki,
		kPar,
		kKindNum = kPar
	};

	/// <summary>
	/// �L�����N�^�[�̃A�b�v�f�[�g�̎��
	/// </summary>
	enum class CharacterUpdateKind
	{
		kNormal,
		kFall,
		kSideJump,
		kBackFry,
		kJump,
		kUpdateKindNum = kJump
	};

public:

	TitleUi();

	~TitleUi();

	void Init();

	void Update();

	void Draw();

private:

	/// <summary>
	/// �L�����N�^�[����ʂɓ����Ă���Ƃ��̈ړ��x�N�g�����擾����
	/// </summary>
	/// <param name="side">�L�����N�^�[�̂���T�C�h</param>
	/// <returns>�ړ��x�N�g��</returns>
	MyEngine::Vector2 GetEnterMoveVec(DrawCharacterSide side);

	/// <summary>
	/// �L�����N�^�[����ʂ���o�Ă����Ƃ��̈ړ��x�N�g�����擾����
	/// </summary>
	/// <param name="side">�L�����N�^�[�̂���T�C�h</param>
	/// <returns>�ړ��x�N�g��</returns>
	MyEngine::Vector2 GetReturnMoveVec(DrawCharacterSide side);

	/// <summary>
	/// �L�����N�^�[�̍��W�𐧌�����
	/// </summary>
	/// <param name="side">�L�����N�^�[�̂���T�C�h</param>
	/// <param name="pos">�L�����N�^�[�̍��W</param>
	/// <returns>������̃L�����N�^�[�̍��W</returns>
	MyEngine::Vector2 ClampPos(DrawCharacterSide side, MyEngine::Vector2 pos);

	/// <summary>
	/// �w�肵���T�C�h�Ƀ����_���ȍ��W��ݒ肷��
	/// </summary>
	/// <param name="side">�L�����N�^�[�̃T�C�h</param>
	/// <returns>�L�����N�^�[�̕`����W</returns>
	MyEngine::Vector2 SetRandomPos(DrawCharacterSide side);

	/// <summary>
	/// ��ʓ��ɂ��邩�ǂ�����Ԃ�
	/// </summary>
	/// <param name="pos">�L�����N�^�[�̍��W</param>
	/// <returns>��ʂɈڂ��Ă���̂Ȃ��true��Ԃ�</returns>
	bool CheckPos(MyEngine::Vector2 pos);

	/// <summary>
	/// �L�����N�^�[�̉摜�n���h�����擾����
	/// </summary>
	/// <param name="characterKind">�L�����N�^�[�̎��</param>
	/// <param name="graphKind">�摜�̎��</param>
	/// <returns>�L�����N�^�[�̉摜�n���h��</returns>
	int GetCharacterHandle(DrawCharacterKind characterKind, CharacterGraphKind graphKind);

	/// <summary>
	/// �L�����N�^�[�̍X�V��ݒ肷��
	/// </summary>
	/// <param name="kind">�X�V�̎��</param>
	/// <param name="character">�L�����N�^�[�̎��</param>
	void SetCharacterUpdateFunc(CharacterUpdateKind kind, DrawCharacterKind character);

private:

	using CharacterUpdateFunc = void(TitleUi::*)(int);

private:

	/// <summary>
	/// �L�����N�^�[��`�悷��ۂɎg�p����\����
	/// </summary>
	struct CharacterDrawInfo
	{
		//�L�����N�^�[�̃n���h��
		int handle = -1;
		//�L�����N�^�[�̍��W
		MyEngine::Vector2 pos;
		//�L�����N�^�[�̈ړ��x�N�g��
		MyEngine::Vector2 moveVec;
		//�L�����N�^�[�̉�]�x
		double rota = 0.0;
		//�L�����N�^�[�̊g�嗦
		double scale = 1.0;
		//�L�����N�^�[�̉�]���x
		double rotaSpeed = 0.0;
		//�L�����N�^�[�̃T�C�h
		DrawCharacterSide side = DrawCharacterSide::kUp;
		//�L�����N�^�[�̈ړ�����
		int moveTime = 0;
		//�L�����N�^�[�̎��
		DrawCharacterKind kind = DrawCharacterKind::kMouse;
		//�L�����N�^�[�̍X�V�֐�
		CharacterUpdateFunc updateFunc = &TitleUi::NormalUpdate;
	};

private:

	/// <summary>
	/// �ʏ펞�̏�����
	/// </summary>
	/// <param name="number">���������s���L�����N�^�[</param>
	void InitNormal(int number);

	/// <summary>
	/// �ォ�痎���Ă���Ƃ��̏�����
	/// </summary>
	/// <param name="number">���������s���L�����N�^�[</param>
	void InitFall(int number);

	/// <summary>
	/// ������W�����v����Ƃ��̏�����
	/// </summary>
	/// <param name="number">���������s���L�����N�^�[</param>
	void InitJump(int number);

	/// <summary>
	/// ������т���Ƃ��̏�����
	/// </summary>
	/// <param name="number">���������s���L�����N�^�[</param>
	void InitSideJump(int number);

	/// <summary>
	/// ������ł����L�����N�^�[
	/// </summary>
	/// <param name="number">���������s���L�����N�^�[</param>
	void InitBackFry(int number);

	/// <summary>
	/// �ʏ펞�̃A�b�v�f�[�g
	/// </summary>
	/// <param name="character">�A�b�v�f�[�g���s���L�����N�^�[</param>
	void NormalUpdate(int number);

	/// <summary>
	/// �ォ�痎���Ă���Ƃ��̃A�b�v�f�[�g
	/// </summary>
	/// <param name="character">�A�b�v�f�[�g���s���L�����N�^�[</param>
	void FallUpdate(int number);

	/// <summary>
	/// ������W�����v����Ƃ��̃A�b�v�f�[�g
	/// </summary>
	/// <param name="character">�A�b�v�f�[�g���s���L�����N�^�[</param>
	void JumpUpdate(int number);

	/// <summary>
	/// ������т���Ƃ��̃A�b�v�f�[�g
	/// </summary>
	/// <param name="number">�A�b�v�f�[�g���s���L�����N�^�[</param>
	void SideJumpUpdate(int number);

	/// <summary>
	/// ������ł����Ƃ��̃A�b�v�f�[�g
	/// </summary>
	/// <param name="number">�A�b�v�f�[�g���s���L�����N�^�[</param>
	void BackFryUpdate(int number);

private:
	//���������Ă��鎞��
	int m_time;

	//�������\������Ă��邩�ǂ���
	bool m_isExistString;

	//�t�H���g�̃n���h��
	int m_fontHandle;

	//�X�J�C�h�[���̃n���h��
	int m_skyDomeHandle;

	//�L�����N�^�[�̕`����
	CharacterDrawInfo m_characterDrawInfo[static_cast<int>(DrawCharacterKind::kCharacterKindNum)];
};

