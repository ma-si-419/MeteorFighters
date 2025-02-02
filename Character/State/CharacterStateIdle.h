#pragma once
#include "CharacterStateBase.h"
class CharacterStateIdle : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateIdle>
{
public:
	CharacterStateIdle(std::shared_ptr<Character> character);

	/// <summary>
	/// �X�e�[�g�̏I���A�j���[�V�������Đ�����(�u�����h�̑����Œ�)
	/// </summary>
	/// <param name="kind">�Đ�����A�j���[�V�����ԍ�</param>
	/// <param name="time">�Đ����鎞��</param>
	void SetEndAnim(int kind, int time);

	/// <summary>
	/// �X�e�[�g�̏I���A�j���[�V�������Đ�����(�u�����h�̑�����)
	/// </summary>
	/// <param name="kind">�Đ�����A�j���[�V�����ԍ�</param>
	/// <param name="time">�Đ����鎞��</param>
	/// <param name="blendSpeed">�A�j���[�V�����u�����h�̑���</param>
	void SetEndAnim(int kind, int time, float blendSpeed);

	/// <summary>
	/// �O�̏�Ԃ��U���������ꍇ�Ă�
	/// </summary>
	void SetLastAttackState();

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	bool m_isPlayEndAnim;

	int m_endAnimTime;

	bool m_isReleaseX;

	bool m_isReleaseY;

	//�󒆂ɂ���Ƃ��ɗh�炷���߂Ɏg���ϐ�
	float m_swayCount;

};

